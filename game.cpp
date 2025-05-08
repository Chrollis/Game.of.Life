#include "game.h"

void Game::enter() {
	cells = Matrix<Cell, WHOLECELL, WHOLECELL>(WHOLECELL, WHOLECELL);
	CreateDirectoryA(".\\cache", nullptr);
}
void Game::proceed(clock_t delta) {
	static clock_t compute_timer = 0;
	static clock_t move_timer = 0;
	if (running) {
		if (compute_timer >= compute_interval) {
			Matrix<Cell, WHOLECELL, WHOLECELL> copies = cells;
			for (int row = 1; row < WHOLECELL - 1; row++) {
				for (int col = 1; col < WHOLECELL - 1; col++) {
					if (cells(row, col).active) {
						if (cells(row, col).survive(copies.block<3, 3>(row - 1, col - 1))) {
							for (int r = 0; r < 5; r++) {
								for (int c = 0; c < 5; c++) {
									if (row - 2 + r >= 0 && row - 2 + r < WHOLECELL) {
										if (col - 2 + c >= 0 && col - 2 + c < WHOLECELL) {
											cells(row - 2 + r, col - 2 + c).active = 1;
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
	if (move_timer >= 42) {
		zoom();
		camera_move();
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
			zooming[LARGE] = 1;
			break;
		case 'K':
		case VK_OEM_MINUS:
			zooming[SMALL] = 1;
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
			zooming[LARGE] = 0;
			break;
		case 'K':
		case VK_OEM_MINUS:
			zooming[SMALL] = 0;
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
	int visible = GRAPHSIZE / scale;
	int x = camera.x / scale * scale - camera.x ;
	int y = camera.y / scale * scale - camera.y ;
	for (int row = -1; row <= visible; row++) {
		for (int col = -1; col <= visible; col++) {
			if (cells(camera.y / scale + BLOCKCELL * 2+ row,
				camera.x  / scale + BLOCKCELL * 2  + col).alive) {
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
	circle(GRAPHSIZE / 2, GRAPHSIZE / 2, ORISCALE / 10);
	solidcircle(GRAPHSIZE / 2, GRAPHSIZE / 2, ORISCALE / 25);
	ostringstream oss;
	oss << "Camera(" << camera.x << ", " << camera.y << ")";
	settextcolor(LIGHTGRAY);
	settextstyle(ORISCALE / 2, 0, L"Times New Roman", 0, 0, 0, 1, 0, 0);
	setbkmode(TRANSPARENT);
	outtextxy(ORISCALE / 10, ORISCALE / 10, string_to_lpcwstr(oss.str()));
	if (ctrl) {
		oss.str("");
		oss << "Block(" << (camera.x + cursor.x + BLOCKCELL * centre.x * scale) / scale
			<< ", " << (camera.y + cursor.y + BLOCKCELL * centre.y * scale) / scale << ")";
		outtextxy(ORISCALE / 10, GRAPHSIZE - ORISCALE / 10 * 11, string_to_lpcwstr(oss.str()));
	}
}
void Game::quit() {
	clear_directory(L".\\cache");
}
void Game::zoom() {
	int ds = (zooming[LARGE] - zooming[SMALL]) * 5;

	if (scale + ds >= 25) {
		camera.x = ((scale + ds) * camera.x + GRAPHSIZE / 2 * ds) / scale;
		camera.y = ((scale + ds) * camera.y + GRAPHSIZE / 2 * ds) / scale;
		scale += ds;
	}
}
void Game::camera_move() {
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

	if (abs(camera.x + dx) <= BLOCKCELL * scale) {
		camera.x += dx;
	}
	if (abs(camera.y + dy) <= BLOCKCELL * scale) {
		camera.y += dy;
	}
	if (abs(camera.x + camera.y) + abs(camera.x - camera.y) >= 2 * (BLOCKCELL - 2) * scale) {
		int x = (camera.x + BLOCKCELL * scale) / (BLOCKCELL * scale * 2 / 3) - 1;
		int y = (camera.y + BLOCKCELL * scale) / (BLOCKCELL * scale * 2 / 3) - 1;
		centre_move(x, y);
	}
}
void Game::centre_move(int x, int y) {
	if (x != 0) {
		for (int i = 0; i < 5; i++) {
			Cell::unload_block(cells.block<BLOCKCELL, BLOCKCELL>(i * BLOCKCELL, -2 * (x - 1) * BLOCKCELL),
				centre.x - 2 * x, centre.y + i - 2);
		}
		Matrix<Cell, 5 * BLOCKCELL, 4 * BLOCKCELL> copies = 
			cells.block<5 * BLOCKCELL, 4 * BLOCKCELL>(0, BLOCKCELL * (1 + x) / 2);
		cells.block<5 * BLOCKCELL, 4 * BLOCKCELL>(0, BLOCKCELL * (1 - x) / 2) = copies;
		centre.x += x;
		camera.x -= x * BLOCKCELL * scale;
		for (int i = 0; i < 5; i++) {
			Cell::load_block(cells.block<BLOCKCELL, BLOCKCELL>(i * BLOCKCELL, 2 * (x + 1) * BLOCKCELL),
				centre.x + 2 * x, centre.y + i - 2);
		}
	}
	if (y != 0) {
		for (int i = 0; i < 5; i++) {
			Cell::unload_block(cells.block<BLOCKCELL, BLOCKCELL>(-2 * (y - 1) * BLOCKCELL, i * BLOCKCELL),
				centre.x + i - 2, centre.y - 2 * y);
		}
		Matrix<Cell, 4 * BLOCKCELL, 5 * BLOCKCELL> copies =
			cells.block<4 * BLOCKCELL, 5 * BLOCKCELL>(BLOCKCELL * (1 + y) / 2, 0);
		cells.block<4 * BLOCKCELL, 5 * BLOCKCELL>(BLOCKCELL * (1 - y) / 2, 0) = copies;
		centre.y += y;
		camera.y -= y * BLOCKCELL * scale;
		for (int i = 0; i < 5; i++) {
			 Cell::load_block(cells.block<BLOCKCELL, BLOCKCELL>(2 * (y + 1) * BLOCKCELL, i * BLOCKCELL),
				 centre.x + i - 2, centre.y + 2 * y);
		}
	}
}
void Game::cell_click(bool alive) {
	int x = (camera.x + cursor.x + BLOCKCELL * 2 * scale) / scale;
	int y = (camera.y + cursor.y + BLOCKCELL * 2 * scale) / scale;
	cells(y, x).alive = alive;
	for (int r = 0; r < 3; r++) {
		for (int c = 0; c < 3; c++) {
			if (y - 1 + r >= 0 && y - 1 + r < WHOLECELL) {
				if (x - 1 + c >= 0 && x - 1 + c < WHOLECELL) {
					cells(y - 1 + r, x - 1 + c).active = 1;
				}
			}
		}
	}
}