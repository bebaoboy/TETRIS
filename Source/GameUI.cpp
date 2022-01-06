#include "Headers/Game.hpp"

void Tetris::drawText(float i_x, float i_y, const std::string& i_text, sf::RenderWindow& i_window, float scale)
{
	//We're gonna align the text ot the left top
	float character_x = i_x;
	float character_y = i_y;

	unsigned char character_width;

	sf::Sprite character_sprite;

	sf::Texture font_texture;
	font_texture.loadFromFile("Source/Resources/Images/Font.png");

	//We're gonna calculate the width of the character based on the font image size
	//96 because there are 96 character in the image
	character_width = font_texture.getSize().x / 96;

	character_sprite.setTexture(font_texture);

	for (const char a : i_text)
	{
		if ('\n' == a)
		{
			//After every newline we put increase the y-coordinate and reset the x-coordinate
			character_x = i_x;
			character_y += font_texture.getSize().y * scale;

			continue;
		}

		//Change the position of the next character
		character_sprite.setPosition(character_x, character_y);
		//Pick the character from the font image
		character_sprite.setTextureRect(sf::IntRect(character_width * (a - 32), 0, 
			character_width, font_texture.getSize().y));
		character_sprite.setScale(scale, scale);
		//Increase the x-coordinate
		character_x += character_width * scale;

		//Draw the character
		i_window.draw(character_sprite);
	}
}

void Tetris::drawBorder() {
	float top_x = CELL_SIZE * (1.5f * COLUMNS - 2.5f), 
		  top_y = CELL_SIZE * (0.25f * ROWS - 2.5f);

	sf::RectangleShape preview_border(sf::Vector2f(5 * CELL_SIZE, 5 * CELL_SIZE));
	preview_border.setFillColor(sf::Color(0, 0, 0));
	preview_border.setOutlineThickness(-1);
	preview_border.setPosition(top_x, top_y);
	window.draw(preview_border);

	if (is_started == 1 && game_over == 0)
		drawText(top_x + 6, top_y - 7, "Up next:", window, .4f);

	if (is_gravity)
		drawText(top_x + 2, top_y + 5 * CELL_SIZE - 6, "(G)ravity: on", window, .3f);
	else 
		drawText(top_x + 2, top_y + 5 * CELL_SIZE - 6, "(G)ravity: off", window, .3f);
}

void Tetris::drawBoard(std::string timerString) {
	//Here we're drawing everything!
	if (FRAME_DURATION > lag)
	{

		//Clear the window from the previous frame
		window.clear();
		
		//Draw the preview border
		drawBorder();

		//Draw the matrix
		drawMatrix();

		// Draw enter to play
		if (is_started == 0) {
			sf::RectangleShape box(sf::Vector2f(75, 11));
			box.setFillColor(sf::Color(0, 0, 0, 70));
			box.setPosition(2, 67);
			box.setOutlineThickness(-0.5);
			window.draw(box);
			drawText(10, 70, "Press Enter to play.", window, .4f);
		}

		// Draw ghost tetromino
		if (game_over == 0 && is_started == 1) {
			drawGhost();

			//Drawing the falling tetromino
			for (Position& mino : tetromino.getMino())
			{
				cell.setPosition(static_cast<float>(CELL_SIZE * mino.x),
					static_cast<float>(CELL_SIZE * mino.y));
				window.draw(cell);
			}
		}

		//Drawing the effect
		drawEffect();

		//Draw the next tetromino
		if (game_over == 0 && 1 == is_started) {
			drawNext();
		}

		//Drawing the text
		if (is_started == 1 || game_over == 1) {
			std::string t;
			if (is_tetris)
				t = "on";
			else
				t = "off";

			drawText(static_cast<unsigned int>(CELL_SIZE * (1 + COLUMNS) + 10),
				static_cast<unsigned int>(0.45f * CELL_SIZE * ROWS),
				"Lines:" + std::to_string(lines_cleared) +
				"\nSpeed:" + std::to_string(
					START_FALL_SPEED / static_cast<float>(current_fall_speed)) + 'x' +
				"\nTime elapsed: " + timerString +
				"\nScores: " + std::to_string(scores) +
				"\nTetris mode: " + t, window, .3f);
			if (START_FALL_SPEED / static_cast<float>(current_fall_speed) >= 6.f) {
				if (is_tetris)
					drawText(static_cast<unsigned int>(CELL_SIZE * (1 + COLUMNS) + 8),
						static_cast<unsigned int>(0.45f * CELL_SIZE * ROWS + 44),
						"TETRIS MODE INCOMING...", window, .35f);
				else
					drawText(static_cast<unsigned int>(CELL_SIZE * (1 + COLUMNS) + 8),
						static_cast<unsigned int>(0.45f * CELL_SIZE * ROWS + 44),
						"MAX SPEED APPROACHING...", window, .35f);
			}
				
		}		

		if (game_over == 1) {
			drawText(static_cast<unsigned int>(CELL_SIZE * (1 + COLUMNS) + 8),
				static_cast<unsigned int>(0.5f * CELL_SIZE * ROWS) + 27,
				"You've lost", window, .65f);
			drawText(static_cast<unsigned int>(CELL_SIZE * (1 + COLUMNS) + 8),
				static_cast<unsigned int>(0.5f * CELL_SIZE * ROWS) + 48,
				"Press R to replay.", window, .3f);
		}

		if (is_started == 0 || game_over == 1) {
			drawText(static_cast<unsigned int>(CELL_SIZE * (1 + COLUMNS) + 8),
				static_cast<unsigned int>(0.5f * CELL_SIZE * ROWS) + 54,
				"Press L to view\nThe leaderboard.", window, .3f);
		}

		if (is_started == 1 && game_over == 0) {
			drawText(static_cast<unsigned int>(CELL_SIZE * (1 + COLUMNS) + 8),
				static_cast<unsigned int>(0.5f * CELL_SIZE * ROWS) + 59,
				"Press P to pause", window, .3f);
		}

		drawText(static_cast<unsigned int>(CELL_SIZE * (1 + COLUMNS) + 8),
			static_cast<unsigned int>(0.5f * CELL_SIZE * ROWS) + 64,
			"Press I to seek help.", window, .3f);
		drawText(static_cast<unsigned int>(CELL_SIZE * (1 + COLUMNS) + 8),
			static_cast<unsigned int>(0.5f * CELL_SIZE * ROWS) + 69,
			"Press Esc to exit.", window, .3f);

		window.display();
	}
}

