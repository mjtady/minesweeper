//
// Created by mjtad on 4/18/2024.
//
#include <iostream>
#include <cctype>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <SFML/Graphics.hpp>

using namespace std;

#ifndef MINESWEEPER_GAME_H
#define MINESWEEPER_GAME_H

using namespace std;

class Timer{
private:
    int elapsedtime;
    bool pause;
    std::chrono::time_point<std::chrono::high_resolution_clock> starttime;

public:
    Timer();
    int getTime();
    void stopTime();
    void restartTime();
};

class GameWindow{
    struct Tile{
        bool ifmine = false;
        bool ifflagged = false;
        bool ifrevealed = false;
        bool hiddentile = true;
        unsigned int row, col, nearbymines;
        vector<Tile*> neighbors;
        Tile(int row, int col, int nearbymines); //creates a tile object while also keeping track of how many mines are in a 3x3 square
    };
private:
    unsigned int col, row, mines_num;
    int totalElapsedTime = 0;
    int totalflags = 0;
    int flagcount = 0;
    bool ifpaused = false;
    bool debugmines = false;
    bool leaderboardopen = false;
    bool allrevealed = false;
    bool mineclicked = false;
    bool leaderboardwritten = false;
    Timer clock;
    sf::RenderWindow game;
    sf::Font font;
    sf::Texture hiddentex, revealedtex, flagtex, minetex,
        happyface, loseface, winface, debugtex, leaderboard, pause_button, play_button,
        digitstex, num1, num2, num3, num4, num5, num6, num7, num8;
    sf::Sprite face, debug, pause, leader_board;
    vector<vector<Tile*>> board, temp_board;
    map<int, sf::Sprite> digitsMap;
    std::chrono::steady_clock::time_point starttime;
    std::chrono::steady_clock::time_point currtime;
    std::chrono::steady_clock::time_point pausetime;
    std::chrono::seconds winningTime;


    void loadBoard(int row, int col);
    void loadEvent(const sf::Event& event);
    void placeMines(int mines_num);
    void adjacentMines(); //checks the number of adjacent mines in a 2D vector and stores the calculated value in a 1D vector
    void revealTile(int mousePos_x, int mousePos_y); //reveals the tiles
    void flagTile(int mousePos_x, int mousePos_y); //flags the tiles
    void counter();
    bool loseGame(); //returns a boolean to check if the player lost the game
    bool winGame(); //returns a boolean to check if the player won the game
    void restartGame(); //resets game
    void writeLeaderboard(float time, const string& player); //writing into leaderboard.txt
    void drawTimer(int elapsed); //creates a game timer
    map<int, sf::Sprite> parseDigits(sf::Sprite digits);
    void draw();

public:
    GameWindow();
    void run();
};




#endif //MINESWEEPER_GAME_H
