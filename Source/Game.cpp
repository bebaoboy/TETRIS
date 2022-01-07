#include "Headers/Game.hpp"
#include <iostream>
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

	music_files = {
		{
			0, {
				"Source/Resources/Music/n1.ogg",
				"Source/Resources/Music/m6.ogg",
				"Source/Resources/Music/gnm.ogg"
	        }
		},
		{
			1, {
				"Source/Resources/Music/n1.ogg",
				"Source/Resources/Music/n2.ogg",
				"Source/Resources/Music/n3.ogg",
				"Source/Resources/Music/n8.ogg",
				"Source/Resources/Music/n9.ogg",
				"Source/Resources/Music/n10.ogg"
			}
		},
		{
			2, {
				"Source/Resources/Music/g2.ogg",
				"Source/Resources/Music/g5.ogg",
				"Source/Resources/Music/g9.ogg",
				"Source/Resources/Music/gnm.ogg"
			}
		},
		{
			3, {
				"Source/Resources/Music/t0.ogg",
				"Source/Resources/Music/t1.ogg",
				"Source/Resources/Music/t4.ogg",
				"Source/Resources/Music/t7.ogg",
				"Source/Resources/Music/n8.ogg"
			}
		}
	};

	music_player.emplace_back(std::make_unique<sf::Music>()); // menu
	music_player.emplace_back(std::make_unique<sf::Music>()); // normal
	music_player.emplace_back(std::make_unique<sf::Music>()); // gravity
	music_player.emplace_back(std::make_unique<sf::Music>()); // tetris
}

