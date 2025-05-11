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
	void expand(int s);
	bool camera_move();
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
	int expanding = -1;
	clock_t compute_interval = 300;
	int scale = ORI_SCALE;
	int wsb = ORI_WSB;//whole side block num
	POINT centre = { ORI_WSB / 2,ORI_WSB / 2 };
	POINT cursor = { 0,0 };
private:
	Matrix<Cell, -1, -1 >* cells;
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