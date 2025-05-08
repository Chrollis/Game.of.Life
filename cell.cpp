#include "cell.h"

Cell::Cell() {
	alive = 0;
	active = 0;
}
Cell::Cell(bool alive, bool active) {
	this->alive = alive;
	this->active = active;
}
Cell& Cell::operator=(const Cell& input) {
	alive = input.alive;
	active = input.active;
	return *this;
}

bool Cell::survive(const Matrix<Cell, 3, 3>& input) {
	int sum = 0;
	for (int i = 0; i < 9; i++) {
		if (i != 4) {
			sum += (int)input(i / 3, i % 3).alive;
		}
	}
	if (!alive && sum == 3) {
		alive = 1;
		return 1;
	}
	else if (alive && (sum < 2 || sum>3)) {
		alive = 0;
		return 1;
	}
	else {
		active = 0;
		return 0;
	}
}

void Cell::unload_block(const Matrix<Cell, BLOCKCELL, BLOCKCELL>& block, int x, int y) {
	ofstream ofs(".\\cache\\block[" + to_string(x) + ", " + to_string(y) + "].bin", ios::binary);
	if (ofs.is_open()) {
		for (int row = 0; row < BLOCKCELL; row++) {
			for (int col = 0; col < BLOCKCELL; col++) {
				ofs << block(row, col).alive << ' '
					<< block(row, col).active << endl;
			}
		}
		ofs.close();
	}
}
void Cell::load_block(Block<Matrix<Cell, WHOLECELL, WHOLECELL, 0, WHOLECELL, WHOLECELL>, BLOCKCELL, BLOCKCELL> block, int x, int y) {
	ifstream ifs(".\\cache\\block[" + to_string(x) + ", " + to_string(y) + "].bin", ios::binary);
	int alive = 0, active = 0;
	if (ifs.is_open()) {
		for (int row = 0; row < BLOCKCELL; row++) {
			for (int col = 0; col < BLOCKCELL; col++) {
				ifs >> alive >> active;
				block(row, col).alive = (bool)alive;
				block(row, col).active = (bool)active;
			}
		}
		ifs.close();
	}
	else {
		for (int row = 0; row < BLOCKCELL; row++) {
			for (int col = 0; col < BLOCKCELL; col++) {
				block(row, col).alive = 0;
				block(row, col).active = 0;
			}
		}
	}
}