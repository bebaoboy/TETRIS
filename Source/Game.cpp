#include "Headers/Game.hpp"

Tetris::Tetris() {
	//Resizing the window
	window.setView(sf::View(sf::FloatRect(0, 0, 2 * CELL_SIZE * COLUMNS, CELL_SIZE * ROWS)));

	//Generate a random shape and store it as the next shape
	next_shape = static_cast<unsigned int>(shape_distribution(random_engine));

	//Get the current time and store it in the variable
	previous_time = std::chrono::steady_clock::now();

	rotate_sound.loadFromFile("Source/Resources/Sounds/rotate.wav");
	normal_drop_sound.loadFromFile("Source/Resources/Sounds/selection.wav");
	hardDrop_sound.loadFromFile("Source/Resources/Sounds/fall2.wav");
	line_clear_sound.loadFromFile("Source/Resources/Sounds/line.wav");
	lost_sound.loadFromFile("Source/Resources/Sounds/gameover.wav");
	opening_sound.loadFromFile("Source/Resources/Sounds/success.wav");
}

void Tetris::start() {
	//While the window is open
	sf::Clock clock;
	float duration = 0.f;


	player.setBuffer(opening_sound);
	player.play();

	while (window.isOpen() == 1)
	{
		setLagTime();

		//While the lag exceeds the maximum allowed frame duration
		while (FRAME_DURATION <= lag)
		{
			lag -= FRAME_DURATION;
			checkEvent();

			if (is_viewed == 0 && clear_effect_timer == 0)
			{
				//If the game is not over
				if (game_over == 0 && is_started == 1)
				{
					if (rotate_pressed == 0)
					{
						if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
						{
							rotate_pressed = 1;
							tetromino.rotate(1, matrix);
							player.setBuffer(rotate_sound);
						    player.play();
						} 
						else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
						{
							rotate_pressed = 1;
							tetromino.rotate(0, matrix);
							player.setBuffer(rotate_sound);
							player.play();
						}						
					}

					if (move_timer == 0)
					{
						if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
						{
							//Reset the move timer
							move_timer = 1;
							tetromino.moveLeft(matrix);
						}
						else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
						{
							move_timer = 1;
							tetromino.moveRight(matrix);
						}
					}
					else
					{
						//Update the move timer
						move_timer = (1 + move_timer) % MOVE_SPEED;
					}

					//If hard drop is not pressed
					if (hardDrop_pressed == 0)
					{
						if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
						{
							hardDrop_pressed = 1;

							//Reset the fall timer
							fall_timer = current_fall_speed;

							//Score by distance to the bottom
							int max_y = 0;
							for (auto& mino : tetromino.getMino()) {
								if (max_y < mino.y) max_y = mino.y;
							}
							scores += (ROWS - max_y) * HARD_DROP_SCORE;

							player.setBuffer(hardDrop_sound);
							player.play();

							tetromino.hardDrop(matrix);
						}
					}

					if (soft_drop_timer == 0)
					{
						if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
						{
							if (tetromino.moveDown(matrix))
							{
								fall_timer = 0;
								soft_drop_timer = 1;
							}
						}
					}
					else
					{
						soft_drop_timer = (1 + soft_drop_timer) % SOFT_DROP_SPEED;
						scores += SOFT_DROP_SCORE;
					}

					//If the fall timer is over
					if (current_fall_speed == fall_timer)
					{
						//If the tetromino can't move down anymore
						if (tetromino.moveDown(matrix) == 0)
						{
							//Put the falling tetromino to the matrix
							//play sound and glowing
							tetromino.updateToMatrix(matrix);
							
							//check any lines to clear
							clearLines();

							//If the player reached a certain number of score
							if (scores % peak_score >= 0)
							{
								//We increase the game speed
								setGameSpeed(2);
								peak_score *= 2;
							}

							//If the effect timer is over
							if (clear_effect_timer == 0) 
								checkLost();
						}
						
						scores += NORMAL_DROP_SCORE * 
							static_cast<unsigned int>((START_FALL_SPEED / static_cast<float>(current_fall_speed)));

						//Restart the fall timer
						fall_timer = 0;
					}
					else
					{
						//Increment the fall timer
						if (is_rotated != 0)
							is_rotated++;
						if (rotate_pressed == 0 || (rotate_pressed && is_rotated == ROTATE_LAG))
							fall_timer++;
					}
				}

				//This is the code for restarting the game
				else 
				{

					// replay
					if (is_viewed == 0 && is_started == 0 && 
						sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
						is_started = 1;
					}
					
					// view highscore when not playing
					else if ((is_started == 0 || game_over == 1) 
						&& sf::Keyboard::isKeyPressed(sf::Keyboard::L)) {						
						is_viewed = 1;
						drawLeaderboard();
					}
				}
			}
			else if (is_viewed == 0)
			{
				//Decrement the effect timer
				clear_effect_timer--;

				if (clear_effect_timer == 0)
				{
					//Loop through each row
					for (unsigned int a = 0; a < ROWS; a++)
					{
						//If the row should be cleared
						if (clear_lines[a])
						{
							//Loop through each cell in the row
							for (unsigned int b = 0; b < COLUMNS; b++)
							{
								//Set the cell to 0 (empty) (the absence of existence)
								matrix[b][a] = 0;

								//Swap the row with the rows above
								for (size_t c = a; c > 0; c--)
								{
									matrix[b][c] = matrix[b][c - 1];
									matrix[b][c - 1] = 0;
								}
							}							
						}
					}

					// Gravity mode
					if (is_gravity == 1) {
						gravityFalls();
					}

					checkLost();

					//Clear the clear lines array
					std::fill(clear_lines.begin(), clear_lines.end(), 0);
				}
			}

			// Replay anytime
			if (is_viewed == 0 && sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
				restart(duration);
				is_started = 0;
				is_gravity = 0;

				player.setBuffer(opening_sound);
				player.play();
			}

			// Activate gravity mode
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::G)) {
				is_gravity = 1;
			}

			if (is_viewed == 1 && is_gravity == 1
				&& sf::Keyboard::isKeyPressed(sf::Keyboard::G)) {
				is_gravity = 0;
			}

			// ESCAPE
			if (is_viewed == 0 && sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
				// confirm
				return;
			}

			// PAUSE game
			if (is_viewed == 0 && sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
				is_viewed = 1;
				drawPause();
			}
			
			// show instruction when playing
			if (is_viewed == 0 && sf::Keyboard::isKeyPressed(sf::Keyboard::I)) {
				is_viewed = 1;
				drawHelp();
			}

			// RESUME game
			if (is_viewed == 1 && sf::Keyboard::isKeyPressed(sf::Keyboard::O)) {
				is_viewed = 0;
				clock.restart();
			}

			if (is_viewed == 0)
				drawBoard(timer(duration, clock));
		}
	}

}

