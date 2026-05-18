#include "Game.h"
#ifdef _WIN32
    #include <windows.h>
#endif
#include <iostream>
#include <locale>
#include <string>
// the mistake i did is we should not use runUCI(int depth,int timelimit);
// uci does take those commands 
// UCI function declaration
void runUCI();
int main(int argc, char* argv[]) {
#ifdef _WIN32
    // to print chess piece in powershell
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    try {
        std::locale::global(std::locale(""));
    }
    catch (...) {}
    // IMPORTANT:
    // If engine is started in UCI mode, do NOT show menu.
    // Do NOT ask for depth/time using cin.
    // these things are interfering with uci
    // 
    if (argc > 1 && std::string(argv[1])=="uci") {
        runUCI();
        return 0;
    }
    // Normal terminal chess mode
    Game game;
    game.run();
    return 0;
}
