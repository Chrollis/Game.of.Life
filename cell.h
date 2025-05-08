#pragma once
#include "utils.h"

class Cell {
public:
	bool alive : 1;
	bool active : 1;
public:
	Cell();
	Cell(bool alive, bool active);
	~Cell() = default;
	Cell& operator=(const Cell& input);
private:
	bool survive(const Matrix<Cell, 3, 3>& input);
	static void unload_block(const Matrix<Cell, BLOCKCELL, BLOCKCELL>& block, int x, int y);
	static void load_block(Block<Matrix<Cell, WHOLECELL, WHOLECELL, 0, WHOLECELL, WHOLECELL>, BLOCKCELL, BLOCKCELL> block, int x, int y);
	friend class Game;
};

