//
// Created by mjtad on 4/18/2024.
//
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <chrono>
#include <SFML/Graphics.hpp>
#include "game.h"
#include "leaderboard.h"
#include "welcome.h"

using namespace std;
string WelcomeScreen::input;
WelcomeScreen welcome;
Leaderboard leaderboardchanges;

GameWindow::GameWindow() {
    //loads font;
    Timer newtimer;
    clock = newtimer;
    font.loadFromFile("files/font.ttf");
    if(!font.loadFromFile("files/font.ttf")){
        cerr << "Failed to load font!" << endl;
        game.close();
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
        } else if (i == 2) {
            iss >> mines_num;
        }
    }

    num1.loadFromFile("files/images/number_1.png");
    num2.loadFromFile("files/images/number_2.png");
    num3.loadFromFile("files/images/number_3.png");
    num4.loadFromFile("files/images/number_4.png");
    num5.loadFromFile("files/images/number_5.png");
    num6.loadFromFile("files/images/number_6.png");
    num7.loadFromFile("files/images/number_7.png");
    num8.loadFromFile("files/images/number_8.png");
    hiddentex.loadFromFile("files/images/tile_hidden.png");
    revealedtex.loadFromFile("files/images/tile_revealed.png");
    flagtex.loadFromFile("files/images/flag.png");
    minetex.loadFromFile("files/images/mine.png");
    happyface.loadFromFile("files/images/face_happy.png");
    loseface.loadFromFile("files/images/face_lose.png");
    winface.loadFromFile("files/images/face_win.png");
    debugtex.loadFromFile("files/images/debug.png");
    pause_button.loadFromFile("files/images/pause.png");
    play_button.loadFromFile("files/images/play.png");
    leaderboard.loadFromFile("files/images/leaderboard.png");
    digitstex.loadFromFile("files/images/digits.png");
    digitstex.loadFromFile("files/images/digits.png");

    game.create(sf::VideoMode(col * 32, (row * 32) + 100), "Minesweeper", sf::Style::Close);

}

void GameWindow::run() {
    loadBoard(row, col);
//    cout << welcome.getUsernameInput() << endl;

    while(game.isOpen()) {
        sf::Event event;
        while (game.pollEvent(event)) {
                loadEvent(event);
        }
        draw();
    }
}

void GameWindow::loadEvent(const sf::Event &event) {
    if (event.type == sf::Event::Closed) {
        game.close();
    } else if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos = game.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
            if (face.getGlobalBounds().contains(mousePos)) {
//                cout << "game reset" << endl;
                restartGame();
            }
            if (pause.getGlobalBounds().contains(mousePos) && !winGame() && !mineclicked) {
                ifpaused = !ifpaused;
                clock.stopTime();
//                cout << "pause/play button clicked" << endl;
            } else if (debug.getGlobalBounds().contains(mousePos)) {
//                cout << "debug button clicked" << endl;
                debugmines = !debugmines;
            } else if (leader_board.getGlobalBounds().contains(mousePos)) {
//                cout << "leaderboard button clicked" << endl;
                sf::Sprite tile;
                for (int i = 0; i < col; i++) {
                    for (int j = 0; j < row; j++) {
                        tile.setTexture(revealedtex);
                        tile.setPosition(i * 32, j * 32);
                        game.draw(tile);
                    }
                }
                clock.stopTime();

                game.display();
                Leaderboard leaderboard1;
                leaderboard1.run();
                leaderboardopen = !leaderboardopen;

                clock.stopTime();
            }  else if (!winGame() &&
                       !mineclicked) { // Only allow tile clicks when the game is not won and a mine is not clicked
                int x_pos = static_cast<int>(mousePos.x) / 32;
                int y_pos = static_cast<int>(mousePos.y) / 32;

                if (x_pos >= 0 && x_pos < col && y_pos >= 0 && y_pos < row) {
                    revealTile(x_pos, y_pos);
//                    cout << "Tile clicked: (" << x_pos << ", " << y_pos << ")" << endl;
                }
            }
        } else if (event.mouseButton.button == sf::Mouse::Right) {
            sf::Vector2f mousePos = game.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
            int x_pos = static_cast<int>(mousePos.x) / 32;
            int y_pos = static_cast<int>(mousePos.y) / 32;

            if (x_pos >= 0 && x_pos < col && y_pos >= 0 && y_pos < row) {
                // Check if the tile is flagged
                if (board[x_pos][y_pos]->ifflagged) {
                    // Unflag the tile
                    board[x_pos][y_pos]->ifflagged = false;
                    flagcount--;
//                    cout << "Tile unflagged: (" << x_pos << ", " << y_pos << ")" << endl;
                } else {
                    flagTile(x_pos, y_pos);
//                    cout << "Tile flagged: (" << x_pos << ", " << y_pos << ")" << endl;
                }
            }
        }
    }
}

