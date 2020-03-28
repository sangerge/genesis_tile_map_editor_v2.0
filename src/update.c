#ifndef _UPDATE_C_
#define _UPDATE_C_

#include <SDL2/SDL.h>
#include <stdio.h>
#include "attribute.c"
#include "mytool.c"
#include "event_process.c"

void UpdateLayoutEvent();
void UpdateLayoutIndex();
void UpdateLayoutEventSubWin();
void UpdateButtonMenuEvent();
void UpdateButtonTilePosition();
void UpdateLayoutOffset();
void UpdateSubWinPosition();
void UpdateSubWinButtonEvent();
void UpdateSubWinTwoButtonEvent();
void UpdateSubWinButtonPosition();
void UpdateSubWinTwoButtonPosition();
void UpdateTextBoxesPosition();
void UpdateInuputBoxesPosition();
void UpdateProjectListButtons();
void UpdateButtonTileEvent();
static void UpdateBrushGetEndPos();
static void UpdateBrushGetEndPosInMap();
static void UpdateBrushModeButtonPos();
static void UpdateBrushModeButtonEvent();
static void UpdateWindowTitle();

void Update()
{
    while (!window_main.quit)
    {
        GetMousePosition(&mouse_position_pre_global, &mouse_position_cur_global, 1);
        GetMousePosition(&mouse_position_pre_local, &mouse_position_cur_local, 0);
        GetWindowPosition(&window_main);
        project_changed_pre = project_changed;

        if (!window_sub.open)
        {
            UpdateButtonMenuEvent();
            UpdateLayoutEvent();
            UpdateLayoutIndex();
            EventProcess(&window_main);
            UpdateLayoutOffset();
            CanvasFollowMouse();
            UpdateButtonTilePosition();
            UpdateBrushModeButtonPos();
            UpdateButtonTileEvent();
            UpdateBrushModeButtonEvent();
            UpdateBrushGetEndPos();
            UpdateBrushGetEndPosInMap();
            GetBrushPosInMap();
            GetACollisionBoxListIndex();
        }

        if (window_sub.open)
        {
            EventProcessSubWin(&window_sub);
            //更新子窗口的位置和布局位置
            UpdateSubWinPosition();
            //更新按钮的位置
            UpdateSubWinButtonPosition();
            //更新两大按钮的位置
            UpdateSubWinTwoButtonPosition();
            //更新文本框的位置
            UpdateTextBoxesPosition();
            //更新输入框的位置
            UpdateInuputBoxesPosition();
            //更新工程列表按钮的位置
            UpdateProjectListButtons();

            //更新布局事件
            UpdateLayoutEventSubWin();
            //更新按钮事件
            UpdateSubWinButtonEvent();
            //更新两大按钮事件
            UpdateSubWinTwoButtonEvent();
        }
        //更新警告计时器
        UpdateTimer(&warning_timer);
        //渲染队列
        RenderQueue();
        //更新窗口title
        UpdateWindowTitle();
        // ReadProjectInfo();
        SDL_Delay(1000 / FPS);
    }
}

void UpdateLayoutEvent()
{
    for (size_t i = 0; i < LAYOUT_EDITOR_COUNT; i++)
        UpdateLayoutActivation(&layout_editor[i]);
}

//更新当前布局索引
void UpdateLayoutIndex()
{
    for (size_t i = 0; i < LAYOUT_EDITOR_COUNT; i++)
    {
        if (layout_editor[i].activation)
        {
            layoutIndex = i;
            break;
        }
        layoutIndex = -1;
    }
}

void UpdateLayoutEventSubWin()
{
    for (size_t i = 0; i < LAYOUT_COUNT_SUB_WIN; i++)
        UpdateLayoutActivation(&layout_sub_win[i]);
}

//更新拖拽布局的偏移值
void UpdateLayoutOffset()
{
    if (!can_grab_layout)
        return;
    layout_editor[1].rect.w = mouse_position_cur_local.x - LAYOUT_SPACING;
    layout_editor[2].rect.w = layout_editor[1].rect.w;
    layout_editor[3].rect.x = layout_editor[1].rect.w + LAYOUT_SPACING * 4;
    layout_editor[3].rect.w = win_width - layout_editor[1].rect.w - LAYOUT_SPACING * 5;

    if (layout_editor[1].rect.w < 50)
    {
        layout_editor[1].rect.w = 50;
        layout_editor[2].rect.w = layout_editor[1].rect.w;
        layout_editor[3].rect.x = layout_editor[1].rect.w + LAYOUT_SPACING * 4;
        layout_editor[3].rect.w = win_width - layout_editor[1].rect.w - LAYOUT_SPACING * 5;
    }
    else if (layout_editor[3].rect.w < 50)
    {
        layout_editor[3].rect.w = 50;
        layout_editor[3].rect.x = win_width - 50 - LAYOUT_SPACING;
        layout_editor[1].rect.w = win_width - layout_editor[3].rect.w - LAYOUT_SPACING * 5;
        layout_editor[2].rect.w = layout_editor[1].rect.w;
    }

    if (mouse_position_cur_local.x != mouse_position_pre_local.x)
    {
        is_refresh_window = 1;
    }
}

