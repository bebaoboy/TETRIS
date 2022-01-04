#pragma once

class Tetromino
{
	unsigned int rotation = 0;
	unsigned int shape;

	std::vector<Position> minos;
public:
	Tetromino(unsigned int i_shape, std::vector<Position> mino);

	bool move_down(const MainMatrix& i_matrix);
	bool reset(unsigned int i_shape, const MainMatrix& i_matrix, std::vector<Position> mino);

	unsigned int get_shape(); 

	void hard_drop(const MainMatrix& i_matrix);
	void move_left(const MainMatrix& i_matrix);
	void move_right(const MainMatrix& i_matrix);

	void update_matrix(MainMatrix& i_matrix);

	std::vector<Position> get_ghost_minos(const MainMatrix& i_matrix);
	std::vector<Position> get_minos();

	virtual void rotate(bool i_clockwise, const MainMatrix& i_matrix);

};