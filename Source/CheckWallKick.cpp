#include <vector>

#include "Headers/Game.hpp"

std::vector<Position> checkWallKick(bool is_I_shape, unsigned char current_orientation, unsigned char next_rotation)
{
	//We return the wall kick data based on the current and next rotations. We also check whether the shape is I or not
	//Fun fact, the website that provided the data had the y-coordinate flipped so I had to flip every y's sign
	//I have NO idea why did they even do this

	if (is_I_shape == 0)
	{
		switch (current_orientation)
		{
			case 0:
			case 2:
			{
				switch (next_rotation)
				{
					case 1:
					{
						return {{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}};
					}
					case 3:
					{
						return {{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}};
					}
				}
			}
			case 1:
			{
				return {{0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2}};
			}
			case 3:
			{
				return {{0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2}};
			}
		}

		return {{0, 0}};
	}
	else
	{
		switch (current_orientation)
		{
			case 0:
			{
				switch (next_rotation)
				{
					case 1:
					{
						return {{0, 0}, {-2, 0}, {1, 0}, {-2, 1}, {1, -2}};
					}
					case 3:
					{
						return {{0, 0}, {-1, 0}, {2, 0}, {-1, -2}, {2, 1}};
					}
				}
			}
			case 1:
			{
				switch (next_rotation)
				{
					case 0:
					{
						return {{0, 0}, {2, 0}, {-1, 0}, {2, -1}, {-1, 2}};
					}
					case 2:
					{
						return {{0, 0}, {-1, 0}, {2, 0}, {-1, -2}, {2, 1}};
					}
				}
			}
			case 2:
			{
				switch (next_rotation)
				{
					case 1:
					{
						return {{0, 0}, {1, 0}, {-2, 0}, {1, 2}, {-2, -1}};
					}
					case 3:
					{
						return {{0, 0}, {2, 0}, {-1, 0}, {2, -1}, {-1, 2}};
					}
				}
			}
			case 3:
			{
				switch (next_rotation)
				{
					case 0:
					{
						return {{0, 0}, {1, 0}, {-2, 0}, {1, 2}, {-2, -1}};
					}
					case 2:
					{
						return {{0, 0}, {-2, 0}, {1, 0}, {-2, 1}, {1, -2}};
					}
				}
			}
		}

		return {{0, 0}};
	}
}