void Tetris::setLagTime() {
	//Get the difference in time between the current frame and the previous frame
	auto delta_time = static_cast<unsigned int>
		(std::chrono::duration_cast<std::chrono::microseconds>
			(std::chrono::steady_clock::now() - previous_time).count());

		//Add the difference to the lag
	lag += delta_time;

	//In other words, we're updating the current time for the next frame.
	previous_time += std::chrono::microseconds(delta_time);
}

void Tetris::checkEvent() {
	//Looping through the events
	while (window.pollEvent(event))
	{
		//Check the event type
		switch (event.type)
		{
			case sf::Event::Closed:
			{
				//Close the window
				window.close();

				break;
			}
			//If the key has been released
			case sf::Event::KeyReleased:
			{
				//Check which key was released
				switch (event.key.code) {
					case sf::Keyboard::C:
					case sf::Keyboard::Z:
					{
						//Rotation key is not pressed anymore
						rotate_pressed = 0;
						is_rotated = 1;

						break;
					}
					case sf::Keyboard::Down:
					{
						//Reset the soft drop timer
						soft_drop_timer = 0;

						break;
					}
					case sf::Keyboard::Left:
					case sf::Keyboard::Right:
					{
						//Reset the move timer
						move_timer = 0;

						break;
					}
					case sf::Keyboard::Space:
					{
						//Hard drop key is not pressed anymore
						hardDrop_pressed = 0;
					}
				}
			}
		}
	}
}

