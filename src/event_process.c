#ifndef _EVENT_PROCESS_C_
#define _EVENT_PROCESS_C_

#include "attribute.c"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <dirent.h>
#include "my_io.c"
#include "button_click_event.c"
#include "collision_box.c"
#include "mytool.c"

static void CanGrabLayout();
static void UpdateInputBoxesEvent();
static void UpdateProjectListEvent(int _mouse_x, int _mouse_y);
static void UpdateNewPrjButtonEvent();

//主窗口事件
void EventProcess(Window_Struct *_win_struct)
{
    while (SDL_PollEvent(&_win_struct->event))
    {
        switch (_win_struct->event.type)
        {
        case SDL_QUIT:
            _win_struct->quit = SDL_TRUE;
            break;
        case SDL_MOUSEBUTTONDOWN:
        {
            //鼠标点击事件，左键
            if (_win_struct->event.button.button == 1)
            {
                //拖拽布局
                CanGrabLayout();
                //菜单按钮点击
                for (size_t i = 0; i < BUTTON_MENU_COUNT; i++)
                {
                    if (button_menu[i].activation)
                    {
                        ButtonEventClick(button_menu[i].id);
                        break;
                    }
                }
                //画笔模式按钮点击事件
                for (size_t i = 0; i < 2; i++)
                {
                    if (brush_mode_button[i].activation)
                    {
                        int pre_draw_collision_box = brush.draw_collision_box;
                        brush.draw_collision_box = brush_mode_button[i].id;
                        if (brush.draw_collision_box && !pre_draw_collision_box)
                        {
                            open_dynamic_grid_pre = open_dynamic_grid;
                            open_dynamic_grid = 0;
                        }
                        else if (!brush.draw_collision_box && pre_draw_collision_box)
                            open_dynamic_grid = open_dynamic_grid_pre;
                        break;
                    }
                }

                //Tile编辑区按钮点击
                for (size_t i = 0; i < 2; i++)
                {
                    if (button_tile[i].activation)
                    {
                        ButtonEventClickTile(button_tile[i].id);
                        break;
                    }
                }
                //没有拖拽左右布局
                if (can_grab_layout == 0)
                {
                    switch (layoutIndex)
                    {
                    case 2:
                        if (brush.draw_collision_box == 0)
                        {
                            if (canvas_tile.texture == NULL)
                            {
                                StartWarning("Load Tile First !");
                                break;
                            }
                            //选择tile块
                            BrushGetStartPos();
                            BrushGetEndPos();
                        }
                        else
                        {
                            if (c_box_lists.count == 0)
                            {
                                StartWarning("Add Colliison Box First !");
                                break;
                            }
                            //选择碰撞盒列表的index
                            SelectCollisionBoxListsIndex();
                        }
                        break;
                    case 3:
                    {
                        int _break = 0;
                        for (size_t i = 0; i < 2; i++)
                        {
                            //鼠标在画笔模式按钮内退出
                            if (IsPointInRect(mouse_position_cur_local.x, mouse_position_cur_local.y, brush_mode_button[i].rect))
                                _break = 1;
                        }
                        if (_break)
                            break;
                        //绘制模式
                        if (brush.draw_collision_box == 0)
                        {
                            if (canvas_tile.texture == NULL)
                            {
                                StartWarning("Load Tile First !");
                                break;
                            }
                            //绘制map
                            brush.startPaste = 1;
                            is_refresh_window = 1;
                        }
                        //碰撞框模式
                        else if (brush.draw_collision_box == 1)
                        {
                            if (c_box_lists.count == 0)
                            {
                                StartWarning("Add Colliison Box First !");
                                break;
                            }
                            BrushGetStartPosInMap();
                            BrushGetEndPosInMap();
                        }
                    }
                    break;
                    }
                }
            }
            //右键点击
            else if (_win_struct->event.button.button == 3)
            {
                grabCanvas = NULL;

                //画布重置位置
                if (layoutIndex == 3)
                    ResetCanvasPosition();
                else if (layoutIndex == 2)
                {
                    if (brush.draw_collision_box)
                    {
                        //单独显示当前索引的碰撞盒列表
                        SoloSelectedCollisionBoxLists();
                        is_refresh_window = 1;
                        break;
                    }
                    else
                        ResetCanvasPosition();
                }
            }
            //鼠标中键点击事件
            else if (_win_struct->event.button.button == 2)
            {
                if (can_grab_layout == 0)
                {
                    switch (layoutIndex)
                    {
                    case 2:
                        if (brush.draw_collision_box == 0)
                        {
                            grabCanvas = &canvas_tile;
                            GetMouseToCanvasOffset();
                        }
                        else
                        {
                            //删除碰撞盒列表
                            DestroySelectedCollisionBoxLists();
                            is_refresh_window = 1;
                        }
                        break;
                    case 3:
                        grabCanvas = &canvas_map;
                        GetMouseToCanvasOffset();
                        break;
                    }
                }
            }
        }
        break;
        //鼠标滚轮事件
        case SDL_MOUSEWHEEL:
        {
            if (layoutIndex == 2 && canvas_tile.texture)
            {
                if (brush.draw_collision_box == 1)
                    return;
                if (_win_struct->event.wheel.y > 0 && *canvas_tile.scale < scale_data[3])
                {
                    canvas_tile.scale++;
                    canvas_tile.rect.w = canvas_tile.h_count * tile_size * *canvas_tile.scale;
                    canvas_tile.rect.h = canvas_tile.v_count * tile_size * *canvas_tile.scale;
                    is_refresh_window = 1;
                }
                else if (_win_struct->event.wheel.y < 0 && *canvas_tile.scale > scale_data[0])
                {
                    canvas_tile.scale--;
                    canvas_tile.rect.w = canvas_tile.h_count * tile_size * *canvas_tile.scale;
                    canvas_tile.rect.h = canvas_tile.v_count * tile_size * *canvas_tile.scale;
                    is_refresh_window = 1;
                }
            }
            else if (layoutIndex == 3 && canvas_map.texture)
            {
                if (_win_struct->event.wheel.y > 0 && *canvas_map.scale < scale_data[3])
                {
                    canvas_map.scale++;
                    canvas_map.rect.w = canvas_map.h_count * tile_size * *canvas_map.scale;
                    canvas_map.rect.h = canvas_map.v_count * tile_size * *canvas_map.scale;
                    is_refresh_window = 1;
                }
                else if (_win_struct->event.wheel.y < 0 && *canvas_map.scale > scale_data[0])
                {
                    canvas_map.scale--;
                    canvas_map.rect.w = canvas_map.h_count * tile_size * *canvas_map.scale;
                    canvas_map.rect.h = canvas_map.v_count * tile_size * *canvas_map.scale;
                    is_refresh_window = 1;
                }
            }
        }
        break;
            //鼠标抬起事件
        case SDL_MOUSEBUTTONUP:
        {
            can_grab_layout = 0;
            //释放抓住的画布
            grabCanvas = NULL;
            //取消tile画笔拷贝
            if (brush.startCopy == 1)
            {
                BrushGetEndPos();
                brush.startCopy = 0;
                is_refresh_window = 1;
            }
            //抬起左键，结束绘制map
            if (_win_struct->event.button.button == 1)
            {
                brush.startPaste = 0;
                is_refresh_window = 1;
            }
            //取消map画笔拷贝
            if (brush.startCopyInMap == 1)
            {
                BrushGetEndPosInMap();
                AddACollisionBox(c_box_lists.lists[c_box_lists.index],
                                 brush.copyRectInMap.x,
                                 brush.copyRectInMap.y,
                                 brush.copyRectInMap.w,
                                 brush.copyRectInMap.h);
                brush.startCopyInMap = 0;
                is_refresh_window = 1;
            }
        }
        break;
        //键盘按下事件
        case SDL_KEYDOWN:
        {
            //按下G键打开关闭网格显示
            if (_win_struct->event.key.keysym.sym == SDLK_g)
            {
                if (layoutIndex == 2 && canvas_tile.texture)
                {
                    if (brush.draw_collision_box == 0)
                    {
                        canvas_tile.show_grid++;
                        canvas_tile.show_grid %= 2;
                        is_refresh_window = 1;
                    }
                }
                else if (layoutIndex == 3 && canvas_map.texture)
                {
                    canvas_map.show_grid++;
                    canvas_map.show_grid %= 2;
                    is_refresh_window = 1;
                }
            }
            //按下D键打开关闭动态网格
            if (_win_struct->event.key.keysym.sym == SDLK_d)
                SwitchDynamicGrid();
            //按下Backspace键删除索引碰撞框
            if (brush.draw_collision_box &&
                _win_struct->event.key.keysym.sym == SDLK_BACKSPACE)
            {
                DelACollisionBox(c_box_lists.lists[c_box_lists.index], c_box_lists.lists[c_box_lists.index]->index);
                is_refresh_window = 1;
            }
        }
        break;
        }
    }
}

