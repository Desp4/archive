#ifdef _WIN32
#include <Windows.h>
#endif

#include "ppaf/player.hpp"

int main(int argc, char** argv)
{
    if (argc != 2)
        return 0;

#ifdef _WIN32
    int numArgs = 0;
    LPWSTR* argvw = CommandLineToArgvW(GetCommandLineW(), &numArgs);

    int charSize = WideCharToMultiByte(CP_UTF8, 0, argvw[1], -1, NULL, 0, NULL, NULL);
    char* filepath = new char[charSize];
    WideCharToMultiByte(CP_UTF8, 0, argvw[1], -1, filepath, charSize, NULL, NULL);
    LocalFree(argvw);
#else
    char* filepath = argv[1];
#endif

    AudioPlayer player;
    player.setVolume(0.25f);
    player.open(filepath);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    player.seek(7);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    player.close();
    
#ifdef _WIN32
    delete[] filepath;
#endif
    return 1;
}