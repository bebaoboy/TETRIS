#include <vector>
#include <utility>

#include "Headers/Tetromino.hpp"
#include "Headers/CheckWallKick.hpp"

Tetromino::Tetromino(int shape, std::vector<Position> mino) :
	shape{ shape },
	minos{ std::move(mino) } {}

bool Tetromino::moveDown(const MainMatrix& matrix)
{
	for (Position& mino : minos)
	{
		//Will we go outside the matrix if we move down?
		if (ROWS == 1 + mino.y)
		{
			return 0;
		}

		//Will we hit another tetromino if we move down?
		if (0 < matrix[mino.x][static_cast<std::vector<int>::size_type>(1) + mino.y])
		{
			return 0;
		}
	}

	//Move the tetromino down
	for (Position& mino : minos)
	{
		mino.y++;
	}

	//Return that everything is okay
	return 1;
}

void Tetromino::hardDrop(const MainMatrix& matrix)
{
	minos = getGhostMino(matrix);
}

void Tetromino::moveLeft(const MainMatrix& matrix)
{
	for (Position& mino : minos)
	{
		if (mino.x - 1 < 0)
			return;

		if (mino.y < 0)
			continue;
		else if (matrix[static_cast<size_t>(mino.x) - 1][mino.y] > 0)
			return;
	}

	for (Position& mino : minos)
		mino.x--;
}

void Tetromino::moveRight(const MainMatrix& matrix)
{
	for (Position& mino : minos)
	{
		if (COLUMNS == 1 + mino.x)
			return;

		if (mino.y < 0)
			continue;
		else if (matrix[static_cast<size_t>(1) + mino.x][mino.y] > 0)
			return;
	}

	for (Position& mino : minos)
		mino.x++;
}

void Tetromino::rotate(bool clockwise, const MainMatrix& matrix)
{
	if (shape != 3)
	{
		unsigned int next_rotation;

		std::vector<Position> current_minos = minos;

		//Calculating the next rotation state
		if (clockwise == 0)
		{
			next_rotation = (3 + rotation) % 4;
		}
		else
		{
			next_rotation = (1 + rotation) % 4;
		}

		//If it's the I shape
		if (shape == 0)
		{
			//We find it's center
			float center_x = 0.5f * (minos[1].x + minos[2].x);
			float center_y = 0.5f * (minos[1].y + minos[2].y);

			switch (rotation)
			{
				case 0:
				{
					center_y += 0.5f;

					break;
				}
				case 1:
				{
					center_x -= 0.5f;

					break;
				}
				case 2:
				{
					center_y -= 0.5f;

					break;
				}
				case 3:
				{
					center_x += 0.5f;
				}
			}

			//Then we rotate every mino based on the center
			for (Position& mino : minos)
			{
				//We find the position of the mino relative to the center
				float x = mino.x - center_x;
				float y = mino.y - center_y;

				if (clockwise == 0)
				{
					mino.x = static_cast<char>(center_x + y);
					mino.y = static_cast<char>(center_y - x);
				}
				else
				{
					mino.x = static_cast<char>(center_x - y);
					mino.y = static_cast<char>(center_y + x);
				}
			}
		}
		else
		{
			//We don't rotate the mino that's located at the center of rotation
			for (unsigned int a = 1; a < minos.size(); a++)
			{
				//We find the position of the mino relative to the central mino
				char x = minos[a].x - minos[0].x;
				char y = minos[a].y - minos[0].y;

				if (clockwise == 0)
				{
					minos[a].x = y + minos[0].x;
					minos[a].y = minos[0].y - x;
				}
				else
				{
					minos[a].x = minos[0].x - y;
					minos[a].y = x + minos[0].y;
				}
			}
		}

		//We try every vector from the wall kick data
		for (Position& wall_kick : checkWallKick(shape == 0, rotation, next_rotation))
		{
			bool can_turn = 1;

			for (Position& mino : minos)
			{
				//Here we're checking every collision that can happen
				if (0 > mino.x + wall_kick.x || COLUMNS <= mino.x + wall_kick.x || ROWS <= mino.y + wall_kick.y)
				{
					can_turn = 0;

					break;
				}

				if (0 > mino.y + wall_kick.y)
				{
					continue;
				}
				else if (0 < matrix[static_cast<size_t>(mino.x) + wall_kick.x][static_cast<size_t>(mino.y) + wall_kick.y])
				{
					can_turn = 0;

					break;
				}
			}

			//If we can turn
			if (can_turn)
			{
				//We turn
				rotation = next_rotation;

				for (Position& mino : minos)
				{
					mino.x += wall_kick.x;
					mino.y += wall_kick.y;
				}

				return;
			}
		}

		//Since we applied changes to the minos array, we're resetting it when we can't rotate the tetromino
		minos = current_minos;
	}
}

void Tetromino::updateToMatrix(MainMatrix& matrix)
{
	//Putting the tetromino to the matrix
	for (Position& mino : minos)
	{
		if (mino.y < 0)
			continue;

		matrix[mino.x][mino.y] = 1 + shape;
	}
}

std::vector<Position> Tetromino::getGhostMino(const MainMatrix& matrix)
{
	//We're just moving the tetromino down until it hits something. Then we're returning it's position
	bool keep_falling = 1;

	unsigned int total_movement = 0;

	std::vector<Position> ghost_minos = minos;

	while (keep_falling)
	{
		total_movement++;

		for (Position& mino : minos)
		{
			if (ROWS == total_movement + mino.y)
			{
				keep_falling = 0;
				break;
			}

			if (0 > total_movement + mino.y)
			{
				continue;
			}
			else if (0 < matrix[mino.x][static_cast<size_t>(total_movement) + mino.y])
			{
				keep_falling = 0;
				break;
			}
		}
	}

	for (Position& mino : ghost_minos)
	{
		mino.y += total_movement - 1;
	}

	return ghost_minos;
}

std::vector<Position> Tetromino::getMino()
{
	return minos;
}

bool Tetromino::reset(int shape, const MainMatrix& matrix, std::vector<Position> mino) {
	//Reset the variables
	rotation = 0;
	this->shape = shape;

	minos = std::move(mino);

	for (Position& mino : minos)
	{
		if (matrix[mino.x][mino.y] > 0)
		{
			//Return that we can't reset because there's a tetromino at the spawn location
			return 0;
		}
	}

	//Return that everything is fine
	return 1;
}

int Tetromino::getShapeCode() {
	//I'm gonna take a wild guess and say that this return the shape of the tetromino
	return shape;
}