//子窗口事件
void EventProcessSubWin(Sub_Window *_sub_win)
{
    while (SDL_PollEvent(&_sub_win->event))
    {
        switch (_sub_win->event.type)
        {
        case SDL_MOUSEBUTTONDOWN:
        {
            //左键被点击
            if (_sub_win->event.button.button == 1)
            {
                //鼠标在拖拽区
                if (layout_sub_win[0].activation)
                {
                    _sub_win->offset_to_mouse.x = mouse_position_cur_global.x - window_sub.rect.x;
                    _sub_win->offset_to_mouse.y = mouse_position_cur_global.y - window_sub.rect.y;
                }
                //输入框事件
                UpdateInputBoxesEvent();
                //新建工程按钮事件
                UpdateNewPrjButtonEvent();
                //工程名字列表按钮事件
                UpdateProjectListEvent(mouse_position_cur_local.x, mouse_position_cur_local.y);

                //两大按钮点击事件
                for (size_t i = 0; i < 2; i++)
                {
                    if (window_sub.two_button[i].activation)
                    {
                        //鼠标点击按钮事件触发
                        TwoButtonSubWinEventClick(window_sub.two_button[i].id);
                        break;
                    }
                }
            }
        }
        break;
        case SDL_MOUSEBUTTONUP:
        {
            if (_sub_win->event.button.button == 1)
            {
                _sub_win->offset_to_mouse.x = -1;
                _sub_win->offset_to_mouse.y = -1;
            }
        }
        break;
        }
    }
}

