#include <vector>

#include "Headers/Global.hpp"
#include "Headers/GetTetromino.hpp"

std::vector<Position> get_tetromino(unsigned char i_shape, unsigned char i_x, unsigned char i_y)
{
	std::vector<Position> new_tetromino(4);

	//We just set the relative position for every mino
	switch (i_shape)
	{
		case 0: // I shape
		{
			new_tetromino[0] = {1, -1};
			new_tetromino[1] = {0, -1};
			new_tetromino[2] = {-1, -1};
			new_tetromino[3] = {-2, -1};

			break;
		}
		case 1: // L shape
		{
			new_tetromino[0] = {0, 0};
			new_tetromino[1] = {1, 0};
			new_tetromino[2] = {-1, -1};
			new_tetromino[3] = {-1, 0};

			break;
		}
		case 2: // J shape
		{
			new_tetromino[0] = {0, 0};
			new_tetromino[1] = {1, 0};
			new_tetromino[2] = {1, -1};
			new_tetromino[3] = {-1, 0};

			break;
		}
		case 3: // O shape
		{
			new_tetromino[0] = {0, 0};
			new_tetromino[1] = {0, -1};
			new_tetromino[2] = {-1, -1};
			new_tetromino[3] = {-1, 0};

			break;
		}
		case 4: // Z shape
		{
			new_tetromino[0] = {0, 0};
			new_tetromino[1] = {1, -1};
			new_tetromino[2] = {0, -1};
			new_tetromino[3] = {-1, 0};

			break;
		}
		case 5: // T shape
		{
			new_tetromino[0] = {0, 0};
			new_tetromino[1] = {1, 0};
			new_tetromino[2] = {0, -1};
			new_tetromino[3] = {-1, 0};

			break;
		}
		case 6: // S shape
		{
			new_tetromino[0] = {0, 0};
			new_tetromino[1] = {1, 0};
			new_tetromino[2] = {0, -1};
			new_tetromino[3] = {-1, -1};
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