void Tetris::drawGhost() {
	//If the game is not over, draw ghost tetromino
	if (game_over == 0)
	{
		//Set the cell color to gray for ghost tetromino
		cell.setFillColor(cell_colors[8]);

		//Drawing the ghost tetromino
		for (Position& mino : tetromino.getGhostMino(matrix))
		{
			cell.setPosition(static_cast<float>(CELL_SIZE * mino.x), static_cast<float>(CELL_SIZE * mino.y));
			window.draw(cell);
		}

		cell.setFillColor(cell_colors[static_cast<size_t>(1) + tetromino.getShapeCode()]);
	}

}

void Tetris::drawNext() {
	//Draw the next tetromino
	cell.setFillColor(cell_colors[1 + static_cast<size_t>(next_shape)]);
	if (game_over != 0)
		cell.setFillColor(cell_colors[8]);
	cell.setSize(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));
	for (Position& mino : getTetromino(next_shape,
		static_cast<unsigned int>(1.5f * COLUMNS), static_cast<unsigned int>(0.25f * ROWS)))
	{
		//Shifting the tetromino to the center of the preview border
		float next_tetromino_x = CELL_SIZE * mino.x;
		float next_tetromino_y = CELL_SIZE * mino.y;

		if (next_shape == 0) // I shape
			next_tetromino_y += static_cast<unsigned int>(round(0.5f * CELL_SIZE));
		else if (next_shape != 3) // O shape
			next_tetromino_x -= static_cast<unsigned int>(round(0.5f * CELL_SIZE));

		cell.setPosition(next_tetromino_x, next_tetromino_y);

		window.draw(cell);
	}
}

void Tetris::drawMatrix(bool is_gravity) {
	//Draw the matrix
	for (unsigned int a = 0; a < COLUMNS; a++)
	{
		for (unsigned int b = 0; b < ROWS; b++)
		{
			if (clear_lines[b] == 0 || is_gravity == 1)
			{
				cell.setPosition(static_cast<float>(CELL_SIZE * a),
					static_cast<float>(CELL_SIZE * b));

				if (game_over == 1 && matrix[a][b] > 0)
					cell.setFillColor(cell_colors[8]);
				else
					cell.setFillColor(cell_colors[matrix[a][b]]);

				window.draw(cell);
			}
		}
	}
}