void UpdateSubWinPosition()
{
    //没有拖拽时不更新
    if (window_sub.offset_to_mouse.x < 0 || window_sub.offset_to_mouse.y < 0)
        return;
    //更新窗口位置
    if (window_sub.offset_to_mouse.x > 0 && window_sub.offset_to_mouse.y > 0)
    {
        window_sub.rect.x = mouse_position_cur_global.x - window_sub.offset_to_mouse.x;
        window_sub.rect.y = mouse_position_cur_global.y - window_sub.offset_to_mouse.y;
    }
    //限制窗口位置
    if (window_sub.rect.x < 0)
        window_sub.rect.x = 0;
    if (window_sub.rect.x + window_sub.rect.w > win_width)
        window_sub.rect.x = win_width - window_sub.rect.w;
    if (window_sub.rect.y < layout_editor[1].rect.y)
        window_sub.rect.y = layout_editor[1].rect.y;
    if (window_sub.rect.y + window_sub.rect.h > win_height)
        window_sub.rect.y = win_height - window_sub.rect.h;
    //更新布局位置
    layout_sub_win[0].rect.x = window_sub.rect.x;
    layout_sub_win[0].rect.y = window_sub.rect.y;
    layout_sub_win[1].rect.x = layout_sub_win[0].rect.x;
    layout_sub_win[1].rect.y = window_sub.rect.y + LAYOUT_SUB_WIN_0_HEIGHT;
    layout_sub_win[2].rect.x = window_sub.rect.x;
    layout_sub_win[2].rect.y = layout_sub_win[1].rect.y + layout_sub_win[1].rect.h;
    //更新按钮位置

    //刷新窗口
    if (mouse_position_pre_local.x != mouse_position_cur_local.y ||
        mouse_position_pre_local.y != mouse_position_pre_local.y)
    {
        is_refresh_window = 1;
    }
}

void UpdateButtonMenuEvent()
{
    for (size_t i = 0; i < BUTTON_MENU_COUNT; i++)
    {
        UpdateButtonActivation(&button_menu[i]);
        if (!button_menu[i].activation_pre && button_menu[i].activation)
            is_refresh_window = 1;
        else if (button_menu[i].activation_pre && !button_menu[i].activation)
            is_refresh_window = 1;
    }
}

void UpdateSubWinButtonEvent()
{
    if (!window_sub.buttons)
        return;
    for (size_t i = 0; i < window_sub.button_count; i++)
    {
        UpdateButtonActivation(&window_sub.buttons[i]);
        if (!window_sub.buttons[i].activation_pre && window_sub.buttons[i].activation)
            is_refresh_window = 1;
        else if (window_sub.buttons[i].activation_pre && !window_sub.buttons[i].activation)
            is_refresh_window = 1;
    }
}

//两大按钮事件
void UpdateSubWinTwoButtonEvent()
{
    for (size_t i = 0; i < 2; i++)
    {
        UpdateButtonActivation(&window_sub.two_button[i]);
        if (!window_sub.two_button[i].activation_pre && window_sub.two_button[i].activation)
            is_refresh_window = 1;
        else if (window_sub.two_button[i].activation_pre && !window_sub.two_button[i].activation)
            is_refresh_window = 1;
    }
}

void UpdateSubWinButtonPosition()
{
    if (!window_sub.buttons)
        return;
    for (size_t i = 0; i < window_sub.button_count; i++)
    {
        window_sub.buttons[i].rect.x = window_sub.rect.x + window_sub.buttons[i].offset_to_parent.x;
        window_sub.buttons[i].rect.y = window_sub.rect.y + window_sub.buttons[i].offset_to_parent.y;
    }
}

