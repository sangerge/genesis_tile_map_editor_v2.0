#ifndef _QUIT_C_
#define _QUIT_C_

#include <SDL2/SDL.h>
#include <stdio.h>
#include "attribute.c"
#include "collision_box.c"

void Quit()
{
    if (opened_project && project_changed)
    {
        if (DiaLog("Save Project? 'Enter' to YES,'ESC' to NO.") == 0)
            SaveProject();
    }
    free(warning);
    free(cur_project_path);
    free(pixels);
    SDL_FreePalette(pal);
    FreeCollisionBoxLists();
    SDL_DestroyTexture(canvas_map.texture);
    SDL_DestroyTexture(canvas_tile.texture);
    SDL_DestroyRenderer(window_main.render);
    SDL_DestroyWindow(window_main.window);
    SDL_Quit();
}

#endif