void Tetris::drawEffect() {
	//Calculating the size of the effect squares
	float clear_cell_size =
		(2 * round(0.5f * CELL_SIZE * (clear_effect_timer / static_cast<float>(CLEAR_EFFECT_DURATION))));
	for (unsigned int a = 0; a < COLUMNS; a++)
	{
		for (unsigned int b = 0; b < ROWS; b++)
		{
			if (1 == clear_lines[b])
			{
				cell.setFillColor(cell_colors[0]);
				cell.setPosition(static_cast<float>(CELL_SIZE * a), static_cast<float>(CELL_SIZE * b));
				cell.setSize(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));

				window.draw(cell);

				cell.setFillColor(sf::Color(255, 255, 255));
				cell.setPosition(floor(CELL_SIZE * (0.5f + a) - 0.5f * clear_cell_size),
					floor(CELL_SIZE * (0.5f + b) - 0.5f * clear_cell_size));
				cell.setSize(sf::Vector2f(clear_cell_size, clear_cell_size));

				window.draw(cell);

				player.setBuffer(line_clear_sound);
				player.play();

				tetromino.hardDrop(matrix);
			}
		}
	}
}

void Tetris::drawPause() {
	// P to pause
	// Enter to exit pause
	dimBackground();

	float top_x = 40, top_y = 67;
	float size_x = 75, size_y = 11;

	sf::RectangleShape box(sf::Vector2f(size_x, size_y));
	box.setFillColor(sf::Color(0, 0, 0, 150));
	box.setPosition(top_x, top_y);
	box.setOutlineThickness(-0.5);
	window.draw(box);
	drawText(top_x + 28, top_y + 3, "PAUSE", window, .4f);
	drawText(top_x + 1, size_y + top_y + 5, 
		"Press G to disable Gravity\
		\nPress T to disable Tetris mode\
		\n\nPress O to continue", window, .35f);
	window.display();
}

void Tetris::drawHelp() {
	// I to help
	// O to exit
	dimBackground();

	float top_x = 33, top_y = 30;
	float size_x = 100, size_y = 90;

	sf::RectangleShape help_border(sf::Vector2f(size_x, size_y));
	help_border.setFillColor(sf::Color(0, 0, 0));
	help_border.setOutlineThickness(-1);
	help_border.setPosition(top_x, top_y);
	drawText(top_x - 10, top_y - 7, "HELP", window, .4f);
	window.draw(help_border);

	drawText(top_x + 5, top_y + 10, "C for rotate clockwise\
		\nZ for counter-clockwise\
		\n\nArrow left, right\
		\nArrow down for soft-dropping\
		\nSpace for hard-dropping\
		\n\nG for Gravity Mode\
		\nT for Tetris Mode\
		\nF for Prefill\
		\nP for setting toggle\
		\n\nFollow the prompts for more.", window, .35f);
	
	drawText(top_x + 3, size_y + 35, 
		"Press G to disable Gravity\
		\nPress T to disable Tetris mode\
		\n\nPress O to continue", window, .35f);
	window.display();
}

void Tetris::drawLeaderboard() {
	dimBackground();

	float top_x = 33, top_y = 30;
	float size_x = 100, size_y = 90;

	sf::RectangleShape help_border(sf::Vector2f(size_x, size_y));
	help_border.setFillColor(sf::Color(0, 0, 0));
	help_border.setOutlineThickness(-1);
	help_border.setPosition(top_x, top_y);
	drawText(top_x - 10, top_y - 7, "HIGH SCORE", window, .4f);
	window.draw(help_border);

	std::sort(score_list.begin(), score_list.end(), [&](const auto& a, const auto& b) {
		return a.second > b.second; 
		}
	);

	drawText(top_x + 5, top_y + 6, "         Time             Score\n", window, .33f);
	std::string s = "";
	for (auto& [time, score] : score_list) {
		s += time + "       " + std::to_string(score) + "\n";
	}

	drawText(top_x + 5, top_y + 14, s, window, .33f);

	drawText(size_x - 5, size_y + 35, "Press O to continue", window, .35f);
	window.display();
}

void Tetris::dimBackground() {
	sf::RectangleShape background(sf::Vector2f(
		2 * static_cast<unsigned int>(CELL_SIZE) * COLUMNS * SCREEN_RESIZE,
		static_cast<unsigned int>(CELL_SIZE) * ROWS * SCREEN_RESIZE));
	background.setFillColor(sf::Color(0, 0, 0, 195));
	window.draw(background);
}