void GameWindow::loadBoard(int row, int col) {
    // this function loads the board in a 2D vector
    // loads the 2D vectors of what's been revealed and what's been flagged
    board.resize(col, vector<Tile*>(row));

    for(int i = 0; i < col; i++){
        for(int j = 0; j < row; j++){
            board[i][j] = new Tile(row, col, 0);
        }
    }
    placeMines(mines_num);
    adjacentMines();
}

void GameWindow::placeMines(int mines_num) {
    srand(static_cast<unsigned int>(time(nullptr))); // set the seed using the current time

    int count = 0;
    while(count < mines_num){
        int x = rand() % col;
        int y = rand() % row;

        if(!board[x][y]->ifmine){
            board[x][y]->ifmine = true;
//            cout << "mine was placed at " << x << " , " << y << endl;
            count++;
        }
    }
}

void GameWindow::adjacentMines() {
    for(int i = 0; i < col; i++){
        for(int j = 0; j < row; j++){
            board[i][j]->neighbors.clear();

            int count = 0;
            for(int dx = -1; dx <= 1; dx++){
                for(int dy = -1; dy <= 1; dy++){
                    int x_pos = i + dx;
                    int y_pos = j + dy;

                    if(x_pos >= 0 && x_pos < col && y_pos >= 0 && y_pos < row && !(dx == 0 && dy == 0)){
                        if (board[x_pos][y_pos]->ifmine) {
                            count++;
                        }
                        board[i][j]->neighbors.push_back(board[x_pos][y_pos]);
                    }
                }
            }
            board[i][j]->nearbymines = count;
//            cout << "Number of adjacent mines for tile (" << i << ", " << j << "): " << count << endl;
        }
    }
}

void GameWindow::revealTile(int mousePos_x, int mousePos_y) {
    // Recursively reveals tiles
    if(!board[mousePos_x][mousePos_y]->ifrevealed && !board[mousePos_x][mousePos_y]->ifflagged){
        board[mousePos_x][mousePos_y]->ifrevealed = true;

        if(board[mousePos_x][mousePos_y]->ifmine){
            loseGame();
        }
        // Checks if neighboring tiles have 0 mines and that the neighboring tiles aren't mines
        if (board[mousePos_x][mousePos_y]->nearbymines == 0 && !board[mousePos_x][mousePos_y]->ifmine){
            for(int i = -1; i <= 1; i++){
                for(int j = -1; j <= 1; j++){
                    int x_pos = mousePos_x + i;
                    int y_pos = mousePos_y + j;

                    if(x_pos >= 0 && x_pos < col &&
                       y_pos >= 0 && y_pos < row &&
                       !board[x_pos][y_pos]->ifrevealed
                       && !board[mousePos_x][mousePos_y]->ifmine) {
                        // Recursive calls
                            revealTile(x_pos, y_pos);
                    }
                }
            }
        }
    }
}

void GameWindow::flagTile(int mousePos_x, int mousePos_y) {
    // Flags tiles, ensures that flagcount goes up if tile is flagged and goes down if unflagged
    if (!board[mousePos_x][mousePos_y]->ifrevealed) {
        board[mousePos_x][mousePos_y]->ifflagged = true;
        if (board[mousePos_x][mousePos_y]) {
            flagcount++;
        } else {
            flagcount--;
        }
    }
}

void GameWindow::counter() {
    // Counter flag function used in draw function
    int remainingFlags = mines_num - flagcount;
    totalflags = remainingFlags;
}

void GameWindow::restartGame() {
    // Initializes everything to zero or default
    for(int i = 0; i < col; i++){
        for (int j = 0; j < row; j++){
            delete board[i][j];
        }
    }
    board.clear();
    loadBoard(row, col);

    ifpaused = false;
    debugmines = false;
    allrevealed = false;
    mineclicked = false;
    leaderboardwritten = false;
    flagcount = 0;
    totalflags = 0;
    clock.restartTime();

    face.setTexture(happyface);
    face.setPosition((col / 2 * 32) - 32, 32 * (row + 0.5));

    game.clear();
}

