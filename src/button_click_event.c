#ifndef _BUTTON_CLICK_EVENT_C_
#define _BUTTON_CLICK_EVENT_C_

#include <SDL2/SDL.h>
#include <stdio.h>
#include "attribute.c"
#include "my_lib.c"
#include "archive.c"
#include "dialog.c"

//主窗口菜单按钮鼠标点击事件
void ButtonEventClick(int _id)
{
    switch (_id)
    {
    case 0:
        //NEW
        if (opened_project && project_changed)
        {
            if (DiaLog("Save Project? 'Enter' to YES,'ESC' to NO.") == 0)
                SaveProject();
        }
        window_sub.open = 1;
        is_refresh_window = 1;
        window_sub.title.content = "New Project";
        window_sub.type = NEW;
        window_sub.buttons = button_new_prj;
        window_sub.button_count = BUTTON_NEW_PRJ_COUNT;
        window_sub.text_boxes_count = NEW_PRJ_TEXT_BOXES_COUNT;
        window_sub.text_boxes = text_boxes_new_prj;
        window_sub.input_boxes = input_box_new_prj;
        window_sub.input_boxes_count = NEW_PRJ_INPUT_BOX_COUNT;
        break;
    case 1:
        //OPEN
        {
            //读取工程信息
            if (opened_project && project_changed)
            {
                if (DiaLog("Save Project? 'Enter' to YES,'ESC' to NO.") == 0)
                    SaveProject();
            }
            int res = ReadProjectInfo();
            if (res > 0)
                break;
            window_sub.open = 1;
            is_refresh_window = 1;
            window_sub.title.content = "Open Project";
            window_sub.type = OPEN;
            window_sub.buttons = NULL;
            window_sub.button_count = 0;
            window_sub.text_boxes_count = 0;
            window_sub.text_boxes = NULL;
            window_sub.input_boxes = NULL;
            window_sub.input_boxes_count = 0;
        }
        break;
    case 2:
        window_sub.open = 0;
        is_refresh_window = 1;
        window_sub.title.content = "Save Project";
        window_sub.type = SAVE;
        window_sub.buttons = NULL;
        window_sub.button_count = 0;
        window_sub.text_boxes_count = 0;
        window_sub.text_boxes = NULL;
        window_sub.input_boxes = NULL;
        window_sub.input_boxes_count = 0;
        //保存工程
        SaveProject();
        break;
    case 3:
        window_sub.open = 0;
        is_refresh_window = 1;
        window_sub.title.content = "Export Project";
        window_sub.type = EXPORT;
        window_sub.buttons = NULL;
        window_sub.button_count = 0;
        window_sub.text_boxes_count = 0;
        window_sub.text_boxes = NULL;
        window_sub.input_boxes = NULL;
        window_sub.input_boxes_count = 0;
        ExportData();
        break;
    case 4:
        //Grid
        {
            if (canvas_map.texture)
            {
                canvas_map.show_grid++;
                canvas_map.show_grid %= 2;
                is_refresh_window = 1;
            }
        }
        break;
    case 5:
        //Light Grid
        {
            if (canvas_map.texture)
            {
                grid_color_index = 0;
                is_refresh_window = 1;
            }
        }
        break;
    case 6:
        //Dark Grid
        {
            if (canvas_map.texture)
            {
                grid_color_index = 1;
                is_refresh_window = 1;
            }
        }
        break;
    case 7:
        //Dy-Grid
        SwitchDynamicGrid();
        break;
    }
}

