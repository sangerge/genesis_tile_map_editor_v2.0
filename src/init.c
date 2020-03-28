#ifndef _INIT_C_
#define _INIT_C_

#include <SDL2/SDL.h>
#include <stdio.h>
#include "attribute.c"
#include "mytool.c"
#include "timer_call.c"
#include "my_timer.c"

static void InitMainWindowSize();
static void InitColors();
static void InitButtonMenu();
static void InitButtonTile();
static void InitTwoButton();
static void InitNewPrjButtons();
static void InitNewPrjInputBoxes();
static void InitNewPrjTextBoxes();
static void InitTitles();
static void InitCanvas();
static void InitBrushModeSelectButton();

void Init()
{
    CreateSystemFolder();
    SDL_Init(SDL_INIT_VIDEO);
    InitCanvas();
    InitColors();
    grid_color_index = 0;
    brush.draw_collision_box = 0;
    open_dynamic_grid = 1;
    //获取屏幕分辨率
    SDL_GetCurrentDisplayMode(0, &display_mode);
    //设置编辑器窗口宽高
    InitMainWindowSize();
    //初始化main窗口
    InitWindowStruct(&window_main, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, win_width, win_height, "Mega Drive Tile Map Editor_v2.0", SDL_WINDOW_SHOWN, 0, window_bg_color);
    //初始化子窗口
    InitSubWindow(&window_sub, layout_editor[1].rect.x + 100, layout_editor[1].rect.y + 100, WIN_SUB_WIDTH, WIN_SUB_HEIGHT);
    //菜单
    InitLayout(window_main, &layout_editor[0], 0, LAYOUT_MENU_X, LAYOUT_MENU_Y, LAYOUT_MENU_WIDTH, LAYOUT_MENU_HEIGHT, layout_bg_color);
    //调色板
    InitLayout(window_main, &layout_editor[1], 1, LAYOUT_PALETTE_X, LAYOUT_PALETTE_Y, LAYOUT_PALETTE_WIDTH, LAYOUT_PALETTE_HEIGHT, layout_bg_color);
    //tile
    InitLayout(window_main, &layout_editor[2], 2, LAYOUT_TILE_X, LAYOUT_TILE_Y, LAYOUT_TILE_WIDTH, LAYOUT_TILE_HEIGHT, layout_bg_color);
    //map
    InitLayout(window_main, &layout_editor[3], 3, LAYOUT_MAP_X, LAYOUT_MAP_Y, LAYOUT_MAP_WIDTH, LAYOUT_MAP_HEIGHT, layout_bg_color);
    InitButtonMenu();
    InitButtonTile();
    InitBrushModeSelectButton();

    //子窗口==============
    //布局
    InitLayout(window_main, &layout_sub_win[0], 0, window_sub.rect.x, window_sub.rect.y, window_sub.rect.w, LAYOUT_SUB_WIN_0_HEIGHT, window_bg_color);
    InitLayout(window_main, &layout_sub_win[1], 1, window_sub.rect.x, window_sub.rect.y + LAYOUT_SUB_WIN_0_HEIGHT, window_sub.rect.w,
               window_sub.rect.h - LAYOUT_SUB_WIN_0_HEIGHT * 2, window_bg_color);
    InitLayout(window_main, &layout_sub_win[2], 2, window_sub.rect.x, layout_sub_win[1].rect.y + layout_sub_win[1].rect.h, window_sub.rect.w, LAYOUT_SUB_WIN_0_HEIGHT, window_bg_color);
    //初始化警告计时器
    InitTimer(&warning_timer, FPS * 3, &WarningEndCall);
    //创建这些按钮之后，需要window_sub里的指针指向这些数组，不同的子窗口有不同的按钮数组
    InitTwoButton();
    //初始化标题
    InitTitles();
    //======================================NEW===========
    //NEW窗口按钮
    InitNewPrjButtons();
    //New窗口文本框
    InitNewPrjTextBoxes();
    //NEW窗口输入框
    InitNewPrjInputBoxes();
    //======================================OPEN==============
    //OPEN窗口按钮
    //===================
    InitCollisionBoxList();
    //开启渲染队列
    is_refresh_window = 1;
}

static void InitMainWindowSize()
{
    win_width = display_mode.w - 20;
    win_height = display_mode.h - 60;
    if (display_mode.w > DEFAULT_WIN_WIDTH)
    {
        win_width = DEFAULT_WIN_WIDTH;
        win_height = DEFAULT_WIN_HEIGHT;
    }
}

