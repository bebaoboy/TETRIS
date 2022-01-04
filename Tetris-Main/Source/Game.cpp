#include "Headers/Game.hpp"

Tetris::Tetris() {
	//Resizing the window
	window.setView(sf::View(sf::FloatRect(0, 0, 2 * CELL_SIZE * COLUMNS, CELL_SIZE * ROWS)));

	//Generate a random shape and store it as the next shape
	next_shape = static_cast<unsigned int>(shape_distribution(random_engine));

	//Get the current time and store it in the variable
	previous_time = std::chrono::steady_clock::now();
}

void Tetris::start() {
	//While the window is open
	sf::Clock clock;
	float duration = 0.f;
	while (1 == window.isOpen())
	{
		setLagTime();

		//While the lag exceeds the maximum allowed frame duration
		while (FRAME_DURATION <= lag)
		{
			lag -= FRAME_DURATION;

			checkEvent();

			//If the clear effect timer is 0
			if (0 == clear_effect_timer)
			{
				//If the game over is 0
				if (0 == game_over)
				{
					//If the rotate pressed is 0 (Damn, I'm so good at commenting!)
					if (0 == rotate_pressed)
					{
						if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::C))
						{
							//Rotation key is pressed!
							rotate_pressed = 1;

							//Do a barrel roll
							tetromino.rotate(1, matrix);
						} 
						else if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
						{
							rotate_pressed = 1;

							tetromino.rotate(0, matrix);
						}
					}

					if (0 == move_timer)
					{
						if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
						{
							//Reset the move timer
							move_timer = 1;

							//Move the tetromino to the left
							tetromino.move_left(matrix);
						}
						else if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
						{
							move_timer = 1;

							tetromino.move_right(matrix);
						}
					}
					else
					{
						//Update the move timer
						move_timer = (1 + move_timer) % MOVE_SPEED;
					}

					//If hard drop is not pressed
					if (0 == hard_drop_pressed)
					{
						//But the Space is pressed, which is the hard drop key (Paradox?)
						if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
						{
							//Get rid of the paradox!
							hard_drop_pressed = 1;

							//Reset the fall timer
							fall_timer = current_fall_speed;

							//Make the falling tetromino drop HAAAAARD!
							tetromino.hard_drop(matrix);

							// play sound and glowing
						}
					}

					if (0 == soft_drop_timer)
					{
						if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
						{
							if (1 == tetromino.move_down(matrix))
							{
								fall_timer = 0;
								soft_drop_timer = 1;
							}
						}
					}
					else
					{
						soft_drop_timer = (1 + soft_drop_timer) % SOFT_DROP_SPEED;
					}

					//If the fall timer is over
					if (current_fall_speed == fall_timer)
					{
						//If the tetromino can't move down anymore
						if (0 == tetromino.move_down(matrix))
						{
							//Put the falling tetromino to the matrix
							tetromino.update_matrix(matrix);

							//Loop through every row
							for (unsigned int a = 0; a < ROWS; a++)
							{
								//Here we're gonna check if the current row should be cleared or not
								bool clear_line = 1;

								//Check if the every cell in the row is filled or not
								for (unsigned int b = 0; b < COLUMNS; b++)
								{
									if (0 == matrix[b][a])
									{
										clear_line = 0;

										break;
									}
								}

								//If we have to clear it
								if (1 == clear_line)
								{
									//WE CLEAR IT!
									//First we increase the score
									lines_cleared++;

									//Then we start the effect timer
									clear_effect_timer = CLEAR_EFFECT_DURATION;

									//Set the current row as the row that should be cleared
									clear_lines[a] = 1;

									//If the player reached a certain number of lines
									if (0 == lines_cleared % LINES_TO_INCREASE_SPEED)
									{
										//We increase the game speed
										current_fall_speed = std::max<int>(1, current_fall_speed - 7);
									}
								}
							}

							//If the effect timer is over
							if (0 == clear_effect_timer)
							{
								//Decide if the game is over or not based on the return value of the reset function
								game_over = 0 == tetromino.reset(next_shape, matrix, get_tetromino(next_shape, COLUMNS / 2, 1));

								//Generate the next shape
								next_shape = static_cast<unsigned int>(shape_distribution(random_engine));
							}
						}

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
				else if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
				{
					//We set everything to 0
					game_over = 0;
					hard_drop_pressed = 0;
					rotate_pressed = 0;

					lines_cleared = 0;

					current_fall_speed = START_FALL_SPEED;
					fall_timer = 0;
					move_timer = 0;
					soft_drop_timer = 0;

					//Then we clear the matrix
					for (auto& a : matrix)
					{
						std::fill(a.begin(), a.end(), 0);
					}
				}
			}
			else
			{
				//Decrement the effect timer
				clear_effect_timer--;

				if (0 == clear_effect_timer)
				{
					//Loop through each row
					for (unsigned int a = 0; a < ROWS; a++)
					{
						//If the row should be cleared
						if (1 == clear_lines[a])
						{
							//Loop through each cell in the row
							for (unsigned int b = 0; b < COLUMNS; b++)
							{
								//Set the cell to 0 (empty) (the absence of existence)
								matrix[b][a] = 0;

								//Swap the row with the rows above
								for (std::vector<int>::size_type c = a; 0 < c; c--)
								{
									matrix[b][c] = matrix[b][c - 1];
									matrix[b][c - 1] = 0;
								}
							}
						}
					}

					game_over = 0 == tetromino.reset(next_shape, matrix, get_tetromino(next_shape, COLUMNS / 2, 1));

					next_shape = static_cast<unsigned int>(shape_distribution(random_engine));

					//Clear the clear lines array
					std::fill(clear_lines.begin(), clear_lines.end(), 0);
				}
			}

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
	while (1 == window.pollEvent(event))
	{
		//Check the event type
		switch (event.type)
		{
			//If the user closed the game
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
			switch (event.key.code)
			{
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
				hard_drop_pressed = 0;
			}
			}
		}
		}
	}
}