void Tetris::start() {
	//While the window is open

	player.setBuffer(opening_sound);
	player.play();

	while (window.isOpen() == 1)
	{
		setLagTime();

		if (time_before_hold > 0)
			special = static_cast<int>(duration) - time_before_hold;

		if (time_before_x > 0)
			emergency = static_cast<int>(duration) - time_before_x;

		//While the lag exceeds the maximum allowed frame duration
		while (FRAME_DURATION <= lag)
		{
			lag -= FRAME_DURATION;
			checkEvent();
			if ((player.getStatus() != sf::Sound::Playing && noneIsPaused())
			&& (is_started == 0 || game_over == 1)) {
				playMusic(0); // main menu
			}

			if (is_viewed) {
				pauseMusic();
			}
			else {
				resumeMusic();
			}

			if (game_over == 0 && is_viewed == 0 && is_started == 1
				&& is_tetris == 0 && is_gravity == 0 && !isPlaying(1)) {
				playMusic(1); // normal
			}

			if (game_over == 0 && is_started && is_tetris 
			 && noneIsPaused() && !isPlaying(3) && !isPlaying(2)) {
				playMusic(3); // tetris
			}

			if (game_over == 0 && is_started && is_gravity 
			 && noneIsPaused() && !isPlaying(2) && !isPlaying(3)) {
				playMusic(2); // gravity
			}


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
							scores = static_cast<unsigned int>(std::max<double>(0.,
								static_cast<double>(scores) - NORMAL_DROP_SCORE * .3 *
								(START_FALL_SPEED / static_cast<double>(current_fall_speed))));
						}
						else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
						{
							rotate_pressed = 1;
							tetromino.rotate(0, matrix);
							player.setBuffer(rotate_sound);
							player.play();
							scores = static_cast<unsigned int>(std::max<double>(0., 
								static_cast<double>(scores) - NORMAL_DROP_SCORE * .3 *
								(START_FALL_SPEED / static_cast<double>(current_fall_speed))));
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
							if (!is_tetris)
								scores += HARD_DROP_SCORE * std::min<int>(5, max_speed_count + 1);

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
						scores += SOFT_DROP_SCORE * std::min<int>(8, max_speed_count + 1);
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
							if (peak_score > 0 && scores / peak_score > 0)
							{
								//We increase the game speed
								setGameSpeed(7);
								if (is_tetris)
									peak_score += T_SCORES_TO_INCREASE_SPEED * std::min<unsigned int>(max_speed_count + 1, 5);
								else
									peak_score += SCORES_TO_INCREASE_SPEED * std::min<unsigned int>(max_speed_count + 1, 5);
							}

							//If the effect timer is over
							if (clear_effect_timer == 0) 
								checkLost();
						}
						
						scores += NORMAL_DROP_SCORE * 
							std::min<int>(1000,
								(static_cast<int>((START_FALL_SPEED / static_cast<float>(current_fall_speed))) 
									* max_speed_count)) + scores/10000 * NORMAL_DROP_SCORE / 2;

						//Restart the fall timer
						fall_timer = 0;
					}
					else
					{
						//Increment the fall timer
						if (is_rotated != 0)
							is_rotated++;
						if (rotate_pressed == 0 || (rotate_pressed && is_rotated == ROTATE_LAG))
						{
							fall_timer++;
						}
					}
				}

				//This is the code for restarting the game
				else 
				{
					// replay
					if (is_viewed == 0 && is_started == 0 && 
						sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
						stopMusic();
						playMusic(1); // normal
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

				stopMusic();
			}

			// Hold
			if (game_over == 0 && is_viewed == 0 && is_started == 1
				&& sf::Keyboard::isKeyPressed(sf::Keyboard::H) && holding_tetromino.getShapeCode() == -1) {
				holding_tetromino.reset(tetromino.getShapeCode(), matrix,
					getTetromino(tetromino.getShapeCode(), COLUMNS / 2, 1));
				tetromino.reset(next_shape, matrix, getTetromino(next_shape, COLUMNS / 2, 1));
				fall_timer = 0;
				time_before_hold = static_cast<int>(duration);
			}

			if (game_over == 0 && is_viewed == 0 && is_started == 1
				&& sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && holding_tetromino.getShapeCode() != -1) {
				if (special <= HOLD_DURATION) {
					tetromino.reset(holding_tetromino.getShapeCode(), matrix,
						getTetromino(holding_tetromino.getShapeCode(), COLUMNS / 2, 1));

					fall_timer = 0;
				}
			}

			if (special > HOLD_DURATION) {
				time_before_hold = 0;
				special = 0;
				holding_tetromino.reset(-1, matrix, {});
			}

			// Activate gravity mode
			if (is_gravity == 0 && is_viewed == 0 && sf::Keyboard::isKeyPressed(sf::Keyboard::G)) {
				is_gravity = 1;
				stopMusic();
				playMusic(2); // gravity
			}

			if (is_tetris == 0 && is_viewed == 0 && sf::Keyboard::isKeyPressed(sf::Keyboard::T)) {
				is_tetris = 1;
				stopMusic();
				playMusic(3); // tetris
			}

			if (is_viewed == 1 && is_gravity == 1
				&& sf::Keyboard::isKeyPressed(sf::Keyboard::G)) {
				is_gravity = 0;
				music_player[2]->stop();
			}

			if (is_viewed == 1 && is_tetris == 1
				&& sf::Keyboard::isKeyPressed(sf::Keyboard::T)) {
				is_tetris = 0;
				music_player[3]->stop();
			}

			// Prefill
			if (game_over == 0 && is_viewed == 0 && is_started == 1
				&& sf::Keyboard::isKeyPressed(sf::Keyboard::F)) {
				prefillMode();
			}

			// Emergency
			if (game_over == 0 && is_viewed == 0 && is_started == 1
				&& emergency == 0 
				&& sf::Keyboard::isKeyPressed(sf::Keyboard::X)) {
				tetrisMode();
				time_before_x = static_cast<int>(duration);
			}

			if (emergency > EMERGENCY_DURATION) {
				time_before_x = 0;
				emergency = 0;
			}

			// Music and sound effect
			if (is_viewed == 1
				&& sf::Keyboard::isKeyPressed(sf::Keyboard::M)) {
				setVolume(70);
			}

			if (is_viewed == 1
				&& sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
				player.setVolume(100);
			}

			if (is_viewed == 0 && music_player[0]->getVolume() != 0
				&& sf::Keyboard::isKeyPressed(sf::Keyboard::M)) {
				setVolume(0);
			}

			if (is_viewed == 0 && player.getVolume() != 0
				&& sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
				player.setVolume(0);
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
			if (is_tetris)
			{
				//We increase the game speed
				if (lines_cleared % T_LINES_TO_INCREASE_SPEED == 0)
					setGameSpeed(2);
			}
			else {
				if (lines_cleared % LINES_TO_INCREASE_SPEED == 0)
					setGameSpeed(2);
			}
		}
	}
	if (tetris_count >= 4) {
		scores += TETRIS_BONUS_SCORE * max_speed_count;
		if (tetris_count > 4)
			scores += (tetris_count - 4) * CLEAR_LINE_SCORE * max_speed_count;
	}	
}

void Tetris::checkLost() {
	//Decide if the game is over or not based on the return value of the reset function
	game_over = tetromino.reset(next_shape, matrix, getTetromino(next_shape, COLUMNS / 2, 1)) == 0;

	//Generate the next shape
	next_shape = shape_distribution(random_engine);

	if (game_over == 1) {

		player.setBuffer(lost_sound);
		player.play();

		tetromino.hardDrop(matrix);
		scores += std::max<int>((lines_cleared - LINE_BONUS) * LINE_BONUS_SCORE, 0);

		score_list.emplace_back(getTime(),std::make_pair(timer(duration, clock),scores));

		stopMusic();
	}
}

