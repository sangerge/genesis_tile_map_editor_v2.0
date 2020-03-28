#ifndef _ATTRIBUTE_C_
#define _ATTRIBUTE_C_

#include <SDL2/SDL.h>
#include "my_timer.c"

#define DEFAULT_WIN_WIDTH 1800
#define DEFAULT_WIN_HEIGHT 960
#define WIN_EDITOR_COUNT 4
#define WIN_BORDER_HEIGHT 30
#define LAYOUT_SPACING 3
#define LAYOUT_EDITOR_COUNT 4
//菜单按钮数量
#define BUTTON_MENU_COUNT 8
//创建工程窗口按钮数量，8,16,32
#define BUTTON_NEW_PRJ_COUNT 3
//子窗口布局0高度
#define LAYOUT_SUB_WIN_0_HEIGHT 30

//布局
#define LAYOUT_MENU_X LAYOUT_SPACING
#define LAYOUT_MENU_Y LAYOUT_SPACING
#define LAYOUT_MENU_WIDTH win_width - LAYOUT_SPACING * 2
#define LAYOUT_MENU_HEIGHT 30
#define LAYOUT_PALETTE_X LAYOUT_SPACING
#define LAYOUT_PALETTE_Y LAYOUT_MENU_HEIGHT + LAYOUT_SPACING * 2
#define LAYOUT_PALETTE_WIDTH 450
#define LAYOUT_PALETTE_HEIGHT 120
#define LAYOUT_TILE_X LAYOUT_SPACING
#define LAYOUT_TILE_Y LAYOUT_MENU_HEIGHT + LAYOUT_PALETTE_HEIGHT + LAYOUT_SPACING * 3
#define LAYOUT_TILE_WIDTH LAYOUT_PALETTE_WIDTH
#define LAYOUT_TILE_HEIGHT win_height - LAYOUT_MENU_HEIGHT - LAYOUT_PALETTE_HEIGHT - LAYOUT_SPACING * 4 - LAYOUT_SUB_WIN_0_HEIGHT
#define LAYOUT_MAP_X LAYOUT_PALETTE_WIDTH + LAYOUT_SPACING * 4
#define LAYOUT_MAP_Y LAYOUT_PALETTE_Y
#define LAYOUT_MAP_WIDTH win_width - LAYOUT_PALETTE_WIDTH - LAYOUT_SPACING * 5
#define LAYOUT_MAP_HEIGHT win_height - LAYOUT_MENU_HEIGHT - LAYOUT_SPACING * 3

#define BUTTON_MENU_WIDTH 100
#define BUTTON_MENU_HEIGHT 28
#define BUTTON_MENU_SPACING 1

#define FONT_SIZE_LAYOUT_MENU_BUTTON 14
#define FONT_SIZE_OK_BUTTON 14
//子窗口布局数量，一个移动布局，一个操作布局
#define LAYOUT_COUNT_SUB_WIN 3
//子窗口宽
#define WIN_SUB_WIDTH 480
//子窗口高度
#define WIN_SUB_HEIGHT 550
//new prj窗口文字框数量
#define NEW_PRJ_TEXT_BOXES_COUNT 5
//子窗口标题文字尺寸
#define FONT_SIZE_SUB_WIN_TITLE 12
//子窗口提示文字尺寸
#define FONT_SIZE_SUB_WIN_TIPS 16
//子窗口输入文字尺寸
#define FONT_SIZE_SUB_WIN_INPUT_TEXT 14
//NEW窗口输入框数量
#define NEW_PRJ_INPUT_BOX_COUNT 3
//最大数字输入框为4*20
//最大字符输入框为18个
//最大字符串长度
#define MAX_STRING_LENGTH 19
//最大数字长度
#define MAX_NUMBER_LENGTH 6
//工程路径
#define PRJ_PATH "./Project/"
//Tile目录
#define TILE_PATH "./Tile/"
//完整工程路径字符串最大长度
#define PRJ_PATH_STR_LENGTH 30
//存档名字
#define SAVE_NAME "save.dat"
//导出的地图名字
#define EXPORT_MAP_NAME "map.bmp"
//导出的碰撞盒数据
#define EXPORT_MAP_DATA "map_data.c"
//最大尺寸8000*8000
#define MAX_CANVAS_SIZE 8000
//调色板最大颜色数
#define MAX_PALETTE_COLOR 65
//最大碰撞框种类，最大40种，26个英文字母+10个阿拉伯数字+_+.
#define MAX_COLLISION_BOX_KIND 36
//每种碰撞盒最大碰撞盒数量3000
#define MAX_COLLISION_BOX_COUNT_ONE_KIND 3000
//碰撞盒编辑器区，横向数量
#define BOXCOLLISION_EDITOR_H_COUNT 2;
//碰撞盒编辑器区，纵向数量
#define BOXCOLLISION_EDITOR_V_COUNT 18;
//浮点数
#define FLOAT 0.000001