void UpdateSubWinTwoButtonPosition()
{
    if (!window_sub.two_button)
        return;
    for (size_t i = 0; i < 2; i++)
    {
        window_sub.two_button[i].rect.x = window_sub.rect.x + window_sub.two_button[i].offset_to_parent.x;
        window_sub.two_button[i].rect.y = window_sub.rect.y + window_sub.two_button[i].offset_to_parent.y;
    }
}

void UpdateTextBoxesPosition()
{
    if (!window_sub.text_boxes)
        return;
    for (size_t i = 0; i < window_sub.text_boxes_count; i++)
    {
        window_sub.text_boxes[i].rect.x = window_sub.rect.x + window_sub.text_boxes[i].offset.x;
        window_sub.text_boxes[i].rect.y = window_sub.rect.y + window_sub.text_boxes[i].offset.y;
    }
}

void UpdateInuputBoxesPosition()
{
    if (!window_sub.input_boxes)
        return;
    for (size_t i = 0; i < window_sub.input_boxes_count; i++)
    {
        window_sub.input_boxes[i].rect.x = window_sub.rect.x + window_sub.input_boxes[i].offset.x;
        window_sub.input_boxes[i].rect.y = window_sub.rect.y + window_sub.input_boxes[i].offset.y;
    }
}

void UpdateProjectListButtons()
{
    if (prj_dir_list.prj_count <= 0 || window_sub.type != OPEN)
        return;
    for (size_t i = 0; i < prj_dir_list.prj_count; i++)
    {
        prj_dir_list.buttons[i].rect.x = window_sub.rect.x + prj_dir_list.buttons[i].offset_to_parent.x;
        prj_dir_list.buttons[i].rect.y = window_sub.rect.y + prj_dir_list.buttons[i].offset_to_parent.y;
    }
}

void UpdateButtonTilePosition()
{
    if (layout_editor[2].rect.w * 0.5 <= 100)
    {
        button_tile[0].name = "+T";
        button_tile[1].name = "+C";
    }
    else if (layout_editor[2].rect.w * 0.5 <= 170)
    {
        button_tile[0].name = " +Tile";
        button_tile[1].name = " +Collis";
    }
    else
    {
        button_tile[0].name = "  +Tile";
        button_tile[1].name = " +CollisionBox";
    }

    button_tile[0].rect.w = layout_editor[2].rect.w * 0.5 - BUTTON_MENU_SPACING;
    button_tile[1].rect.x = layout_editor[2].rect.x + layout_editor[2].rect.w * 0.5;
    button_tile[1].rect.w = layout_editor[2].rect.w * 0.5;
}

void UpdateButtonTileEvent()
{
    for (size_t i = 0; i < 2; i++)
    {
        UpdateButtonActivation(&button_tile[i]);
        if (!button_tile[i].activation_pre && button_tile[i].activation)
            is_refresh_window = 1;
        else if (button_tile[i].activation_pre && !button_tile[i].activation)
            is_refresh_window = 1;
    }
}

static void UpdateBrushGetEndPos()
{
    if (brush.startCopy &&
        (mouse_position_cur_global.x != mouse_position_pre_global.x || mouse_position_cur_global.y != mouse_position_pre_global.y))
    {
        BrushGetEndPos();
        is_refresh_window = 1;
    }
}

static void UpdateBrushGetEndPosInMap()
{
    if (brush.startCopyInMap &&
        (mouse_position_cur_global.x != mouse_position_pre_global.x || mouse_position_cur_global.y != mouse_position_pre_global.y))
    {
        BrushGetEndPosInMap();
        is_refresh_window = 1;
    }
}

static void UpdateBrushModeButtonPos()
{
    brush_mode_button[0].rect.x = layout_editor[3].rect.x + 2;
    brush_mode_button[1].rect.x = brush_mode_button[0].rect.x;
}

static void UpdateBrushModeButtonEvent()
{
    if (can_grab_layout)
        return;
    for (size_t i = 0; i < 2; i++)
    {
        UpdateButtonActivation(&brush_mode_button[i]);
        if (!brush_mode_button[i].activation_pre && brush_mode_button[i].activation)
            is_refresh_window = 1;
        else if (brush_mode_button[i].activation_pre && !brush_mode_button[i].activation)
            is_refresh_window = 1;
    }
}

static void UpdateWindowTitle()
{
    if (!project_changed_pre && project_changed)
        SDL_SetWindowTitle(window_main.window, "Mega Drive Tile Map Editor_v2.0                    **工程已经修改，注意保存");
    else if (project_changed_pre && !project_changed)
        SDL_SetWindowTitle(window_main.window, "Mega Drive Tile Map Editor_v2.0");
}

#endif