map<int, sf::Sprite> GameWindow::parseDigits(sf::Sprite digits) {
    map<int, sf::Sprite> digitsMap;

    int xPos = 0; // Start at the beginning of the sprite sheet

    // Parse the digits from the sprite sheet
    for (int i = 0; i < 10; i++) {
        sf::IntRect rect(xPos, 0, 21, 32);
        digits.setTextureRect(rect);
        sf::Sprite sprite = digits;
        digitsMap.emplace(i, sprite);

        xPos += 21;
    }

    // Add sprite for negative sign at the end
    sf::IntRect rectNeg(xPos, 0, 21, 32); // Assuming the negative sign sprite is at the end
    digits.setTextureRect(rectNeg);
    sf::Sprite negSprite = digits;
    digitsMap.emplace(-1, negSprite);

    return digitsMap;
}

void GameWindow::drawTimer(int elapsed) {
    int minutes = elapsed / 60;
    int seconds = elapsed % 60;

    int minutes0 = minutes / 10 % 10;
    int minutes1 = minutes % 10;
    int seconds0 = seconds / 10 % 10;
    int seconds1 = seconds % 10;

    // Set positions for each digit
    int xPos = (col * 32) - 97;
    digitsMap[minutes0].setPosition(xPos, 32 * (row + 0.5) + 16);
    game.draw(digitsMap[minutes0]);

    xPos += 21;
    digitsMap[minutes1].setPosition(xPos, 32 * (row + 0.5) + 16);
    game.draw(digitsMap[minutes1]);

    int xPos2 = (col * 32) - 54;
    digitsMap[seconds0].setPosition(xPos2, 32 * (row + 0.5) + 16);
    game.draw(digitsMap[seconds0]);

    xPos2 += 21;
    digitsMap[seconds1].setPosition(xPos2, 32 * (row + 0.5) + 16);
    game.draw(digitsMap[seconds1]);
}

bool GameWindow::loseGame() {
    mineclicked = true;
}

bool GameWindow::winGame(){
    // Checks if game is won, initializes flagcount and total flags to zero;
    allrevealed = true;
    for(int i = 0; i < col; i++){
        for(int j = 0; j < row; j++){
            if(!board[i][j]->ifrevealed && !board[i][j]->ifmine){
                allrevealed = false;
                return false;
            }
        }
    }
    if(allrevealed){
        flagcount = 0;
        totalflags = 0;
        return true;
    }
}

