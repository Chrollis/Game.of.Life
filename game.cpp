#include "game.h"

void Game::enter() {
	cells = new Matrix<Cell, -1, -1>(wsb * BSC , wsb * BSC);
	CreateDirectoryA(".\\cache", nullptr);
}
void Game::proceed(clock_t delta) {
	static clock_t compute_timer = 0;
	static clock_t move_timer = 0;
	static clock_t expand_timer = 0;
	if (running) {
		if (compute_timer >= compute_interval) {
			Matrix<Cell, -1 , -1 > copies = *cells;
			for (int row = 1; row < wsb * BSC  - 1; row++) {
				for (int col = 1; col < wsb * BSC  - 1; col++) {
					if ((*cells)(row, col).active) {
						if ((*cells)(row, col).survive(copies.block<3, 3>(row - 1, col - 1))) {
							for (int r = 0; r < 5; r++) {
								for (int c = 0; c < 5; c++) {
									if (row - 2 + r >= 0 && row - 2 + r < wsb * BSC ) {
										if (col - 2 + c >= 0 && col - 2 + c < wsb * BSC ) {
											(*cells)(row - 2 + r, col - 2 + c).active = 1;
										}
									}
								}
							}
						}
					}
				}
			}
			compute_timer = 0;
		}
		compute_timer += delta;
	}
	if (expanding == LARGE || expanding == SMALL) {
		if (expand_timer >= 504) {
			expand(expanding);
			while (camera_move());
			expanding = -1;
			expand_timer = 0;
		}
		expand_timer += delta;
	}
	if (move_timer >= 42) {
		zoom();
		while (camera_move());
		move_timer = 0;
	}
	move_timer += delta;
	
}
void Game::input(const ExMessage& msg) {
	switch (msg.message) {
	case WM_KEYDOWN:
		switch (msg.vkcode) {
		case 'A':
		case VK_LEFT:
			moving[LEFT] = 1;
			break;
		case 'W':
		case VK_UP:
			moving[UP] = 1;
			break;
		case 'D':
		case VK_RIGHT:
			moving[RIGHT] = 1;
			break;
		case 'S':
		case VK_DOWN:
			moving[DOWN] = 1;
			break;
		case 'J':
		case VK_OEM_4:
			if (compute_interval == fps_controller.interval) {
				compute_interval = 75;
			}
			else if (compute_interval < 2400) {
				compute_interval *= 2;
			}
			break;
		case 'L':
		case VK_OEM_6:
			if (compute_interval > 75) {
				compute_interval /= 2;
			}
			else {
				compute_interval = fps_controller.interval;
			}
			break;
		case 'I':
		case VK_OEM_PLUS:
			if (ctrl) {
				expanding = LARGE;
			}
			else {
			zooming[LARGE] = 1;
			}
			break;
		case 'K':
		case VK_OEM_MINUS:
			if (ctrl) {
				expanding = SMALL;
			}
			else {
			zooming[SMALL] = 1;
			}
			break;
		case VK_SPACE:
			running = !running;
			break;
		case VK_SHIFT:
			shift = 1;
			break;
		case VK_CONTROL:
			ctrl = 1;
			break;
		case VK_ESCAPE:
			quit();
			exit(0);
			break;
		default:
			break;
		}
		break;
	case WM_KEYUP:
		switch (msg.vkcode) {
		case 'A':
		case VK_LEFT:
			moving[LEFT] = 0;
			break;
		case 'W':
		case VK_UP:
			moving[UP] = 0;
			break;
		case 'D':
		case VK_RIGHT:
			moving[RIGHT] = 0;
			break;
		case 'S':
		case VK_DOWN:
			moving[DOWN] = 0;
			break;
		case 'I':
		case VK_OEM_PLUS:
			if (ctrl) {
				expanding = -1;
			}
			else {
				zooming[LARGE] = 0;
			}
			break;
		case 'K':
		case VK_OEM_MINUS:
			if (ctrl) {
				expanding = -1;
			}
			else {
				zooming[SMALL] = 0;
			}
			break;
		case VK_SHIFT:
			shift = 0;
			break;
		case VK_CONTROL:
			ctrl = 0;
			break;
		default:
			break;
		}
		break;
	case WM_MOUSEMOVE:
		cursor.x = msg.x;
		cursor.y = msg.y;
		break;
	case WM_LBUTTONDOWN:
		cell_click(1);
		break;
	case WM_RBUTTONDOWN:
		cell_click(0);
		break;
	default:
		break;
	}
}
void Game::draw() {
	int visible = GRAPH_SIZE / scale;
	int x = (camera.x + BSC * (wsb / 2) * scale) / scale * scale - camera.x - BSC * (wsb / 2) * scale;
	int y = (camera.y + BSC * (wsb / 2) * scale) / scale * scale - camera.y - BSC * (wsb / 2) * scale;
	for (int row = -1; row <= visible; row++) {
		for (int col = -1; col <= visible; col++) {
			if ((*cells)((camera.y + BSC * (wsb / 2) * scale) / scale + row,
				(camera.x + BSC * (wsb / 2) * scale) / scale + col).alive) {
				setfillcolor(WHITE);
				fillrectangle(x + col * scale, y + row * scale,
					x + col * scale + scale, y + row * scale + scale);
			}
			setlinecolor(DARKGRAY);
			rectangle(x + col * scale, y + row * scale,
				x + col * scale + scale, y + row * scale + scale);
		}
	}
	setlinecolor(LIGHTGRAY);
	setfillcolor(LIGHTGRAY);
	circle(GRAPH_SIZE / 2, GRAPH_SIZE / 2, ORI_SCALE / 10);
	solidcircle(GRAPH_SIZE / 2, GRAPH_SIZE / 2, ORI_SCALE / 25);
	settextcolor(LIGHTGRAY);
	settextstyle(ORI_SCALE / 2, 0, L"Times New Roman", 0, 0, 0, 1, 0, 0);
	setbkmode(TRANSPARENT);
	ostringstream oss;
	oss << "Camera(" << camera.x << ", " << camera.y << ")";
	outtextxy(ORI_SCALE / 10, ORI_SCALE / 10, string_to_lpcwstr(oss.str()));
	if (running) {
		outtextxy(ORI_SCALE / 10, ORI_SCALE / 10 * 6, L"The Game is Running");
	}
	else {
		outtextxy(ORI_SCALE / 10, ORI_SCALE / 10 * 6, L"The Game has Paused");
	}
	oss.str("");
	oss << "Side length = " << BSC * wsb << " cells";
	outtextxy(ORI_SCALE / 10, ORI_SCALE / 10 * 11, string_to_lpcwstr(oss.str()));
	oss.str("");
	oss << "Scale = " << scale << " pixel per cell";
	outtextxy(ORI_SCALE / 10, ORI_SCALE / 10 * 16, string_to_lpcwstr(oss.str()));
	oss.str("");
	oss << "Focus on (" << (camera.x + GRAPH_SIZE / 2 + BSC * centre.x * scale) / scale
		<< ", " << (camera.y + GRAPH_SIZE / 2 + BSC * centre.y * scale) / scale << ")";
	outtextxy(ORI_SCALE / 10, GRAPH_SIZE - ORI_SCALE / 10 * 11, string_to_lpcwstr(oss.str()));
	oss.str("");
	oss << "Cursor on (" << (camera.x + cursor.x + BSC * centre.x * scale) / scale
		<< ", " << (camera.y + cursor.y + BSC * centre.y * scale) / scale << ")";
	outtextxy(ORI_SCALE / 10, GRAPH_SIZE - ORI_SCALE / 10 * 16, string_to_lpcwstr(oss.str()));
}
void Game::quit() {
	clear_directory(L".\\cache");
	delete cells;
}
void Game::zoom() {
	int ds = zooming[LARGE] - zooming[SMALL];

	if (scale + ds > (int)(double(GRAPH_SIZE) / ((wsb - 3) * BSC)) + 4) {
		camera.x = ((scale + ds) * camera.x + GRAPH_SIZE / 2 * ds) / scale;
		camera.y = ((scale + ds) * camera.y + GRAPH_SIZE / 2 * ds) / scale;
		scale += ds;
	}
}
void Game::expand(int s) {
	if (s == LARGE) {
		if (wsb + 2 < 17) {
			Matrix<Cell, -1, -1> copies = *cells;
			delete cells;
			wsb += 2;
			cells = new Matrix<Cell, -1, -1>(wsb * BSC, wsb * BSC);
			cells->block(BSC, BSC, (wsb - 2) * BSC, (wsb - 2) * BSC) = copies;
			for (int i = 0; i < wsb; i++) {
				Cell::load_block(cells->block<BSC, BSC>(i * BSC, 0), centre.x - wsb / 2, centre.y + i - wsb / 2);
				Cell::load_block(cells->block<BSC, BSC>(i * BSC, (wsb - 1) * BSC), centre.x + wsb / 2, centre.y + i - wsb / 2);
			}
			for (int i = 1; i < wsb - 1; i++) {
				Cell::load_block(cells->block<BSC, BSC>(0, i * BSC), centre.x + i - wsb / 2, centre.y - wsb / 2);
				Cell::load_block(cells->block<BSC, BSC>((wsb - 1) * BSC, i * BSC), centre.x + i - wsb / 2, centre.y + wsb / 2);
			}
		}
	}
	else if (s == SMALL) {
		if (wsb - 2 > 4) {
			if (scale >= (int)(double(GRAPH_SIZE) / ((wsb - 5) * BSC)) + 4) {
				Matrix<Cell, -1, -1> copies = *cells;
				delete cells;
				wsb -= 2;
				cells = new Matrix<Cell, -1, -1>(wsb * BSC, wsb * BSC);
				*cells = copies.block(BSC, BSC, wsb * BSC, wsb * BSC);
				for (int i = 0; i < wsb + 2; i++) {
					Cell::unload_block(copies.block<BSC, BSC>(i * BSC, 0), centre.x - (wsb + 2) / 2, centre.y + i - (wsb + 2) / 2);
					Cell::unload_block(copies.block<BSC, BSC>(i * BSC, (wsb + 1) * BSC), centre.x + (wsb + 2) / 2, centre.y + i - (wsb + 2) / 2);
				}
				for (int i = 1; i < wsb + 1; i++) {
					Cell::unload_block(copies.block<BSC, BSC>(0, i * BSC), centre.x + i - (wsb + 2) / 2, centre.y - (wsb + 2) / 2);
					Cell::unload_block(copies.block<BSC, BSC>((wsb + 1) * BSC, i * BSC), centre.x + i - (wsb + 2) / 2, centre.y + (wsb + 2) / 2);
				}
			}
			else {
				int ds = (int)(double(GRAPH_SIZE) / ((wsb - 5) * BSC)) + 4 - scale;
				camera.x = ((scale + ds) * camera.x + GRAPH_SIZE / 2 * ds) / scale;
				camera.y = ((scale + ds) * camera.y + GRAPH_SIZE / 2 * ds) / scale;
				scale += ds;
			}
		}
	}
}
bool Game::camera_move() {
	int dx = moving[RIGHT] - moving[LEFT];
	int dy = moving[DOWN] - moving[UP];
	double dl = sqrt(dx * dx + dy * dy);

	if (shift && camera.speed < 18) {
		camera.speed += 2;
	}
	if (!shift && camera.speed > 2) {
		camera.speed -= 2;
	}

	dx = (int)(camera.speed * dx * dl);
	dy = (int)(camera.speed * dy * dl);

	if (abs(camera.x + dx) <= BSC * scale) {
		camera.x += dx;
	}
	if (abs(camera.y + dy) <= BSC * scale) {
		camera.y += dy;
	}
	if (abs(camera.x + camera.y) + abs(camera.x - camera.y) >= 2 * (BSC - 2) * scale) {
		int x = (camera.x + BSC * scale) / (BSC * scale * 2 / 3) - 1;
		int y = (camera.y + BSC * scale) / (BSC * scale * 2 / 3) - 1;
		centre_move(x, y);
		return 1;
	}
	return 0;
}
void Game::centre_move(int x, int y) {
	if (x != 0) {
		for (int i = 0; i < wsb; i++) {
			Cell::unload_block(cells->block<BSC, BSC>(i * BSC, -(wsb / 2) * (x - 1) * BSC),
				centre.x - (wsb / 2) * x, centre.y + i - wsb / 2);
		}
		Matrix<Cell, -1, -1> copies =
			cells->block(0, BSC * (1 + x) / 2, wsb * BSC, (wsb - 1) * BSC);
		cells->block(0, BSC * (1 - x) / 2, wsb * BSC, (wsb - 1) * BSC) = copies;
		centre.x += x;
		camera.x -= x * BSC * scale;
		for (int i = 0; i < wsb; i++) {
			Cell::load_block(cells->block<BSC, BSC>(i * BSC, (wsb / 2) * (x + 1) * BSC),
				centre.x + (wsb / 2) * x, centre.y + i - wsb / 2);
		}
	}
	if (y != 0) {
		for (int i = 0; i < wsb; i++) {
			Cell::unload_block(cells->block<BSC, BSC>(-(wsb / 2) * (y - 1) * BSC, i * BSC),
				centre.x + i - wsb / 2, centre.y - (wsb / 2) * y);
		}
		Matrix<Cell, -1, -1> copies =
			cells->block(BSC * (1 + y) / 2, 0, (wsb - 1) * BSC, wsb * BSC);
		cells->block(BSC * (1 - y) / 2, 0, (wsb - 1) * BSC, wsb * BSC) = copies;
		centre.y += y;
		camera.y -= y * BSC * scale;
		for (int i = 0; i < wsb; i++) {
			 Cell::load_block(cells->block<BSC, BSC>((wsb / 2) * (y + 1) * BSC, i * BSC),
				 centre.x + i - wsb / 2, centre.y + (wsb / 2) * y);
		}
	}
}
void Game::cell_click(bool alive) {
	int x = (camera.x + cursor.x + BSC * (wsb / 2) * scale) / scale;
	int y = (camera.y + cursor.y + BSC * (wsb / 2) * scale) / scale;
	(*cells)(y, x).alive = alive;
	for (int r = 0; r < 3; r++) {
		for (int c = 0; c < 3; c++) {
			if (y - 1 + r >= 0 && y - 1 + r < wsb * BSC ) {
				if (x - 1 + c >= 0 && x - 1 + c < wsb * BSC ) {
					(*cells)(y - 1 + r, x - 1 + c).active = 1;
				}
			}
		}
	}
}