void Tetris::clearLines() {
	int tetris_count = 0;
	//Loop through every row
	for (unsigned int a = 0; a < ROWS; a++)
	{
		//Here we're gonna check if the current row should be cleared or not
		bool clear_line = 1;

		//Check if the every cell in the row is filled or not
		for (unsigned int b = 0; b < COLUMNS; b++)
		{
			if (matrix[b][a] == 0)
			{
				clear_line = 0;
				break;
			}
		}

		//If we have to clear it
		if (clear_line == 1)
		{
			//WE CLEAR IT!
			//First we increase the score
			lines_cleared++;
			scores += CLEAR_LINE_SCORE;
			tetris_count++;

			//Then we start the effect timer
			clear_effect_timer = CLEAR_EFFECT_DURATION;

			//Set the current row as the row that should be cleared
			clear_lines[a] = 1;

			//If the player reached a certain number of lines
			if (lines_cleared % LINES_TO_INCREASE_SPEED == 0)
			{
				//We increase the game speed
				setGameSpeed(5);
			}
		}
	}
	if (tetris_count >= 4) {
		scores += TETRIS_BONUS_SCORE;
		if (tetris_count > 4)
			scores += (tetris_count - 4) * CLEAR_LINE_SCORE;
	}	
}

void Tetris::checkLost() {
	//Decide if the game is over or not based on the return value of the reset function
	game_over = tetromino.reset(next_shape, matrix, get_tetromino(next_shape, COLUMNS / 2, 1)) == 0;

	//Generate the next shape
	next_shape = shape_distribution(random_engine);

	if (game_over == 1) {

		player.setBuffer(lost_sound);
		player.play();

		tetromino.hardDrop(matrix);
		scores += std::max<int>((lines_cleared - LINE_BONUS) * LINE_BONUS_SCORE, 0);

		score_list.emplace_back(getTime(),scores);
	}
}

void Tetris::restart(float& duration) {
	//We set everything to 0
	game_over = 0;
	hardDrop_pressed = 0;
	rotate_pressed = 0;

	lines_cleared = 0;
	scores = 0;

	current_fall_speed = START_FALL_SPEED;
	fall_timer = 0;
	move_timer = 0;
	soft_drop_timer = 0;
	duration = 0;

	//Then we clear the matrix
	for (auto& a : matrix)
		std::fill(a.begin(), a.end(), 0);
}

std::string Tetris::timer(float& duration, sf::Clock& clock) {
	sf::Time time = clock.restart();
	float fMilliseconds, fSeconds;
	int intMilliseconds, intSeconds;
	int minutes = 0, hours = 0;
	sf::String stringMilliseconds;
	sf::String stringSeconds, stringMinutes, stringHours;
	sf::String timerString;

	if (is_started == 1 && game_over == 0)
		duration += time.asSeconds();
	fMilliseconds = std::modf(duration, &fSeconds);

	intSeconds = static_cast<int>(fSeconds);
	intMilliseconds = static_cast<int>(fMilliseconds * 1000);

	minutes = intSeconds / 60;
	intSeconds %= 60;
	hours = minutes / 60;
	minutes %= 60;

	stringMilliseconds = std::to_string(intMilliseconds);
	stringSeconds = std::to_string(intSeconds);
	stringMinutes = std::to_string(minutes);
	stringHours = std::to_string(hours);

	if (intMilliseconds <= 0) {
		stringMilliseconds = "000";
	}

	if (intSeconds < 10) {
		stringSeconds = "0" + stringSeconds;
	}

	if (minutes < 10) {
		stringMinutes = "0" + stringMinutes;
	}

	if (hours < 10) {
		stringHours = "0" + stringHours;
	}

	timerString = stringHours + ":" + stringMinutes + ":" + 
		stringSeconds/* + ":" + stringMilliseconds*/;
	return timerString;
}