typedef struct vector
{
    int x;
    int y;
} Vector;

typedef struct vector_f
{
    float x;
    float y;
} Vector_f;

typedef struct _window_struct
{
    //xy代表窗口位置，wh代表窗口宽高
    SDL_Rect rect;
    SDL_Window *window;
    SDL_Renderer *render;
    SDL_Color bg_color;
    SDL_Event event;
    int quit;
} Window_Struct;

typedef struct _layout_struct
{
    int id;
    int activation_pre;
    //0未激活，1激活
    int activation;
    //布局坐标以及宽高
    SDL_Rect rect;
    //布局贴图
    SDL_Texture *texture;
    SDL_Color bg_color;
} Layout;

typedef struct _button
{
    //1激活，0未激活
    int activation_pre;
    //1激活，0未激活
    int activation;
    //相对父窗口的偏移值
    Vector offset_to_parent;
    //坐标以及大小
    SDL_Rect rect;
    //ID
    int id;
    //文字
    char *name;
    //正常颜色
    SDL_Color color_normal;
    //进入颜色
    SDL_Color color_enter;
} Button;

typedef enum _sub_win_type
{
    DEFAULT = 0,
    NEW = 1,
    OPEN = 2,
    SAVE = 3,
    EXPORT = 4,
    OPTION = 5,
} SUB_WIN_TYPE;

//文本框
typedef struct _text_box
{
    Vector offset;
    SDL_Rect rect;
    char *content;
    int font_size;
} Text_Box;

typedef struct _input_box
{
    //工程名字
    void *target;
    //1只能输入数字，0可以混输
    int only_number;
    int activation_pre;
    int activation;
    Vector offset;
    //xy位置，wh宽高，初始化时用相对子窗口的位置，然后对齐函数对齐一下
    SDL_Rect rect;
    //可输入的最多字符数
    int max_char_count;
    //光标位置索引
    int curser_index;
    //接收输入的字符
    char *content;
} Input_Box;

typedef struct _sub_window
{
    //标题
    Text_Box title;
    //标记一下是哪个按钮打开的子窗口
    SUB_WIN_TYPE type;
    //是否打开
    int open;
    SDL_Rect rect;
    SDL_Texture *texture;
    //文本框数量
    int text_boxes_count;
    //文字框数组
    Text_Box *text_boxes;
    //按钮数量
    int button_count;
    Button *buttons;
    Button *two_button;
    int layout_count;
    Layout *layouts;
    Input_Box *input_boxes;
    int input_boxes_count;
    SDL_Event event;
    //xy小于0代表没有拖拽
    Vector offset_to_mouse;
} Sub_Window;

//目录工程列表
typedef struct _prj_dir_list
{
    //选中的索引号
    int index;
    //用来存放所有目录
    char **dir_name_list;
    //工程的个数
    int prj_count;
    //每个工程对应一个按钮
    Button *buttons;
} Prj_Dir_List;

//刷新率
int FPS = 144;
//获取显示器的分辨率
SDL_DisplayMode display_mode;
//窗口宽度
int win_width = DEFAULT_WIN_WIDTH;
//窗口高度
int win_height = DEFAULT_WIN_HEIGHT;
//主窗口结构体
Window_Struct window_main;
//程序结束
SDL_bool quit = SDL_FALSE;
//鼠标上一个屏幕位置
Vector mouse_position_pre_global;
//鼠标当前屏幕位置
Vector mouse_position_cur_global;
//鼠标上一个窗口位置
Vector mouse_position_pre_local;
//鼠标当前窗口位置
Vector mouse_position_cur_local;
//背景颜色
SDL_Color window_bg_color;
//布局颜色
SDL_Color layout_bg_color;
//布局边框颜色
SDL_Color layout_border_color;
//按钮正常颜色
SDL_Color button_normal_color;
//按钮进入颜色
SDL_Color button_enter_color;
//布局数组，0：menu，1：palette，2：tile，3：map
Layout layout_editor[LAYOUT_EDITOR_COUNT];
//子窗口布局,0:title,1:中间布局,2:OK,CANCEL按钮布局
Layout layout_sub_win[LAYOUT_COUNT_SUB_WIN];
//菜单按钮
Button button_menu[BUTTON_MENU_COUNT];
//是否刷新窗口
int is_refresh_window = 0;
//是否能拖拽布局
int can_grab_layout = 0;
//子窗口
Sub_Window window_sub;
//OK和CANCEL按钮
Button two_button[2];
//Load and edit
Button button_tile[2];
//新建工程布局按钮
Button button_new_prj[BUTTON_NEW_PRJ_COUNT];
//新建工程文字框
Text_Box text_boxes_new_prj[NEW_PRJ_TEXT_BOXES_COUNT];
//NEW窗口输入框数量
Input_Box input_box_new_prj[NEW_PRJ_INPUT_BOX_COUNT];