static void InitColors()
{
    window_bg_color.r = 0xd3;
    window_bg_color.g = 0xcb;
    window_bg_color.b = 0xbe;
    window_bg_color.a = 0xff;

    layout_bg_color.r = 0x7d;
    layout_bg_color.g = 0x92;
    layout_bg_color.b = 0x9e;
    layout_bg_color.a = 0xff;

    layout_border_color.r = 0x00;
    layout_border_color.g = 0x00;
    layout_border_color.b = 0x00;

    button_normal_color.r = window_bg_color.r;
    button_normal_color.g = window_bg_color.g;
    button_normal_color.b = window_bg_color.b;
    button_normal_color.a = window_bg_color.a;

    button_enter_color.r = 0xff;
    button_enter_color.g = 0xeb;
    button_enter_color.b = 0xb6;
    button_enter_color.a = 0xff;

    grid_color_black.r = 0;
    grid_color_black.g = 0;
    grid_color_black.b = 0;
    grid_color_black.a = 0xff;

    grid_color_white.r = window_bg_color.r;
    grid_color_white.g = window_bg_color.g;
    grid_color_white.b = window_bg_color.b;
    grid_color_white.a = window_bg_color.a;
}

static void InitButtonMenu()
{
    InitButton(&button_menu[0], 0, " NEW", LAYOUT_SPACING + BUTTON_MENU_SPACING, LAYOUT_SPACING + BUTTON_MENU_SPACING, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT, 0);
    InitButton(&button_menu[1], 1, " OPEN", LAYOUT_SPACING + BUTTON_MENU_SPACING + BUTTON_MENU_WIDTH, LAYOUT_SPACING + BUTTON_MENU_SPACING, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT, 0);
    InitButton(&button_menu[2], 2, " SAVE", LAYOUT_SPACING + BUTTON_MENU_SPACING + BUTTON_MENU_WIDTH * 2, LAYOUT_SPACING + BUTTON_MENU_SPACING, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT, 0);
    InitButton(&button_menu[3], 3, "  EXPORT", LAYOUT_SPACING + BUTTON_MENU_SPACING + BUTTON_MENU_WIDTH * 3, LAYOUT_SPACING + BUTTON_MENU_SPACING, BUTTON_MENU_WIDTH * 1.5, BUTTON_MENU_HEIGHT, 0);
    InitButton(&button_menu[4], 4, " GRID", LAYOUT_SPACING + BUTTON_MENU_SPACING + BUTTON_MENU_WIDTH * 9, LAYOUT_SPACING + BUTTON_MENU_SPACING, BUTTON_MENU_WIDTH, BUTTON_MENU_HEIGHT, 0);
    InitButton(&button_menu[5], 5, "  Light Grid", LAYOUT_SPACING + BUTTON_MENU_SPACING + BUTTON_MENU_WIDTH * 10, LAYOUT_SPACING + BUTTON_MENU_SPACING, BUTTON_MENU_WIDTH * 2, BUTTON_MENU_HEIGHT, 0);
    InitButton(&button_menu[6], 6, "  Dark Grid", LAYOUT_SPACING + BUTTON_MENU_SPACING + BUTTON_MENU_WIDTH * 12, LAYOUT_SPACING + BUTTON_MENU_SPACING, BUTTON_MENU_WIDTH * 2, BUTTON_MENU_HEIGHT, 0);
    InitButton(&button_menu[7], 7, " Dy-Grid", LAYOUT_SPACING + BUTTON_MENU_SPACING + BUTTON_MENU_WIDTH * 16, LAYOUT_SPACING + BUTTON_MENU_SPACING, BUTTON_MENU_WIDTH * 1.2, BUTTON_MENU_HEIGHT, 0);
}

static void InitTwoButton()
{
    InitButton(&two_button[0], 0, "        O.K", 0, window_sub.rect.h - LAYOUT_SUB_WIN_0_HEIGHT, window_sub.rect.w * 0.5, LAYOUT_SUB_WIN_0_HEIGHT, 1);
    InitButton(&two_button[1], 1, "     CANCEL", window_sub.rect.w * 0.5, window_sub.rect.h - LAYOUT_SUB_WIN_0_HEIGHT, window_sub.rect.w * 0.5, LAYOUT_SUB_WIN_0_HEIGHT, 1);
    window_sub.two_button = two_button;
}

static void InitNewPrjButtons()
{
    InitButton(&button_new_prj[0], 0, "  8px", 50, LAYOUT_SUB_WIN_0_HEIGHT * 3, 100, LAYOUT_SUB_WIN_0_HEIGHT, 1);
    InitButton(&button_new_prj[1], 1, " 16px", 50 + 100, LAYOUT_SUB_WIN_0_HEIGHT * 3, 100, LAYOUT_SUB_WIN_0_HEIGHT, 1);
    InitButton(&button_new_prj[2], 2, " 32px", 50 + 100 * 2, LAYOUT_SUB_WIN_0_HEIGHT * 3, 100, LAYOUT_SUB_WIN_0_HEIGHT, 1);
}

