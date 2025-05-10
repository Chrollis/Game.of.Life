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
	static void unload_block(const Matrix<Cell, BSC, BSC>& block, int x, int y);
	template<int _Ts, int _Bs> static void load_block(Block<Matrix<Cell, _Ts, _Ts, 0, _Ts, _Ts>, _Bs, _Bs> block, int x, int y) {
		ifstream ifs(".\\cache\\block[" + to_string(x) + ", " + to_string(y) + "].bin", ios::binary);
		int alive = 0, active = 0;
		if (ifs.is_open()) {
			for (int row = 0; row < BSC; row++) {
				for (int col = 0; col < BSC; col++) {
					ifs >> alive >> active;
					block(row, col).alive = (bool)alive;
					block(row, col).active = (bool)active;
				}
			}
			ifs.close();
		}
		else {
			for (int row = 0; row < BSC; row++) {
				for (int col = 0; col < BSC; col++) {
					block(row, col).alive = 0;
					block(row, col).active = 0;
				}
			}
		}
	}
	friend class Game;
};

