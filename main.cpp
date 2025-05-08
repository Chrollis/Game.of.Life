#include "game.h"

int __stdcall WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd) {
    Game game;
    ExMessage msg;
    clock_t last_tick = clock();
    clock_t current_tick = clock();

    initgraph(GRAPHSIZE, GRAPHSIZE, EX_NOCLOSE);
    game.enter();
    game.fps_controller.set(FPS);

    BeginBatchDraw();

    while (1) {
        game.fps_controller.start();

        while (peekmessage(&msg)) {
            game.input(msg);
        }

        current_tick = clock();
        game.proceed(current_tick - last_tick);
        last_tick = current_tick;

        cleardevice();
        game.draw();
        game.fps_controller.draw();

        FlushBatchDraw();
        game.fps_controller.delay();
    }

    EndBatchDraw();
    closegraph();
    return 0;
}