std::vector<Position> Tetris::get_tetromino(unsigned char i_shape, unsigned char i_x, unsigned char i_y) {
	std::vector<Position> new_tetromino(4);

	//We just set the relative position for every mino
	switch (i_shape)
	{
	case 0: // I shape
	{
		new_tetromino[0] = { 1, -1 };
		new_tetromino[1] = { 0, -1 };
		new_tetromino[2] = { -1, -1 };
		new_tetromino[3] = { -2, -1 };

		break;
	}
	case 1: // L shape
	{
		new_tetromino[0] = { 0, 0 };
		new_tetromino[1] = { 1, 0 };
		new_tetromino[2] = { -1, -1 };
		new_tetromino[3] = { -1, 0 };

		break;
	}
	case 2: // J shape
	{
		new_tetromino[0] = { 0, 0 };
		new_tetromino[1] = { 1, 0 };
		new_tetromino[2] = { 1, -1 };
		new_tetromino[3] = { -1, 0 };

		break;
	}
	case 3: // O shape
	{
		new_tetromino[0] = { 0, 0 };
		new_tetromino[1] = { 0, -1 };
		new_tetromino[2] = { -1, -1 };
		new_tetromino[3] = { -1, 0 };

		break;
	}
	case 4: // Z shape
	{
		new_tetromino[0] = { 0, 0 };
		new_tetromino[1] = { 1, -1 };
		new_tetromino[2] = { 0, -1 };
		new_tetromino[3] = { -1, 0 };

		break;
	}
	case 5: // T shape
	{
		new_tetromino[0] = { 0, 0 };
		new_tetromino[1] = { 1, 0 };
		new_tetromino[2] = { 0, -1 };
		new_tetromino[3] = { -1, 0 };

		break;
	}
	case 6: // S shape
	{
		new_tetromino[0] = { 0, 0 };
		new_tetromino[1] = { 1, 0 };
		new_tetromino[2] = { 0, -1 };
		new_tetromino[3] = { -1, -1 };
	}
	}

	//Then we add the given x and y to the minos
	for (Position& mino : new_tetromino)
	{
		mino.x += i_x;
		mino.y += i_y;
	}

	//Return the tetromino
	return new_tetromino;
}

std::string Tetris::getTime() {
	std::time_t raw_time = std::time(nullptr);
	struct tm time_info;
	localtime_s(&time_info, &raw_time);
	char buffer[100];
	std::strftime(buffer, 32, "%Y-%m-%d %H:%M:%S", &time_info);
	return buffer;
}

void Tetris::gravityFalls() {
	for (int a = static_cast<int>(ROWS) - 1; a >= 0; a--) {
		if (clear_lines[a]) {
			size_t c;
			for (unsigned int b = 0; b < COLUMNS; b++) {
				c = a;
				while (c > 0) {
					size_t d = c - 1;
					while (d > 0 && matrix[b][d] == 0) {
						d--;
					}
					if (matrix[b][c] != 0)
						std::swap(matrix[b][c - 1], matrix[b][d]);
					else
						std::swap(matrix[b][c], matrix[b][d]);
					drawMatrix(true);
					window.display();
					for (int i = 0; i < 30000; ++i) {
						int j = 1;
						j = j++ * i - i / 2 + i;
						++j;
					}
					c--;
				}
			}
		}
	}
}

void Tetris::setGameSpeed(unsigned int i) {
	current_fall_speed = std::max<int>(MIN_SPEED, current_fall_speed - i);
}
