#pragma once
#include "cell.h"

class Game {
public:
	Game() = default;
	~Game() = default;

	void enter();
	void proceed(clock_t delta);
	void input(const ExMessage& msg);
	void draw();
	void quit();
private:
	void zoom();
	void camera_move();
	void centre_move(int x, int y);
	void cell_click(bool alive);
private:
	struct Camera {
		int x;
		int y;
		int speed;
	}camera = { 0,0,1 };
	bool running = 0;
	bool shift = 0;
	bool ctrl = 0;
	bool moving[4] = { 0,0,0,0 };
	bool zooming[2] = { 0,0 };
	clock_t compute_interval = 300;
	int scale = ORISCALE;
	POINT centre = { 2,2 };
	POINT cursor = { 0,0 };
private:
	Matrix<Cell, WHOLECELL, WHOLECELL> cells;
	enum Direction4 {
		UP,
		DOWN,
		LEFT,
		RIGHT
	};
	enum Direction2 {
		LARGE,
		SMALL
	};
public:
	FpsController fps_controller;
};