void Tetris::restart(float& duration) {
	//We set everything to 0
	game_over = 0;
	hardDrop_pressed = 0;
	rotate_pressed = 0;
	is_tetris = 0;
	is_gravity = 0;

	lines_cleared = 0;
	scores = 0;
	peak_score = SCORES_TO_INCREASE_SPEED;

	max_speed_count = 0;
	current_fall_speed = START_FALL_SPEED;
	fall_timer = 0;
	move_timer = 0;
	soft_drop_timer = 0;
	duration = 0;

	time_before_hold = 0;
	special = 0;
	holding_tetromino.reset(-1, matrix, {});
	time_before_x = 0;
	emergency = 0;

	next_shape = shape_distribution(random_engine);
	tetromino.reset(next_shape, matrix, getTetromino(next_shape, COLUMNS / 2, 1));

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

std::vector<Position> Tetris::getTetromino(unsigned char i_shape, unsigned char i_x, unsigned char i_y) {
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
					delay();
					c--;
				}
			}
		}
	}
	delay(21999);
	if (player.getStatus() != sf::Sound::Playing) {

		player.setBuffer(normal_drop_sound);
		player.play();
	}
}
#include <iostream>
void Tetris::setGameSpeed(unsigned int i) {
	if (11.f <= (START_FALL_SPEED / static_cast<float>(current_fall_speed))) {
		if (music_player[0]->getVolume() != 0)
			setVolume(40);

		player.setBuffer(opening_sound);
		player.play();

		max_speed_count++;

		if (is_tetris) {
			tetrisMode();
			current_fall_speed = std::max<int>(18, START_FALL_SPEED - max_speed_count);
			scores += LINE_BONUS_SCORE * max_speed_count;
		}			
		else {
			if (player.getStatus() != sf::Sound::Playing) {

				player.setBuffer(opening_sound);
				player.play();
			}
			maxSpeedReached();
			current_fall_speed = std::max<int>(15, START_FALL_SPEED - max_speed_count);
			scores += LINE_BONUS_SCORE * max_speed_count * 2;
		}	

		fall_timer = 0;
	}
	else {
		current_fall_speed = std::max<int>(static_cast<int>(current_fall_speed) - i, 4);
	}
	if (music_player[0]->getVolume() != 0)
		setVolume(70);
}

void Tetris::tetrisMode() {

	std::random_device rd; 
	std::mt19937 seed(rd());
	std::uniform_int_distribution<int> r_row(0, ROWS - 1), 
		r_col(0, COLUMNS - 1),
		del_num(27, ROWS * 3 - max_speed_count),
		colors(0,6);

	int num_del_cells = del_num(seed);
	std::vector<Position> del_cells;
	for (int i = 0; i < num_del_cells; ++i) {
		del_cells.push_back(Position{ r_col(seed), r_row(seed) });
	}

	clear_lines[ROWS - 1] = 1;

	for (int i = 1; i <= num_del_cells * 20; ++i) {
		
		// random tile color
		int r = r_col(seed), c = r_row(seed);
		int ori = matrix[r][c];
		matrix[r][c] = colors(seed);
		drawMatrix();
		window.display();

		int r2 = r_col(seed), c2 = r_row(seed);
		int ori2 = matrix[r2][c2];
		matrix[r2][c2] = colors(seed);
		drawMatrix();
		window.display();

		matrix[r][c] = ori;
		matrix[r2][c2] = ori2;

		// random delete cells
		if (i % 20 == 0) {

			delay(20000);

			matrix[del_cells.back().x][del_cells.back().y] = 0;
			del_cells.pop_back();

			drawText(static_cast<unsigned int>(CELL_SIZE * (1 + COLUMNS) + 8),
			static_cast<unsigned int>(0.5f * CELL_SIZE * ROWS) + 45,
			"TETRIS MODE", window, .45f);
			window.display();
		}

		delay();
	}
	
	// collapse
	gravityFalls();

	clear_lines[ROWS - 1] = 0;

	//clearLines();

}

