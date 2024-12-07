//
// Created by mjtad on 4/18/2024.
//

#ifndef MINESWEEPER_LEADERBOARD_H
#define MINESWEEPER_LEADERBOARD_H

#include <iostream>
#include <cctype>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <SFML/Graphics.hpp>

using namespace std;
class Leaderboard{
public:
    Leaderboard();
    void writeLeaderboard(float time, const string &player);
    void readLeaderboard();
    void updateLeaderboard();
    void run();

private:
    unsigned int row, col;
    string player, newentry;
    float time;
    sf::RenderWindow leaderboard;
    sf::Font font;
    vector<pair<string, float>> person;
    void draw();
};


#endif //MINESWEEPER_LEADERBOARD_H