void EventProcessInputBox(Input_Box *_input_box)
{
    int quit = 0;
    SDL_Event event;
    DrawInputBox(*_input_box);
    SDL_RenderPresent(window_main.render);
    while (!quit)
    {
        int pre_index = _input_box->curser_index;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_KEYDOWN:
            {
                if (event.key.keysym.sym == SDLK_RETURN)
                {
                    quit = 1;
                    break;
                }
                Scanf_s(_input_box->content,
                        _input_box->max_char_count,
                        &_input_box->curser_index,
                        event,
                        _input_box->only_number);
            }
            break;
            case SDL_MOUSEBUTTONDOWN:
            {
                if (event.button.button == 1)
                {
                    quit = 1;
                    break;
                }
                DeleteCharacter(event, &_input_box->curser_index, _input_box->content);
            }
            break;
            }
        }
        //绘制输入框
        if (pre_index != _input_box->curser_index)
        {
            DrawInputBox(*_input_box);
            SDL_RenderPresent(window_main.render);
        }
        SDL_Delay(1000 / FPS);
    }
    is_refresh_window = 1;
}

//是否能拖拽布局
static void CanGrabLayout()
{
    if (can_grab_layout)
        return;
    if (mouse_position_cur_local.x >= layout_editor[2].rect.x + layout_editor[2].rect.w - 2 &&
        mouse_position_cur_local.x <= layout_editor[3].rect.x + 2 &&
        mouse_position_cur_local.y > layout_editor[1].rect.y)
        can_grab_layout = 1;
}

static void UpdateInputBoxesEvent()
{
    for (size_t i = 0; i < window_sub.input_boxes_count; i++)
    {
        if (IsPointInRect(mouse_position_cur_local.x, mouse_position_cur_local.y, window_sub.input_boxes[i].rect))
        {
            EventProcessInputBox(&window_sub.input_boxes[i]);
            break;
        }
    }
}

static void UpdateProjectListEvent(int _mouse_x, int _mouse_y)
{
    if (prj_dir_list.prj_count <= 0)
        return;
    //所有列表的范围
    SDL_Rect rec;
    rec.x = window_sub.rect.x + 50;
    rec.y = window_sub.rect.y + LAYOUT_SUB_WIN_0_HEIGHT;
    rec.w = window_sub.rect.w - 100;
    rec.h = prj_dir_list.buttons[0].rect.h * prj_dir_list.prj_count;
    if (IsPointInRect(mouse_position_cur_local.x, mouse_position_cur_local.y, rec) == 0)
        return;
    for (size_t i = 0; i < prj_dir_list.prj_count; i++)
    {
        prj_dir_list.buttons[i].activation_pre = prj_dir_list.buttons[i].activation;
        prj_dir_list.buttons[i].activation = IsPointInRect(_mouse_x, _mouse_y, prj_dir_list.buttons[i].rect);
        if (prj_dir_list.buttons[i].activation)
            prj_dir_list.index = i;
        if (prj_dir_list.buttons[i].activation != prj_dir_list.buttons[i].activation_pre)
            is_refresh_window = 1;
    }
}

static void UpdateNewPrjButtonEvent()
{
    if (window_sub.type != NEW)
        return;
    for (size_t i = 0; i < BUTTON_NEW_PRJ_COUNT; i++)
    {
        if (button_new_prj[i].activation)
        {
            switch (button_new_prj[i].id)
            {
            case 0:
                tile_size_tmp = 8;
                text_boxes_new_prj[1].content = "8";
                break;
            case 1:
                tile_size_tmp = 16;
                text_boxes_new_prj[1].content = "16";
                break;
            case 2:
                tile_size_tmp = 32;
                text_boxes_new_prj[1].content = "32";
                break;
            }
            is_refresh_window = 1;
            break;
        }
    }
}

#endif