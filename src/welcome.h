//
// Created by mjtad on 4/18/2024.
//
#include <iostream>
#include <cctype>
#include <vector>
#include <string>
#include <algorithm>
#include <SFML/Graphics.hpp>

using namespace std;


#ifndef MINESWEEPER_WELCOME_H
#define MINESWEEPER_WELCOME_H

class WelcomeScreen{
private:
    sf::RenderWindow window;
    sf::Text welcome, entername, username;
    sf::Font font;
    static string input;
    unsigned int row{}, col{};

    void loadEvent(const sf::Event& event);
    void draw();
public:
    WelcomeScreen();
    static string getUsernameInput();
    void run();

};

void setText(sf::Text &text, float x, float y);

#endif //MINESWEEPER_WELCOME_H