void Tetris::drawBoard(std::string timerString) {
	//Here we're drawing everything!
	if (FRAME_DURATION > lag)
	{
		//Calculating the size of the effect squares
		float clear_cell_size =
			(2 * round(0.5f * CELL_SIZE * (clear_effect_timer / static_cast<float>(CLEAR_EFFECT_DURATION))));

		//We're gonna use this object to draw every cell in the game
		sf::RectangleShape cell(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));
		//Next shape preview border (White square at the corner)
		sf::RectangleShape preview_border(sf::Vector2f(5 * CELL_SIZE, 5 * CELL_SIZE));
		preview_border.setFillColor(sf::Color(0, 0, 0));
		preview_border.setOutlineThickness(-1);
		preview_border.setPosition(CELL_SIZE * (1.5f * COLUMNS - 2.5f), CELL_SIZE * (0.25f * ROWS - 2.5f));

		//Clear the window from the previous frame
		window.clear();

		//Draw the matrix
		for (unsigned int a = 0; a < COLUMNS; a++)
		{
			for (unsigned int b = 0; b < ROWS; b++)
			{
				if (0 == clear_lines[b])
				{
					cell.setPosition(static_cast<float>(CELL_SIZE * a), static_cast<float>(CELL_SIZE * b));

					if (1 == game_over && 0 < matrix[a][b])
					{
						cell.setFillColor(cell_colors[8]);
					}
					else
					{
						cell.setFillColor(cell_colors[matrix[a][b]]);
					}

					window.draw(cell);
				}
			}
		}

		//If the game is not over
		if (0 == game_over)
		{
			//Set the cell color to gray for ghost tetromino
		    cell.setFillColor(cell_colors[8]);

			//Drawing the ghost tetromino
			for (Position& mino : tetromino.get_ghost_minos(matrix))
			{
				cell.setPosition(static_cast<float>(CELL_SIZE * mino.x), static_cast<float>(CELL_SIZE * mino.y));

				window.draw(cell);
			}

			cell.setFillColor(cell_colors[static_cast<size_t>(1) + tetromino.get_shape()]);
		}

		//Drawing the falling tetromino
		for (Position& mino : tetromino.get_minos())
		{
			cell.setPosition(static_cast<float>(CELL_SIZE * mino.x), static_cast<float>(CELL_SIZE * mino.y));

			window.draw(cell);
		}

		//Drawing the effect
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
				}
			}
		}

		cell.setFillColor(cell_colors[1 + static_cast<size_t>(next_shape)]);
		cell.setSize(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));

		//Draw the preview border
		window.draw(preview_border);

		//Draw the next tetromino
		for (Position& mino : get_tetromino(next_shape,
			static_cast<unsigned int>(1.5f * COLUMNS), static_cast<unsigned int>(0.25f * ROWS)))
		{
			//Shifting the tetromino to the center of the preview border
			float next_tetromino_x = CELL_SIZE * mino.x;
			float next_tetromino_y = CELL_SIZE * mino.y;

			if (0 == next_shape)
			{
				next_tetromino_y += static_cast<unsigned int>(round(0.5f * CELL_SIZE));
			}
			else if (3 != next_shape)
			{
				next_tetromino_x -= static_cast<unsigned int>(round(0.5f * CELL_SIZE));
			}

			cell.setPosition(next_tetromino_x, next_tetromino_y);

			window.draw(cell);
		}

		//Drawing the text
		draw_text(static_cast<unsigned int>(CELL_SIZE * (1 + COLUMNS) + 5),
			static_cast<unsigned int>(0.5f * CELL_SIZE * ROWS),
			"Lines:" + std::to_string(lines_cleared) +
			"\nSpeed:" + std::to_string(START_FALL_SPEED / static_cast<float>(current_fall_speed)) + 'x' +
			"\nTime elapsed: " + timerString, window);
		window.display();

	}
}

std::string Tetris::timer(float& duration, sf::Clock& clock) {
	sf::Time time = clock.restart();
	float fMilliseconds, fSeconds;
	int intMilliseconds, intSeconds;
	int minutes = 0, hours = 0;
	sf::String stringMilliseconds;
	sf::String stringSeconds, stringMinutes, stringHours;
	sf::String timerString;

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

	timerString = stringHours + ":" + stringMinutes + ":" + stringSeconds/* + ":" + stringMilliseconds*/;
	return timerString;
}
