#include <iostream>
#include <cctype>
#include <fstream>
#include <sstream>
#include "welcome.h"
#include "game.h"
#include "leaderboard.h"
#include <SFML/Graphics.hpp>
#include <SFML/config.hpp>

int main() {
    WelcomeScreen welcomeScreen;
    welcomeScreen.run();
}