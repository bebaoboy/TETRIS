#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

//The size of the cell
constexpr float CELL_SIZE = 8.0;
constexpr unsigned int COLUMNS = 10;
constexpr unsigned int ROWS = 20;
//Screen resize
constexpr unsigned int SCREEN_RESIZE = 4;

//Lines needed to increase the game speed
constexpr unsigned int LINES_TO_INCREASE_SPEED = 3;
constexpr unsigned int SCORES_TO_INCREASE_SPEED = 5000;
constexpr unsigned int T_LINES_TO_INCREASE_SPEED = 2;
constexpr unsigned int T_SCORES_TO_INCREASE_SPEED = 3000;

constexpr unsigned int MOVE_SPEED = 4;
constexpr unsigned int SOFT_DROP_SPEED = 4;
//Fall speed at the start of the game. Lower the value, faster the falling
constexpr unsigned int START_FALL_SPEED = 60;
constexpr unsigned int MIN_SPEED = 4;

//The duration of the line clearing effect
constexpr unsigned int CLEAR_EFFECT_DURATION = 38;
//Delay when rotate
constexpr unsigned int ROTATE_LAG = 500;
//The duration of every frame
constexpr unsigned int FRAME_DURATION = 16667;

// Scoring system
constexpr unsigned int NORMAL_DROP_SCORE = 15;
constexpr unsigned int SOFT_DROP_SCORE = 50;
constexpr unsigned int HARD_DROP_SCORE = 100;
constexpr unsigned int CLEAR_LINE_SCORE = 1000;
constexpr unsigned int TETRIS_BONUS_SCORE = 4000;
constexpr unsigned int LINE_BONUS = 5;
constexpr unsigned int LINE_BONUS_SCORE = 500;

using Position = sf::Vector2<int>;
using MainMatrix = std::vector<std::vector<int>>;