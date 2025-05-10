#include "utils.h"

wstring string_to_wstring(const string& str) {
	if (str.empty()) {
		return L"";
	}
	int size_needed = MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)str.size(), nullptr, 0);
	if (size_needed == 0) {
		return L"";
	}
	wstring wstr(size_needed, 0);
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)str.size(), &wstr[0], size_needed);
	return wstr;
}
bool clear_directory(LPCWSTR path) {
    WIN32_FIND_DATAW findData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    wstring searchPath = path + wstring(L"\\*");

    hFind = FindFirstFileW(searchPath.c_str(), &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        return false;
    }

    do {
        if (wcscmp(findData.cFileName, L".") == 0 ||
            wcscmp(findData.cFileName, L"..") == 0) {
            continue;
        }
        wstring fullPath = path + wstring(L"\\") + findData.cFileName;
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (!clear_directory(fullPath.c_str())) {
                FindClose(hFind);
                return false;
            }
            if (!RemoveDirectoryW(fullPath.c_str())) {
                FindClose(hFind);
                return false;
            }
        }
        else {
            if (!DeleteFileW(fullPath.c_str())) {
                FindClose(hFind);
                return false;
            }
        }
    } while (FindNextFileW(hFind, &findData) != 0);
    FindClose(hFind);
    return 1;
}

void FpsController::start() {
    begin = clock();
}
void FpsController::delay()const {
    Sleep((DWORD)(max(0, interval + clock() - begin)));
}
void FpsController::set(const int fps) {
    interval = 500 / fps;
    fps_target = fps;
}
void FpsController::draw() {
    static int counter = 0;
    if (counter == 0) {
        actual_fps = 1000 * fps_target / (clock() - end);
        end = clock();
    }
    counter = (++counter) % fps_target;
    std::ostringstream oss;
    oss << "Fps = " << actual_fps << " : " << fps_target;
    settextcolor(LIGHTGRAY);
    settextstyle(ORI_SCALE / 2, 0, L"Times New Roman", 0, 0, 0, 1, 0, 0);
    setbkmode(TRANSPARENT);
    outtextxy(ORI_SCALE / 10, GRAPH_SIZE - ORI_SCALE / 10 * 6, string_to_lpcwstr(oss.str()));
}