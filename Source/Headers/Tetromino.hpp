#pragma once
#include "Headers/Global.hpp"

class Tetromino
{
	unsigned int rotation = 0;
	unsigned int shape;

	std::vector<Position> minos;
public:
	Tetromino(unsigned int shape, std::vector<Position> mino);

	bool moveDown(const MainMatrix& matrix);
	bool reset(unsigned int shape, const MainMatrix& matrix, std::vector<Position> mino);

	unsigned int getShapeCode(); 

	void hardDrop(const MainMatrix& matrix);
	void moveLeft(const MainMatrix& matrix);
	void moveRight(const MainMatrix& matrix);

	void updateToMatrix(MainMatrix& matrix);

	std::vector<Position> getGhostMino(const MainMatrix& matrix);
	std::vector<Position> getMino();

	virtual void rotate(bool i_clockwise, const MainMatrix& matrix);

};