void GameWindow::draw() {
    // cout << clock.getTime() << endl;
    sf::Sprite digits;

    digits.setTexture(digitstex);
    digitsMap = parseDigits(digits);

    game.clear(sf::Color::White);
    sf::Sprite tile;
    // Drawing the face buttons

    face.setTexture(happyface); // default to happy face
    if (mineclicked) {
        //cout << "you lost :(" << endl;
        face.setTexture(loseface); // change to lose face if mine is clicked
        clock.stopTime();
    } else if (winGame() ) {

        face.setTexture(winface);
        game.draw(face);
        if (!leaderboardwritten){
            leaderboardchanges.writeLeaderboard(clock.getTime(), welcome.getUsernameInput());
            leaderboardwritten = true;
        }

        for (int i = 0; i < col; i++) {
            for (int j = 0; j < row; j++) {
                if (board[i][j]->ifmine && !board[i][j]->ifflagged) {
                    flagTile(i, j);
                }

                tile.setTexture(minetex);
                tile.setPosition(i * 32, j * 32);
                game.draw(tile);

                tile.setTexture(flagtex);
                game.draw(tile);
            }
        }
        clock.stopTime();
    }
    face.setPosition((col / 2 * 32) - 32, 32 * (row + 0.5));
    game.draw(face);

    counter();
    // Calculate digits
    int hundredsDigit = abs(totalflags) / 100; // Use absolute value for calculation
    int tensDigit = (abs(totalflags) / 10) % 10;
    int onesDigit = abs(totalflags) % 10;

    // Draw the negative sign if flagcount is negative
    if (totalflags < 0) {
        digitsMap[-1].setPosition(12, 32 * (row + 0.5) + 16);
        game.draw(digitsMap[-1]);
        // Set the hundreds digit to zero if it's negative
        hundredsDigit = 0;
    } else {
        // Draw the hundreds digit
        digitsMap[hundredsDigit].setPosition(12, 32 * (row + 0.5) + 16);
        game.draw(digitsMap[hundredsDigit]);
    }
    // Draw the tens digit
    digitsMap[tensDigit].setPosition(33, 32 * (row + 0.5) + 16);
    game.draw(digitsMap[tensDigit]);
    // Draw the ones digit
    digitsMap[onesDigit].setPosition(54, 32 * (row + 0.5) + 16);
    game.draw(digitsMap[onesDigit]);

    //Drawing the tiles, game implementation
    for (int i = 0; i < col; i++) {
        for (int j = 0; j < row; j++) {
            if(!board[i][j]->ifrevealed && !board[i][j]->ifflagged){
                tile.setTexture(hiddentex);
                tile.setPosition(i * 32, j * 32);
                game.draw(tile);
            } else if (board[i][j]->ifrevealed) {
                // draw revealed tile
                tile.setTexture(revealedtex);
                tile.setPosition(i * 32, j * 32);
                game.draw(tile);

                // ff the tile contains a mine, draw the mine sprite
                if (board[i][j]->ifmine) {
                    tile.setTexture(minetex);
                    game.draw(tile);
                } else if (!ifpaused && board[i][j]->nearbymines > 0) {
                    // draw number texture
                    sf::Texture* numTexture = nullptr;
                    int numMines = board[i][j]->nearbymines;

                    if (numMines == 1) numTexture = &num1;
                    else if (numMines == 2) numTexture = &num2;
                    else if (numMines == 3) numTexture = &num3;
                    else if (numMines == 4) numTexture = &num4;
                    else if (numMines == 5) numTexture = &num5;
                    else if (numMines == 6) numTexture = &num6;
                    else if (numMines == 7) numTexture = &num7;
                    else if (numMines == 8) numTexture = &num8;

                    if (numTexture) {
                        tile.setTexture(*numTexture);
                        tile.setPosition(i * 32, j * 32);
                        game.draw(tile);
                    }
                }
            } else if (board[i][j]->ifflagged){
                // draw flagged tile

                tile.setTexture(hiddentex);
                tile.setPosition(i * 32, j * 32);
                game.draw(tile);

                tile.setTexture(flagtex);
                game.draw(tile);
            }
        }
    }

    leader_board.setTexture(leaderboard);
    leader_board.setPosition((col * 32) - 176, 32 * (row + 0.5));
    game.draw(leader_board);

    // Set's textures to revealed when paused
    pause.setTexture(pause_button);
    if(ifpaused){
        pause.setTexture(play_button);
        pause.setPosition((col * 32) - 240, 32 * (row + 0.5));
        game.draw(pause);
        for (int i = 0; i < col; i++) {
            for (int j = 0; j < row; j++) {
                if (!board[i][j]->ifrevealed) {
                    tile.setTexture(revealedtex);
                    tile.setPosition(i * 32, j * 32);
                    game.draw(tile);
                }
            }
        }
    }else if(!ifpaused && !winGame() && !mineclicked){
        pause.setPosition((col * 32) - 240, 32 * (row + 0.5));
        game.draw(pause);
    }else{
        pause.setPosition((col * 32) - 240, 32 * (row + 0.5));
        game.draw(pause);
    }

    drawTimer(clock.getTime());

    // Debug (shows all mines present when debug toggled)
    debug.setTexture(debugtex);
    debug.setPosition((col * 32) - 304, 32 * (row + 0.5));
    game.draw(debug);

    if(!ifpaused && debugmines){
        for (int i = 0; i < col; i++) {
            for (int j = 0; j < row; j++) {
                if (board[i][j]->ifmine) {
                    // Draw mine sprite
                    tile.setTexture(minetex);
                    tile.setPosition(i * 32, j * 32);
                    game.draw(tile);
                }
            }
        }
    }
    game.display();
}

// Tile object
GameWindow::Tile::Tile(int row, int col, int nearbymines) {
    this->row = row;
    this->col = col;
    this->nearbymines = nearbymines;
    ifmine = false;
    ifflagged = false;
    bool ifrevealed = false;
    bool hiddentile = true;
}

Timer::Timer(){
    starttime = std::chrono::high_resolution_clock::now();
    pause = false;
}
int Timer::getTime(){
    if (!pause){
        auto endtime = std::chrono::high_resolution_clock::now();
        elapsedtime += chrono::duration_cast<chrono::milliseconds>(endtime-starttime).count(); // might have to doublecheck here
        starttime = std::chrono::high_resolution_clock ::now();
        return elapsedtime / 1000;
    }
    return elapsedtime / 1000;
}
void Timer::stopTime(){
    pause = !pause;
    starttime = std::chrono::high_resolution_clock ::now();
}

void Timer::restartTime() {
    starttime = std::chrono::high_resolution_clock::now();
    elapsedtime = 0;
}
