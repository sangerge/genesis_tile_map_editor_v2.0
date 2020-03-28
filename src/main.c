#include <SDL2/SDL.h>
#include <SDL2/SDL_test.h>
#include <stdio.h>
#include "attribute.c"
#include "init.c"
#include "update.c"
#include "quit.c"

int main(int argc, char **argv)
{
    Init();
    Update();
    Quit();
    return 0;
}