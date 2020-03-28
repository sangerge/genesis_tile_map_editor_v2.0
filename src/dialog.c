#ifndef _DIALOC_C_
#define _DIALOC_C_

#include <SDL2/SDL.h>
#include "mytool.c"

//对话框，返回0确定，-1取消，回车键确定，esc键取消
int DiaLog(char *_str)
{
    {
        ShowCharacter(window_main.render,
                      _str,
                      24,
                      layout_editor[3].rect.x + 10,
                      layout_editor[3].rect.y + layout_editor[3].rect.h * 0.5,
                      255, 0, 0, 0);
        SDL_RenderPresent(window_main.render);
    }
    int res = 0;
    int quit = 0;
    SDL_Event event;
    while (quit == 0)
    {
        int pre_res = res;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_RETURN:
                    res = 0;
                    quit = 1;
                    break;
                case SDLK_ESCAPE:
                    res = -1;
                    quit = 1;
                    break;
                }
            }
        }
        if (pre_res != res)
            SDL_RenderPresent(window_main.render);
        SDL_Delay(1000 / FPS);
    }
    return res;
}

#endif