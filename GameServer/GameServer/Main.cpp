#include <iostream>
#include <string>
#include "Game.h"

int main(int argc, char** argv)
{
    const uint16_t server_port = 7171;
    std::cout << "Starting gameserver on port " << server_port << std::endl;
    Game game(server_port);
    game.Start();

    std::string input;
    while (game.IsRunning())
    {
        std::getline(std::cin, input);

        if (input == "/shutdown")
        {
            game.Shutdown();
        }
    }
    return EXIT_SUCCESS;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