void Tetris::maxSpeedReached() {
	// perform clear line
	delay();

	std::random_device rd;
	std::mt19937 seed(rd());
	std::uniform_int_distribution<int>
		del_nums(2, 3),
		del_line(ROWS - 6, ROWS - 1),
		del_color(0, 6);

	for (int i = 1; i <= del_nums(seed); ++i) {
		drawText(static_cast<unsigned int>(CELL_SIZE * (1 + COLUMNS) + 8),
			static_cast<unsigned int>(0.5f * CELL_SIZE * ROWS) + 45,
			"MAX SPEED REACHED", window, .45f);

		int line = del_line(seed);

		delay();
		delay(30001);
		for (int j = 0; j < COLUMNS; j++) {
			matrix[j][line] = 0;
		}

		if (line > 15) {
			int t = del_color(seed);
			if (clear_lines[t] != 1) {
				for (int i = 0; i < COLUMNS; i++) {
					if (matrix[i][t] == 1) {
						break;
					}
				}
				for (int j = 0; j < COLUMNS; j++) {
					matrix[j][t] = 0;
				}
			}
		}

		delay(30001);
		delay(30030);
	}

	delay();
	delay();

	clear_lines[ROWS-1] = 1;

	for (unsigned int a = 0; a < ROWS; a++)
	{
		bool is_cleared = 1;
		//Check if the every cell in the row is filled or not
		for (unsigned int b = 0; b < COLUMNS; b++)
		{
			if (matrix[b][a] == 0)
			{
				is_cleared = 0;
				break;
			}
		}
		if (is_cleared) {
			for (unsigned int b = 0; b < COLUMNS; b++)
			{
				matrix[b][a] = 0;
			}
		}		
	}

	gravityFalls();
	//clearLines();

	window.display();
	clear_lines[ROWS - 1] = 0;

	//Clear the clear lines array
	//std::fill(clear_lines.begin(), clear_lines.end(), 0);
}

void Tetris::prefillMode() {
	std::random_device rd;
	std::mt19937 seed(rd());
	std::uniform_int_distribution<int> row_num(3, ROWS - 10);
	std::uniform_int_distribution<int> r_row(ROWS - row_num(seed), ROWS - 1),
		r_col(0, COLUMNS - 1),
		new_num(70, (ROWS - 10) * COLUMNS - 10),
		del_num(1, 4),
		shapes(0,6);

	int num_new_cells = new_num(seed);
	std::vector<Position> new_cells;
	for (int i = 0; i < num_new_cells; ++i) {
		auto temp = Position{ r_col(seed), r_row(seed) };
		while (matrix[temp.x][temp.y] != 0)
			temp = Position{ r_col(seed), r_row(seed) };
		new_cells.push_back(temp);
	}

	for (int i = 1; i <= num_new_cells * 20; ++i) {
		// random delete cells
		if (i % 20 == 0) {

			delay(20000);
			
			matrix[new_cells.back().x][new_cells.back().y] = shapes(seed);

			new_cells.pop_back();

			drawText(static_cast<unsigned int>(CELL_SIZE * (1 + COLUMNS) + 8),
				static_cast<unsigned int>(0.5f * CELL_SIZE * ROWS) + 45,
				"PREFILL_ING", window, .45f);
			window.display();
		}

		delay();
	}
	
	std::vector<int> full_lines;

	for (int i = 0; i < ROWS; ++i) {
		bool full_line = true;
		for (int j = 0; j < COLUMNS; ++j) {
			if (matrix[j][i] == 0) {
				full_line = false;
				break;
			}
		}
		if (full_line)
			full_lines.push_back(i);
	}

	for (int i : full_lines) {
		for (int _ = 0; _ < del_num(seed); ++_)
			matrix[r_col(seed)][i] = 0;
		delay();
		drawMatrix();
		window.display();
	}
}

void Tetris::delay(int num) {
	for (int i = 0; i < num; ++i) {
		int j = 1;
		j = j++ * i - i / 2 + i;
		++j;
	}
}

void Tetris::playMusic(int choice, int volume) {
	sf::Music *player = music_player[choice].get();
	if (player->getStatus() != sf::Music::Playing) {
		player->openFromFile(getMusic(choice));
		if (player->getVolume() != 0)
			player->setVolume(static_cast<float>(volume));
		player->play();
	}
}

bool Tetris::isPlaying(std::unique_ptr<sf::Music>& ms) {
	return ms->getStatus() == sf::Music::Playing;
}

bool Tetris::isPlaying(int choice) {
	return music_player[choice]->getStatus() == sf::Music::Playing;
}

bool Tetris::noneIsPaused() {
	for (auto& ms : music_player)
		if (ms->getStatus() == sf::Music::Paused)
			return false;
	return true;
}

void Tetris::stopMusic() {
	for (auto& ms : music_player)
		if (isPlaying(ms))
			ms->stop();
}

void Tetris::pauseMusic() {
	for (auto& ms : music_player)
		if (isPlaying(ms))
			ms->pause();
}

void Tetris::resumeMusic() {
	for (auto& ms : music_player)
		if (ms->getStatus() == sf::Music::Paused)
			ms->play();
}

std::string Tetris::getMusic(int choice) {
	std::uniform_int_distribution<int> 
		music_distribution{ 0, static_cast<int>(music_files[static_cast<size_t>(choice)].size()) - 1 };
	return music_files[choice][music_distribution(random_engine)];	
}

void Tetris::setVolume(int volume) {
	for (auto& ms : music_player)
		ms->setVolume(static_cast<float>(volume));
}
