//
// Created by mjtad on 4/23/2024.
//
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "leaderboard.h"
#include "welcome.h"
#include <SFML/Graphics.hpp>

using namespace std;
WelcomeScreen newestentry;

Leaderboard::Leaderboard(){
    font.loadFromFile("files/font.ttf");
    if(!font.loadFromFile("files/font.ttf")){
        cerr << "Failed to load font!" << endl;
        leaderboard.close();
    }

    //loads images and textures
    ifstream file("files/config.cfg");
    if(!file.is_open()){
        cerr << "Failed to load file" << endl;
    }
    string line;
    for(int i = 0; i < 3 && getline(file, line); i++) {
        istringstream iss(line);
        if (i == 0) {
            iss >> col;
        } else if (i == 1) {
            iss >> row;
        }
    }

    leaderboard.create(sf::VideoMode(col * 16, (row * 16) + 50), "Minesweeper", sf::Style::Close);
}

void Leaderboard::readLeaderboard() {
    ifstream leaderboardfile("files/leaderboard.txt");
    if (!leaderboardfile.is_open()) {
        cerr << "Failed to open leaderboard file!" << endl;
        return;
    }

    person.clear(); // Clear any previous data
    string txtline;
    while (getline(leaderboardfile, txtline)) {
        stringstream ss(txtline);
        string timeStr, playername;
        getline(ss, timeStr, ',');
        getline(ss, playername);
        //cout << "playername: " << playername << endl;

        stringstream timeStream(timeStr);
        string minuteStr, secondStr; // Change hourStr to minuteStr
        getline(timeStream, minuteStr, ':'); // Parse minutes
        getline(timeStream, secondStr); // Parse seconds

        int minutes, seconds;
        try {
            minutes = stoi(minuteStr); // Parse minutes
            seconds = stoi(secondStr); // Parse seconds
        } catch (const std::invalid_argument& e) {
            cerr << "Failed to convert string to integer: " << e.what() << endl;
            continue; // Skip this entry if conversion fails
        }

        float totaltime = minutes * 60 + seconds; // Calculate total time in seconds

        person.emplace_back(playername, totaltime);
    }

    leaderboardfile.close();
}

void Leaderboard::writeLeaderboard(float time, const string &player) {
    // Extract minutes and seconds
    int minutes = static_cast<int>(time) / 60;
    int seconds = static_cast<int>(time) % 60;

    // Format minutes and seconds with leading zeros if necessary
    std::ostringstream formattedtime;
    formattedtime << std::setfill('0') << std::setw(2) << minutes << ":" << std::setw(2) << seconds;

    std::ofstream leaderboardfile("files/leaderboard.txt", std::ios_base::app);
    if (!leaderboardfile.is_open()) {
        std::cerr << "Error: Failed to open leaderboard file." << std::endl;
        return; // Exit the function if the file couldn't be opened
    }

    // Write the formatted time and player name to the file
    leaderboardfile << formattedtime.str() << "," << player << std::endl;
    leaderboardfile.close();
}

void Leaderboard::updateLeaderboard() {
    readLeaderboard();

    // Sort person vector by time (shortest to longest)
    sort(person.begin(), person.end(), [](const pair<string, float> &a, const pair<string, float> &b) {
        return a.second < b.second;
    });
    // Resize person to 5 (remove values outside of it)
    if (person.size() > 5) {
        person.resize(5);
    }
    // Rewrite leaderboard
    ofstream file("files/leaderboard.txt");
    for (const auto &entry : person) {
        int minutes = static_cast<int>(entry.second / 60);
        int seconds = static_cast<int>(entry.second) % 60;
        string timeStr = to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + to_string(seconds); // Format as "MM:SS"
        file << timeStr << "," << entry.first << "\n";
    }
}

void Leaderboard::run() {
    readLeaderboard();
    while (leaderboard.isOpen()) {
        leaderboard.requestFocus();
        sf::Event event;
        while (leaderboard.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                leaderboard.close();
            }
        }
        draw();
    }
}

void Leaderboard::draw() {
    // Clear the window with a blue color
    string newentry = newestentry.getUsernameInput();
    leaderboard.clear(sf::Color::Blue);

    // Text to display leaderboard
    sf::Text leaderboardText;
    leaderboardText.setFont(font);
    leaderboardText.setCharacterSize(20);
    leaderboardText.setString("LEADERBOARD");
    leaderboardText.setFillColor(sf::Color::White);
    leaderboardText.setStyle(sf::Text::Bold | sf::Text::Underlined);
    ::setText(leaderboardText, (col * 16) / 2.0, ((row * 16 + 50) / 2.0) - 120.0);
    leaderboard.draw(leaderboardText);

    updateLeaderboard();

    stringstream entries;
    sf::Text entriestext;
    entriestext.setFont(font);
    entriestext.setCharacterSize(18);
    entriestext.setFillColor(sf::Color::White);
    entriestext.setStyle(sf::Text::Bold);

    int yPos = (((row * 8) + 25) / 2) + 20;
    int count = 1; // Start with entry number 1
    bool asterisk = false; // Flag to track if asterisk has been added for the newest entry
    for (auto &entry: person) {
        int minutes = static_cast<int>(entry.second / 60); // Calculate total minutes
        int seconds = static_cast<int>(entry.second) % 60; // Calculate remaining seconds

        // Format the time string as "MM:SS"
        stringstream timestream;
        timestream << setfill('0') << setw(2) << minutes << ":" << setw(2) << seconds;
        string timeStr = timestream.str();

        string formatted = to_string(count++) + ". " + timeStr + "\t" + entry.first;

        if (entry.first == newentry && !asterisk) {
            formatted += "*"; // Add asterisk next to the new player's name
            asterisk = true;
        } else {
            // Remove asterisk from previous entries
            size_t pos = formatted.find('*');
            if (pos != string::npos) {
                formatted.erase(pos, 1);
            }
        }

        entries << formatted << "\n\n";
        yPos += 20;
    }

    entriestext.setString(entries.str());
    ::setText(entriestext, (col * 16) / 2.0, ((row * 16 + 50) / 2.0) + 20.0);
    leaderboard.draw(entriestext);

    // Display everything drawn
    leaderboard.display();
}

