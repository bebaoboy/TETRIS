#pragma once
#include <chrono>
#include <random>

#include "Tetromino.hpp"
#include <SFML/Audio.hpp>

class Tetris
{
	//Used to check whether the game is over or not
	bool game_over = 0;
	bool is_started = 0;
	bool is_viewed = 0;
	//Is the hard drop button pressed?
	bool hardDrop_pressed = 0;
	//Is the rotate button pressed?
	bool rotate_pressed = 0;
	int is_rotated = 0;

	bool is_gravity = 0;
	bool is_tetris = 0;

	//Used to make the game framerate-independent
	unsigned int lag = 0;
	//How many lines the player cleared?
	unsigned lines_cleared = 0;

	//Timer for the line clearing effect
	unsigned int clear_effect_timer = 0;
	int max_speed_count = 0;

	unsigned int current_fall_speed = START_FALL_SPEED;
	//Timer for the tetromino falling
	unsigned int fall_timer = 0;
	//Timer for the tetromino moving horizontally
	unsigned int move_timer = 0;
	//Next shape (The shape that comes after the current shape)
	unsigned int next_shape;
	//Timer for the tetromino's soft drop
	unsigned int soft_drop_timer = 0;
	
	// Scoring
	unsigned long scores = 0;
	unsigned long peak_score = SCORES_TO_INCREASE_SPEED;
	std::vector<std::pair< std::string, unsigned long>> score_list;

	//Similar to lag, used to make the game framerate-independent
	std::chrono::time_point<std::chrono::steady_clock> previous_time;

	std::random_device random_device;

	std::default_random_engine random_engine{ random_device() };
	std::uniform_int_distribution<unsigned int> shape_distribution{ 0, 6 };

	//Stores the current state of each row. Whether they need to be cleared or not
	std::vector<bool> clear_lines = std::vector<bool>(ROWS, 0);

	//We're gonna use this object to draw every cell in the game
	sf::RectangleShape cell{ sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1) };

	//All the colors for the cells
	std::vector<sf::Color> cell_colors = {
		sf::Color(36, 36, 85),
		sf::Color(0, 219, 255),
		sf::Color(0, 36, 255),
		sf::Color(255, 146, 0),
		sf::Color(255, 219, 0),
		sf::Color(0, 219, 0),
		sf::Color(146, 0, 255),
		sf::Color(219, 0, 0),
		sf::Color(73, 73, 85)
	};

	//Game matrix. Everything will happen to this matrix
	MainMatrix matrix = MainMatrix(COLUMNS, std::vector<int>(ROWS));

	sf::Event event{};
	sf::Sound player;

	sf::SoundBuffer opening_sound;
	sf::SoundBuffer hardDrop_sound, normal_drop_sound;
	sf::SoundBuffer lost_sound;
	sf::SoundBuffer line_clear_sound;
	sf::SoundBuffer rotate_sound;

	sf::Music music_player;
	std::vector<std::string> music_files;
	std::uniform_int_distribution<unsigned int> music_distribution{ 0, 8 };

	sf::RenderWindow window{
		sf::VideoMode(2 * static_cast<unsigned int>(CELL_SIZE) * COLUMNS * SCREEN_RESIZE,
			static_cast<unsigned int>(CELL_SIZE) * ROWS * SCREEN_RESIZE),
		"Tetris", sf::Style::Close };

	unsigned char shape = static_cast<unsigned int>(shape_distribution(random_engine));
	//Falling tetromino. At the start we're gonna give it a random shape
	Tetromino tetromino{ shape, getTetromino(shape, COLUMNS / 2, 1) };
	
public:
	Tetris();
	void start();

private:
	void setLagTime();
	void checkEvent();

	void clearLines();
	void checkLost();
	void restart(float& duration);
	std::string timer(float& duration, sf::Clock& clock);
	std::vector<Position> getTetromino(unsigned char shape, unsigned char i_x, unsigned char i_y);
	std::string getTime();
	void gravityFalls();
	void setGameSpeed(unsigned int i);
	void tetrisMode();
	void maxSpeedReached();

	void drawText(float i_x, float i_y, const std::string& text, sf::RenderWindow& window, float scale = 1.f);
	void drawBoard(std::string timerString);
	void drawBorder();
	void drawGhost();
	void drawNext();
	void drawMatrix(bool is_gravity = false);
	void drawEffect();
	void drawPause();
	void drawHelp();
	void drawLeaderboard();
	void dimBackground();

	void delay(int num = 30000);
};