//工程名字
char prj_name[MAX_STRING_LENGTH] = {'\0'};

//工程目录列表
Prj_Dir_List prj_dir_list;
//当前工程目录
char *cur_project_path = NULL;
//警告文字
char *warning = NULL;
//是否显示警告
int show_warning = 0;

Timer warning_timer;

//============================================================================================

//缩放数据
int scale_data[4] = {1, 2, 4, 8};
//画布结构体，并不是布局3的尺寸，他是真实画布大小，绘制的那个尺寸，根据缩放值改变的那个尺寸
typedef struct
{
    //指向缩放数据的指针
    int *scale;
    //map id = 1；tile id = 0
    int ID;
    //横向tile块数
    int h_count;
    //纵向tile块数
    int v_count;
    //真实的位置和尺寸信息
    SDL_Rect rect;
    //tile用来载入图块；map用来贴画笔的tile块
    SDL_Surface *surface;
    SDL_Texture *texture;
    //是否显示网格
    int show_grid;
} Canvas;

typedef struct
{
    //0：绘制TILE，1：绘制碰撞框
    int draw_collision_box;
    //开始拷贝，初始为0,点击变为1标记一下成功进入copy状态，点击的时候直接计算出那个图块的rect信息，并且动态创建一个二维数组记录tile值，松开的时候如果startCopy是1，再次计算一次rect，startCopy变为0
    int startCopy;
    int startCopyInMap;
    //单位是tile
    Vector startPos;
    Vector startPosInMap;
    //单位是tile
    Vector endPos;
    Vector endPosInMap;
    //单位是tile
    SDL_Rect copyRect;
    SDL_Rect copyRectInMap;
    //======================
    int startPaste; //针对map
    //上一个在map上的位置
    SDL_Rect posInMap_pre;
    //在Map上的位置，坐标基于像素,宽高基于像素（不带缩放）
    SDL_Rect posInMap;
    //指向tile数组的指针，每一次copy之前都要释放内存，然后重新创建，结束程序的时候再次释放
    // char ***valuePointer;
} Brush;

//画布tile
Canvas canvas_tile;
//画布map
Canvas canvas_map;
//抓住的画布
Canvas *grabCanvas = NULL;
//Tile块大小，默认8，最小8
int tile_size = 8;
//临时的Tile块大小
int tile_size_tmp = 8;
//地图尺寸，基于Tile
Vector map_size;
//调色板
SDL_Palette *pal = NULL;
//鼠标到画布的偏移值
Vector offset_mouse_to_canvas;
//布局索引
int layoutIndex = 0;
//0白色网格，1黑色网格
int grid_color_index = 0;
//黑色网格颜色
SDL_Color grid_color_black;
//白色网格颜色
SDL_Color grid_color_white;
//画笔
Brush brush;
//用来标记从碰撞盒模式切回tile绘制模式时动态网格的状态
int open_dynamic_grid_pre = 1;
//是否打开动态网格
int open_dynamic_grid = 1;
//动态map尺寸，绘制不同大小的tile块用到
Vector dynamicMapSize;

//====================================
Button brush_mode_button[2];

//每一种碰撞盒的列表
typedef struct Collision_Box_List
{
    //被选中的索引
    int index;
    int count;
    SDL_Color color;
    char value;
    SDL_Rect **rects;
} C_Box_List;

//碰撞盒列表
typedef struct C_Box_Lists
{
    //单独显示
    int solo;
    //被选中的index索引
    int index;
    C_Box_List **lists;
    int count;
} C_Box_Lists;

C_Box_Lists c_box_lists;
//随机种子
time_t seed = 0;
//保存tile的像素数据
Uint32 *pixels = NULL;
int pitch = 0;
//是否打开工程
int opened_project = 0;
//工程是否改变
int project_changed = 0;
int project_changed_pre = 0;

#endif