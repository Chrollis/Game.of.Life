#pragma once
#include <graphics.h>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include "C:/Libraries/eigen-3.4.0/Eigen/Dense"

using namespace std;
using Eigen::Matrix;
using Eigen::Vector;
using Eigen::Block;

#define BLOCKCELL 16
#define WHOLECELL 80
#define ORISCALE 50
#define GRAPHSIZE 800

wstring string_to_wstring(const string& str);
#define string_to_lpcwstr(x) string_to_wstring(x).c_str()
bool clear_directory(LPCWSTR path);

#define FPS 24

class FpsController {
private:
	int interval = 83;
	int fps_target = 12;
	clock_t begin = 0;
	clock_t end = 0;
	int actual_fps = 0;
public:
	FpsController() = default;
	~FpsController() = default;

	void start();
	void delay()const;
	void set(const int fps);
	void draw();
	friend class Game;
};