static void InitTitles()
{
    InitTextBox(&window_sub.title, "New Project", 10, FONT_SIZE_SUB_WIN_TITLE, window_sub.rect.w * 0.5, LAYOUT_SUB_WIN_0_HEIGHT, FONT_SIZE_SUB_WIN_TITLE);
}

static void InitNewPrjTextBoxes()
{
    // InitTextBox(&text_boxes_new_prj[0], "New Project", 10, FONT_SIZE_SUB_WIN_TITLE, window_sub.rect.w * 0.5, LAYOUT_SUB_WIN_0_HEIGHT, FONT_SIZE_SUB_WIN_TITLE);
    InitTextBox(&text_boxes_new_prj[0], "Select Tile Size:", 10, LAYOUT_SUB_WIN_0_HEIGHT * 2, 50, LAYOUT_SUB_WIN_0_HEIGHT, FONT_SIZE_SUB_WIN_TIPS);
    InitTextBox(&text_boxes_new_prj[1], "8", window_sub.rect.w * 0.4, LAYOUT_SUB_WIN_0_HEIGHT * 2, window_sub.rect.w, LAYOUT_SUB_WIN_0_HEIGHT, FONT_SIZE_SUB_WIN_TIPS);
    InitTextBox(&text_boxes_new_prj[2], "Map Width  [unit:tile]:", 10, LAYOUT_SUB_WIN_0_HEIGHT * 6, window_sub.rect.w, LAYOUT_SUB_WIN_0_HEIGHT, FONT_SIZE_SUB_WIN_TIPS);
    InitTextBox(&text_boxes_new_prj[3], "Map Height [unit:tile]:", 10, LAYOUT_SUB_WIN_0_HEIGHT * 9, window_sub.rect.w, LAYOUT_SUB_WIN_0_HEIGHT, FONT_SIZE_SUB_WIN_TIPS);
    InitTextBox(&text_boxes_new_prj[4], "Project Name:", 10, LAYOUT_SUB_WIN_0_HEIGHT * 14, window_sub.rect.w, LAYOUT_SUB_WIN_0_HEIGHT, FONT_SIZE_SUB_WIN_TIPS);
}

static void InitNewPrjInputBoxes()
{
    //Map Width 输入框
    InitInputBox(&input_box_new_prj[0], 50, LAYOUT_SUB_WIN_0_HEIGHT * 7, 60, LAYOUT_SUB_WIN_0_HEIGHT, MAX_NUMBER_LENGTH, 1, &map_size.x);
    //Map Height 输入框
    InitInputBox(&input_box_new_prj[1], 50, LAYOUT_SUB_WIN_0_HEIGHT * 10, 60, LAYOUT_SUB_WIN_0_HEIGHT, MAX_NUMBER_LENGTH, 1, &map_size.y);
    //Project Name 输入框
    InitInputBox(&input_box_new_prj[2], 50, LAYOUT_SUB_WIN_0_HEIGHT * 15, 160, LAYOUT_SUB_WIN_0_HEIGHT, MAX_STRING_LENGTH, 0, prj_name);
}

static void InitButtonTile()
{
    InitButton(&button_tile[0], 0, "  +Tile",
               layout_editor[2].rect.x,
               layout_editor[2].rect.y + layout_editor[2].rect.h + BUTTON_MENU_SPACING,
               layout_editor[2].rect.w * 0.5 - BUTTON_MENU_SPACING,
               LAYOUT_SUB_WIN_0_HEIGHT - BUTTON_MENU_SPACING,
               0);
    InitButton(&button_tile[1], 1, " +CollisionBox",
               layout_editor[2].rect.x + layout_editor[2].rect.w * 0.5,
               layout_editor[2].rect.y + layout_editor[2].rect.h + BUTTON_MENU_SPACING,
               layout_editor[2].rect.w * 0.5,
               LAYOUT_SUB_WIN_0_HEIGHT - BUTTON_MENU_SPACING,
               0);
}

static void InitCanvas()
{
    canvas_map.scale = &scale_data[0];
    canvas_map.show_grid = 1;
    canvas_map.ID = 3;

    canvas_tile.scale = &scale_data[0];
    canvas_tile.show_grid = 1;
    canvas_tile.ID = 2;
}

static void InitBrushModeSelectButton()
{
    InitButton(&brush_mode_button[0], 0, "TL", layout_editor[3].rect.x + 2, layout_editor[3].rect.y + 300, 32, 64, 0);
    InitButton(&brush_mode_button[1], 1, "CB", layout_editor[3].rect.x + 2, layout_editor[3].rect.y + 300 + 100, 32, 64, 0);
}

#endif