//主窗口Tile编辑器区按钮点击事件
void ButtonEventClickTile(int _id)
{
    switch (_id)
    {
    case 0:
        LoadTile();
        ResetBrush();
        brush.draw_collision_box = 0;
        is_refresh_window = 1;
        open_dynamic_grid = open_dynamic_grid_pre;
        break;
    case 1:
        //+Colliison Box
        if (canvas_map.texture == NULL)
        {
            StartWarning("Create Project First !");
            break;
        }
        if (c_box_lists.count >= MAX_COLLISION_BOX_KIND)
        {
            StartWarning("More then max number !");
            break;
        }

        char value[2];
        value[0] = '\0';
        //提示输入碰撞框数值
        {
            int quit = 0;
            SDL_Event event;
            int index = 0;
            while (quit == 0)
            {
                while (SDL_PollEvent(&event))
                {
                    if (event.type == SDL_KEYDOWN)
                    {
                        if (event.key.keysym.sym == SDLK_ESCAPE)
                        {
                            quit = 1;
                            value[0] = '\0';
                            break;
                        }
                        if (event.key.keysym.sym == SDLK_RETURN && value[0] != '\0')
                        {
                            quit = 1;
                            break;
                        }
                        Scanf_s(value, 2, &index, event, 0);
                        if (value[0] == '_' || value[0] == '.')
                        {
                            value[0] = '\0';
                            index = 0;
                        }
                    }
                }

                //显示提示
                SDL_Rect r;
                r.x = layout_editor[3].rect.x + 10;
                r.y = layout_editor[3].rect.y + layout_editor[3].rect.h - 30;
                r.w = 24;
                r.h = 24;

                ShowCharacter(window_main.render,
                              "Input a char (0-9 or a-z). ESC to skip.",
                              24,
                              r.x - 8,
                              r.y - 20,
                              0xff, 0, 0, 0);

                SDL_SetRenderDrawColor(window_main.render, 0, 0, 0, 0xff);

                SDL_RenderFillRect(window_main.render, &r);
                SDL_SetRenderDrawColor(window_main.render, 0xff, 0, 0, 0xff);
                SDL_RenderDrawRect(window_main.render, &r);

                //显示输入
                // r.x + 4,r.y + 14,
                ShowCharacter(window_main.render,
                              value,
                              20,
                              r.x - 1,
                              r.y + 12,
                              0xff, 0, 0, 1);

                SDL_RenderPresent(window_main.render);
                SDL_Delay(1000 / 60);
            }
        }
        if (value[0] == '\0')
        {
            StartWarning("Skip Add Collision Box.");
            is_refresh_window = 1;
            break;
        }
        int pre_draw_collision_box = brush.draw_collision_box;
        brush.draw_collision_box = 1;
        if (brush.draw_collision_box && !pre_draw_collision_box)
        {
            open_dynamic_grid_pre = open_dynamic_grid;
            open_dynamic_grid = 0;
        }
        int res = CreateCollisionBoxList(value[0]);
        if (res == -2)
        {
            StartWarning("Value already exists !");
            break;
        }
        else if (res == -4)
        {
            StartWarning("return -4");
            break;
        }

        is_refresh_window = 1;
        break;
    }
}

//子窗口两大窗口点击事件
void TwoButtonSubWinEventClick(int _id)
{
    switch (_id)
    {
    case 0:
        // OK
        {
            int res = SetInputBoxesContentToTarget();
            int _window_open = 0;
            //新建工程
            if (window_sub.type == NEW)
            {
                if (res == 0)
                    //获取工程目录
                    GetProjectPathTotal();
                else if (res == 1)
                {
                    _window_open = 1;
                    StartWarning("Please Input Project Name !");
                }
                else if (res == 2)
                {
                    _window_open = 1;
                    StartWarning("Project Already Exists !");
                }
                //检查地图宽高是否超过8000
                if (map_size.x * tile_size_tmp > MAX_CANVAS_SIZE || map_size.y * tile_size_tmp > MAX_CANVAS_SIZE)
                {
                    StartWarning("Max map size is 8000 x 8000 pixel !!");
                    _window_open = 1;
                }
                map_size.x = __max(map_size.x, 1);
                map_size.y = __max(map_size.y, 1);
                if (_window_open == 0)
                {
                    tile_size = tile_size_tmp;
                    FreeCanvas(&canvas_map);
                    FreeCanvas(&canvas_tile);
                    ResetBrush();
                    CreateMap();
                    LoadTile();
                    FreeCollisionBoxLists();
                    opened_project = 1;
                }
            }
            //打开工程
            else if (window_sub.type == OPEN)
            {
                char tmp_name[MAX_STRING_LENGTH];
                //保存目录名字
                for (int i = 0; i < MAX_STRING_LENGTH; i++)
                {
                    tmp_name[i] = prj_dir_list.dir_name_list[prj_dir_list.index][i];
                }

                char *cur_project_path_tmp = (char *)malloc(sizeof(char) * PRJ_PATH_STR_LENGTH);
                for (size_t i = 0; i < PRJ_PATH_STR_LENGTH; i++)
                {
                    cur_project_path_tmp[i] = '\0';
                }
                strcat(cur_project_path_tmp, PRJ_PATH);
                strcat(cur_project_path_tmp, tmp_name);
                //记录当前目录
                char cur_path[INT16_MAX];
                getcwd(cur_path, INT16_MAX);
                //进入工程目录
                chdir(cur_project_path_tmp);
                //查看有没有save.dat这个存档
                FILE *fp = fopen(SAVE_NAME, "rb");
                if (fp)
                {
                    for (int i = 0; i < MAX_STRING_LENGTH; i++)
                        prj_name[i] = tmp_name[i];
                    cur_project_path = (char *)malloc(sizeof(char) * PRJ_PATH_STR_LENGTH);
                    for (size_t i = 0; i < PRJ_PATH_STR_LENGTH; i++)
                        cur_project_path[i] = '\0';
                    strcat(cur_project_path, PRJ_PATH);
                    strcat(cur_project_path, prj_name);
                    chdir(cur_path);
                    LoadProject();
                    opened_project = 1;
                }
                else
                {
                    //存档不存在
                    StartWarning("Not found save.dat");
                    _window_open = 1;
                }

                //回到上一层目录
                chdir(cur_path);
                //释放内存
                free(fp);
                fp = NULL;
                free(cur_project_path_tmp);
                cur_project_path_tmp = NULL;
            }
            window_sub.open = _window_open;
        }
        break;
    case 1:
        window_sub.open = 0;
        // CANCEL
        break;
    }
}

#endif