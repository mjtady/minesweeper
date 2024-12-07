//
// Created by mjtad on 4/18/2024.
//
#include <iostream>
#include <cctype>
#include <string>
#include <fstream>
#include <sstream>
#include <SFML/Graphics.hpp>
#include "welcome.h"
#include "game.h"

using namespace std;

WelcomeScreen::WelcomeScreen(){
    ifstream file("files/config.cfg");
    if(!file.is_open()){
        cerr << "Failed to load file" << endl;
    }
    string line;
    for(int i = 0; i < 3 && getline(file, line); i++){
        istringstream iss(line);
        if(i == 0){
            iss >> col;
        }else if(i == 1){
            iss >> row;
        }
    }

//    window.create(sf::VideoMode(col * 32, (row * 32) + 100), "Minesweeper", sf::Style::Close);
    window.create(sf::VideoMode(800, 612), "Minesweeper", sf::Style::Close);

    font.loadFromFile("files/font.ttf");
    if(!font.loadFromFile("files/font.ttf")){
        cerr << "Failed to load font!" << endl;
        window.close();
    }

    welcome.setFont(font);
    welcome.setString("WELCOME TO MINESWEEPER!");
    welcome.setCharacterSize(24);
    welcome.setFillColor(sf::Color::White);
    welcome.setStyle(sf::Text::Bold | sf::Text::Underlined);

    entername.setFont(font);
    entername.setString("Enter your name:");
    entername.setCharacterSize(20);
    entername.setFillColor(sf::Color::White);
    entername.setStyle(sf::Text::Bold);

    username.setFont(font);
    username.setCharacterSize(18);
    username.setFillColor(sf::Color::Yellow);
    username.setStyle(sf::Text::Bold);
}

void WelcomeScreen::run() {
    while(window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            loadEvent(event);
        }
        draw();
    }
}

void WelcomeScreen::loadEvent(const sf::Event &event) {
    if(event.type == sf::Event::Closed) {
        window.close();
    } else if (event.type == sf::Event::TextEntered) {
        if (event.text.unicode < 128 && isalpha(event.text.unicode)) {
            if (username.getString().getSize() < 10) {
                char typedChar = static_cast<char>(event.text.unicode);
                if (!username.getString().isEmpty()) {
                    typedChar = tolower(typedChar);
                } else {
                    typedChar = toupper(typedChar);
                }
                input += typedChar;
                username.setString(input);
            }
        } else if (event.text.unicode == '\b') {
            if (!input.empty()) {
                input.pop_back();
                username.setString(input);
            }
        } else if (event.text.unicode == '\n' || event.text.unicode == '\r') {
            if (!input.empty()) {
                //cout << "Username input: " << input << endl;
                window.close();
                GameWindow game;
                game.run();
            }
        }
    }
}

void setText(sf::Text &text, float x, float y){
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width/2.0f, textRect.top + textRect.height/2.0f);
    text.setPosition(sf::Vector2f(x, y));
}

void WelcomeScreen::draw() {
    ::setText(welcome, window.getSize().x/2.f, window.getSize().y/2.f - 150);
    ::setText(entername, window.getSize().x/2.f, window.getSize().y/2.f - 75);
    ::setText(username, window.getSize().x/2.f, window.getSize().y/2.f - 45);

    window.clear(sf::Color::Blue);
    window.draw(welcome);
    window.draw(entername);
    window.draw(username);

    if(input.size() <= 10){
        sf::Text cursor("|", font, 18);
        cursor.setFillColor(sf::Color::White);
        ::setText(cursor, username.getPosition().x + username.getLocalBounds().width/2.f + 5.f, username.getPosition().y);
        window.draw(cursor);
    }

    window.display();
}

string WelcomeScreen::getUsernameInput(){
    return input;
}

