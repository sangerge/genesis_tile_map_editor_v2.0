#ifndef _MYTOOL_C_
#define _MYTOOL_C_

#include <SDL2/SDL.h>
#include <SDL2/SDL_test.h>
#include <stdio.h>
#include <dirent.h>
#include "attribute.c"
#include "my_io.c"
#include "my_lib.c"
#include "collision_box.c"

//遇BUG不解来一句
extern void CaoNiMa()
{
    SDLTest_DrawString(window_main.render, 0, 0, " ");
}

/*
    _win_struct:Window_Struct结构体指针
    _pos_x:窗口的位置
    _pos_y:窗口的位置
    _size_w:窗口宽
    _size_h:窗口高
    _title:窗口标题
    _window_flag:SDL_WindowFlags
    _render_index:渲染器index
    _render_flag:SDL_RendererFlags
*/
void InitWindowStruct(Window_Struct *_win_struct, int _pos_x, int _pos_y, int _size_w, int _size_h, const char *_title, SDL_WindowFlags _flag, int _render_index, SDL_Color _bg_color)
{
    _win_struct->rect.x = _pos_x;
    _win_struct->rect.y = _pos_y;
    _win_struct->rect.w = _size_w;
    _win_struct->rect.h = _size_h;
    _win_struct->window = SDL_CreateWindow(_title, _win_struct->rect.x, _win_struct->rect.y, _win_struct->rect.w, _win_struct->rect.h, _flag);
    _win_struct->render = SDL_CreateRenderer(_win_struct->window, _render_index, SDL_RENDERER_ACCELERATED);
    _win_struct->bg_color = _bg_color;
    _win_struct->quit = 0;
}

//在屏幕上显示文字
extern void ShowCharacter(SDL_Renderer *_render, char *_str, Uint8 _font_pixel_size, int _x, int _y, Uint8 _r, Uint8 _g, Uint8 _b, int _bg_transparent)
{
    //获取字符串的长度
    int length = 0;
    while (_str[length] != '\0')
        length++;
    SDL_Rect _tmp_rect;
    _tmp_rect.x = _x + 8;
    _tmp_rect.y = _y - 9;
    _tmp_rect.w = length * 8 + 4;
    _tmp_rect.h = 8 + 8;

    if (!_bg_transparent)
    {
        SDL_SetRenderDrawColor(window_main.render, 0, 0, 0, 0xff);
        SDL_RenderFillRect(window_main.render, &_tmp_rect);
    }
    SDL_SetRenderDrawColor(window_main.render, _r, _g, _b, 0xff);
    SDLTest_DrawString(window_main.render, _x + 10, _y - 5, _str);

    return;
    /*
    //获取字符串的长度
    int length = 0;
    while (_str[length] != '\0')
        length++;
    //先创建一个texture
    SDL_Texture *texture = SDL_CreateTexture(_render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, length * 8, 8);
    //更改渲染贴图
    SDL_SetRenderTarget(_render, texture);
    SDL_Color col;
    col.r = _r;
    col.g = _g;
    col.b = _b;
    col.a = 0xff;
    //设置字体颜色
    SDL_SetRenderDrawColor(_render, col.r, col.g, col.b, col.a);
    //把字符打印到texture上
    SDLTest_DrawString(_render, 0, 0, (char *)_str);
    SDL_Rect size_rect;
    size_rect.x = 0;
    size_rect.y = 0;
    size_rect.w = _font_pixel_size * length;
    size_rect.h = _font_pixel_size;
    //再创建一个放大尺寸的贴图
    SDL_Texture *bigTexture = SDL_CreateTexture(_render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, size_rect.w, size_rect.h);
    if (_bg_transparent)
        SDL_SetTextureBlendMode(bigTexture, SDL_BLENDMODE_BLEND);
    //修改渲染目标为放大的贴图
    SDL_SetRenderTarget(_render, bigTexture);
    //把字符串贴到bigTexture
    SDL_RenderCopy(_render, texture, NULL, &size_rect);
    //修改渲染目标为屏幕
    SDL_SetRenderTarget(_render, NULL);
    //设置文字显示的坐标
    size_rect.x = _x;
    size_rect.y = _y - (_font_pixel_size * 0.5);
    //把文字贴到屏幕
    SDL_RenderCopy(_render, bigTexture, NULL, &size_rect);
    //释放内存
    SDL_DestroyTexture(texture);
    texture = NULL;
    SDL_DestroyTexture(bigTexture);
    bigTexture = NULL;
    CaoNiMa();
    */
}

//获取鼠标位置
void GetMousePosition(Vector *_pre_pos, Vector *_cur_pos, int _global)
{
    _pre_pos->x = _cur_pos->x;
    _pre_pos->y = _cur_pos->y;
    if (_global)
        SDL_GetGlobalMouseState(&_cur_pos->x, &_cur_pos->y);
    else
        SDL_GetMouseState(&_cur_pos->x, &_cur_pos->y);
}

//获取窗口的位置
void GetWindowPosition(Window_Struct *_window_struct)
{
    SDL_GetWindowPosition(_window_struct->window, &_window_struct->rect.x, &_window_struct->rect.y);
}

//初始化Layout布局，子窗口通用
//_win_struct：用于创建布局Texture
void InitLayout(Window_Struct _win_main, Layout *_layout, int _id, int _x, int _y, int _w, int _h, SDL_Color _bg_color)
{
    _layout->id = _id;
    _layout->activation_pre = 0;
    _layout->activation = 0;
    _layout->rect.x = _x;
    _layout->rect.y = _y;
    _layout->rect.w = _w;
    _layout->rect.h = _h;
    _layout->texture = SDL_CreateTexture(_win_main.render, SDL_PIXELFORMAT_RGBA4444, SDL_TEXTUREACCESS_STATIC, _w, _h);
    _layout->bg_color = _bg_color;
}

void UpdateLayoutActivation(Layout *_layout)
{
    _layout->activation_pre = _layout->activation;
    Vector p;
    p.x = mouse_position_cur_global.x - window_main.rect.x;
    p.y = mouse_position_cur_global.y - window_main.rect.y;
    _layout->activation = IsPointInRect(p.x, p.y, _layout->rect);
}

void UpdateButtonActivation(Button *_button)
{
    _button->activation_pre = _button->activation;
    Vector p;
    p.x = mouse_position_cur_global.x - window_main.rect.x;
    p.y = mouse_position_cur_global.y - window_main.rect.y;
    _button->activation = IsPointInRect(p.x, p.y, _button->rect);
}

//_relative_position:是否相对子窗口位置
void InitButton(Button *_button, int _id, char *_name, int _x, int _y, int _w, int _h, int _relative_position)
{
    _button->activation = 0;
    _button->activation_pre = 0;
    _button->id = _id;
    _button->name = _name;
    _button->rect.x = _x;
    _button->rect.y = _y;
    _button->rect.w = _w;
    _button->rect.h = _h;
    _button->color_normal = button_normal_color;
    _button->color_enter = button_enter_color;
    if (_relative_position)
    {
        _button->offset_to_parent.x = _button->rect.x;
        _button->offset_to_parent.y = _button->rect.y;
        _button->rect.x = window_sub.rect.x + _button->offset_to_parent.x;
        _button->rect.y = window_sub.rect.y + _button->offset_to_parent.y;
    }
}

//_x,_y是相对子窗口位置
void InitTextBox(Text_Box *_text_box, char *_content, int _x, int _y, int _w, int _h, int _font_size)
{
    _text_box->content = _content;
    _text_box->rect.x = _x;
    _text_box->rect.y = _y;
    _text_box->offset.x = _text_box->rect.x;
    _text_box->offset.y = _text_box->rect.y;
    _text_box->font_size = _font_size;
    _text_box->offset.x = _x;
    _text_box->offset.y = _y;
    _text_box->rect.x = window_sub.rect.x + _text_box->offset.x;
    _text_box->rect.y = window_sub.rect.y + _text_box->offset.y;
}

static void ShowButton(Window_Struct _win_struct, Button _button, int _font_size, int _with_border)
{
    SDL_Color col;
    col = button_enter_color;
    if (!_button.activation)
        col = button_normal_color;
    SDL_SetRenderDrawBlendMode(window_main.render, SDL_BLENDMODE_NONE);
    //绘制背景色
    SDL_SetRenderDrawColor(_win_struct.render, col.r, col.g, col.b, col.a);
    SDL_RenderFillRect(_win_struct.render, &_button.rect);
    //绘制文字
    ShowCharacter(_win_struct.render, _button.name, _font_size, _button.rect.x, _button.rect.y + _button.rect.h * 0.5, 0, 0, 0, 1);
    //绘制边框
    if (_with_border)
    {
        SDL_SetRenderDrawColor(_win_struct.render, 100, 100, 100, 255);
        SDL_RenderDrawRect(_win_struct.render, &_button.rect);
    }
    //绘制进入时的边框
    if (!_button.activation)
        return;
    SDL_SetRenderDrawColor(_win_struct.render, 50, 50, 50, 255);
    SDL_RenderDrawRect(_win_struct.render, &_button.rect);
}

static void DrawButtons(Button *_buttons, int _button_count, Uint8 _font_size, int _with_border)
{
    for (size_t i = 0; i < _button_count; i++)
    {
        ShowButton(window_main, _buttons[i], _font_size, _with_border);
    }
}

static void DrawWindowBgColor(SDL_Rect *_rect)
{
    SDL_SetRenderDrawColor(window_main.render, window_bg_color.r, window_bg_color.g, window_bg_color.b, window_bg_color.a);
    SDL_RenderFillRect(window_main.render, _rect);
}

static void DrawLayoutColor(Layout *_layouts, int _layout_count, SDL_Color _color)
{
    SDL_SetRenderDrawColor(window_main.render, _color.r, _color.g, _color.b, _color.a);
    for (size_t i = 0; i < _layout_count; i++)
    {
        SDL_RenderFillRect(window_main.render, &_layouts[i].rect);
    }

    SDL_SetRenderDrawColor(window_main.render, layout_border_color.r, layout_border_color.g, layout_border_color.b, layout_border_color.a);
    for (size_t i = 0; i < _layout_count; i++)
    {
        SDL_RenderDrawRect(window_main.render, &_layouts[i].rect);
    }
}

static void DrawTextBoxes(Text_Box *_text_boxes, int _text_box_count)
{
    for (size_t i = 0; i < _text_box_count; i++)
    {
        ShowCharacter(window_main.render, _text_boxes[i].content, _text_boxes[i].font_size, _text_boxes[i].rect.x, _text_boxes[i].rect.y + 5, 0, 0, 0, 1);
    }
}

static void DrawInputBox(Input_Box _input_box)
{
    //绘制背景
    SDL_SetRenderDrawColor(window_main.render, layout_bg_color.r, layout_bg_color.g, layout_bg_color.b, layout_bg_color.a);
    SDL_RenderFillRect(window_main.render, &_input_box.rect);
    //绘制边框
    SDL_SetRenderDrawColor(window_main.render, layout_border_color.r, layout_border_color.g, layout_border_color.b, layout_border_color.a);
    SDL_RenderDrawRect(window_main.render, &_input_box.rect);
    //绘制文字
    ShowCharacter(window_main.render, _input_box.content, FONT_SIZE_SUB_WIN_TIPS, _input_box.rect.x,
                  _input_box.rect.y + FONT_SIZE_SUB_WIN_TIPS, 0, 0, 0, 1);
}

static void DrawInputBoxes(Input_Box *_input_boxes, int _box_count)
{
    for (size_t i = 0; i < _box_count; i++)
    {
        //绘制背景
        // SDL_SetRenderDrawColor(window_main.render, layout_bg_color.r, layout_bg_color.g, layout_bg_color.b, layout_bg_color.a);
        SDL_SetRenderDrawColor(window_main.render, 200, 200, 200, 255);
        SDL_RenderFillRect(window_main.render, &_input_boxes[i].rect);
        //绘制边框
        SDL_SetRenderDrawColor(window_main.render, layout_border_color.r, layout_border_color.g, layout_border_color.b, layout_border_color.a);
        SDL_RenderDrawRect(window_main.render, &_input_boxes[i].rect);
        //绘制文字
        ShowCharacter(window_main.render, _input_boxes[i].content, FONT_SIZE_SUB_WIN_TIPS, _input_boxes[i].rect.x,
                      _input_boxes[i].rect.y + FONT_SIZE_SUB_WIN_TIPS, 0, 0, 0, 1);
    }
}

static void DrawProjectList()
{
    for (size_t i = 0; i < prj_dir_list.prj_count; i++)
    {
        //绘制背景色
        if (!prj_dir_list.buttons[i].activation)
            SDL_SetRenderDrawColor(window_main.render, prj_dir_list.buttons[i].color_normal.r, prj_dir_list.buttons[i].color_normal.g, prj_dir_list.buttons[i].color_normal.b, prj_dir_list.buttons[i].color_normal.a);
        else
            SDL_SetRenderDrawColor(window_main.render, prj_dir_list.buttons[i].color_enter.r, prj_dir_list.buttons[i].color_enter.g, prj_dir_list.buttons[i].color_enter.b, prj_dir_list.buttons[i].color_enter.a);
        SDL_RenderFillRect(window_main.render, &prj_dir_list.buttons[i].rect);
        //绘制文字
        int _font_size = prj_dir_list.buttons[i].rect.h - 4;
        if (_font_size > FONT_SIZE_LAYOUT_MENU_BUTTON)
            _font_size = FONT_SIZE_LAYOUT_MENU_BUTTON;
        int _font_x = prj_dir_list.buttons[i].rect.x + 20;
        int _space = prj_dir_list.buttons[i].rect.h - _font_size;
        int _font_y = prj_dir_list.buttons[i].rect.y + _space + _font_size * 0.5;
        ShowCharacter(window_main.render, prj_dir_list.buttons[i].name, _font_size, _font_x, _font_y, 0, 0, 0, 1);
        //绘制边框
        SDL_SetRenderDrawColor(window_main.render, layout_border_color.r, layout_border_color.g, layout_border_color.b, layout_border_color.a);
        SDL_RenderDrawRect(window_main.render, &prj_dir_list.buttons[i].rect);
    }
}

static void DrawTitle()
{
    ShowCharacter(window_main.render, window_sub.title.content, window_sub.title.font_size, window_sub.rect.x + 10, window_sub.rect.y + 15, 0, 0, 0, 1);
}

void InitSubWindow(Sub_Window *_sub_win, int _x, int _y, int _w, int _h)
{
    _sub_win->type = DEFAULT;
    _sub_win->open = 0;
    _sub_win->rect.x = _x;
    _sub_win->rect.y = _y;
    _sub_win->rect.w = _w;
    _sub_win->rect.h = _h;
    _sub_win->offset_to_mouse.x = -1;
    _sub_win->offset_to_mouse.y = -1;
    _sub_win->texture = SDL_CreateTexture(window_main.render, SDL_PIXELFORMAT_RGBA4444, SDL_TEXTUREACCESS_STATIC, _w, _h);
    _sub_win->button_count = -1;
    _sub_win->layout_count = 2;
    _sub_win->buttons = NULL;
    _sub_win->two_button = NULL;
    _sub_win->text_boxes = NULL;
    _sub_win->input_boxes = NULL;
}

//初始化输入框
void InitInputBox(Input_Box *_input_box, int _x, int _y, int _w, int _h, int _max_char_count, int _only_num, void *_target)
{
    _input_box->only_number = _only_num;
    _input_box->activation = 0;
    _input_box->activation_pre = 0;
    _input_box->curser_index = 0;
    _input_box->rect.x = _x,
    _input_box->rect.y = _y;
    _input_box->rect.w = _w;
    _input_box->rect.h = _h;
    _input_box->max_char_count = _max_char_count;
    _input_box->content = (char *)malloc(sizeof(char) * (_input_box->max_char_count));
    _input_box->content[0] = '\0';
    _input_box->content[_input_box->max_char_count] = '\0';
    _input_box->offset.x = _input_box->rect.x;
    _input_box->offset.y = _input_box->rect.y;
    _input_box->rect.x = window_sub.rect.x + _input_box->offset.x;
    _input_box->rect.y = window_sub.rect.y + _input_box->offset.y;
    _input_box->target = _target;
}

//把输入框的内容赋值给target目标，返回0代表成功，返回1代表字符串是空的，返回2代表路径存在，返回-1代表input_boxes是空指针
int SetInputBoxesContentToTarget()
{
    if (window_sub.input_boxes == NULL)
        return -1;
    for (size_t i = 0; i < window_sub.input_boxes_count; i++)
    {
        if (window_sub.input_boxes[i].only_number)
        {
            int num = CharStarToInt(window_sub.input_boxes[i].content);
            memcpy(window_sub.input_boxes[i].target, &num, sizeof(int));
        }
        else
        {
            //判断是不是空字符串
            if (window_sub.input_boxes[i].content[0] == '\0')
                return 1;

            //判断工程路径是否存在
            char *_path = (char *)malloc(sizeof(char) * PRJ_PATH_STR_LENGTH);
            for (size_t i = 0; i < PRJ_PATH_STR_LENGTH; i++)
            {
                _path[i] = '\0';
            }
            strcat(_path, PRJ_PATH);
            strcat(_path, window_sub.input_boxes[i].content);
            DIR *_dir = opendir(_path);
            if (_dir)
            {
                closedir(_dir);
                free(_dir);
                _dir = NULL;
                return 2;
            }
            else
            {
                free(cur_project_path);
                cur_project_path = (char *)malloc(sizeof(char) * PRJ_PATH_STR_LENGTH);
                memcpy(cur_project_path, _path, PRJ_PATH_STR_LENGTH);
            }
            closedir(_dir);
            free(_dir);
            _dir = NULL;
            free(_path);
            _path = NULL;

            for (size_t j = 0; j < window_sub.input_boxes[i].max_char_count; j++)
            {
                memcpy(window_sub.input_boxes[i].target, &window_sub.input_boxes[i].content[j], sizeof(char));
                window_sub.input_boxes[i].target++;
            }
            window_sub.input_boxes[i].target -= window_sub.input_boxes[i].max_char_count;
        }
    }
    return 0;
}

//系统目录不存在的话创建系统目录，0代表创建成功，-1代表创建失败
void CreateSystemFolder()
{
    DIR *dir = NULL;

    dir = opendir(PRJ_PATH);
    if (!dir)
    {
        int _res = mkdir("Project");
        if (_res == -1)
            StartWarning("\"./Project\" Folder Create Failed !");
    }
    closedir(dir);

    dir = opendir(TILE_PATH);
    if (!dir)
    {
        int _res = mkdir("Tile");
        if (_res == -1)
            StartWarning("\"./Tile\" Folder Create Failed !");
    }
    closedir(dir);

    // dir = opendir(EXPORT_PATH);
    // if (!dir)
    // {
    //     int _res = mkdir("Export");
    //     if (_res == -1)
    //     {
    //         StartWarning("\"./Export\" Folder Create Failed !");
    //         return;
    //     }
    // }
    closedir(dir);
    free(dir);
    dir = NULL;
}

//读取工程信息到工程列表，0代表成功，1代表没找到工程目录，2代表工程目录下没有工程
int ReadProjectInfo()
{
    DIR *dir = opendir(PRJ_PATH);
    if (!dir)
    {
        free(dir);
        dir = NULL;
        StartWarning("Not Found Project Folder !");
        return 1;
    }

    struct dirent *_dirent = NULL;

    //上一次工程数量
    int _tmp_prj_count = prj_dir_list.prj_count;
    //获取工程数量，未排除文件
    prj_dir_list.prj_count = 0;
    while (_dirent = readdir(dir))
    {
        prj_dir_list.prj_count++;
    }
    prj_dir_list.prj_count -= 2;

    if (prj_dir_list.prj_count == 0)
    {
        StartWarning("Not Found Project !");
        return 2;
    }

    //释放内存
    for (size_t i = 0; i < _tmp_prj_count; i++)
    {
        free(prj_dir_list.dir_name_list[i]);
        prj_dir_list.dir_name_list[i] = NULL;
    }

    free(prj_dir_list.dir_name_list);
    prj_dir_list.dir_name_list = NULL;

    //根据工程数量创建相同数量的字符串，每个字符串长度是MAX_STRING_LENGTH
    prj_dir_list.dir_name_list = (char **)malloc(sizeof(char *) * prj_dir_list.prj_count);
    for (size_t i = 0; i < prj_dir_list.prj_count; i++)
    {
        prj_dir_list.dir_name_list[i] = (char *)malloc(sizeof(char) * MAX_STRING_LENGTH);
        for (size_t j = 0; j < MAX_STRING_LENGTH; j++)
        {
            prj_dir_list.dir_name_list[i][j] = '\0';
        }
    }

    int index = 0;
    seekdir(dir, 0);
    //给字符串列表赋值
    while (_dirent = readdir(dir))
    {
        //前两个是没用的信息，从第三个开始
        if (index > 1)
        {
            int count = __min(_dirent->d_namlen, MAX_STRING_LENGTH);
            if (count == MAX_STRING_LENGTH)
                count = MAX_STRING_LENGTH - 1;
            for (size_t i = 0; i < count; i++)
            {
                prj_dir_list.dir_name_list[index - 2][i] = _dirent->d_name[i];
            }
            prj_dir_list.dir_name_list[index - 2][count + 1] = '\0';
        }
        index++;
    }

    closedir(dir);

    //释放内存
    free(_dirent);
    _dirent = NULL;
    free(dir);
    dir = NULL;

    //创建按钮
    free(prj_dir_list.buttons);
    prj_dir_list.buttons = NULL;
    prj_dir_list.buttons = (Button *)malloc(sizeof(Button) * prj_dir_list.prj_count);
    for (size_t i = 0; i < prj_dir_list.prj_count; i++)
    {
        prj_dir_list.buttons[i].activation = 0;
        prj_dir_list.buttons[i].activation_pre = 0;
        prj_dir_list.buttons[i].id = i;
        prj_dir_list.buttons[i].color_enter = button_enter_color;
        prj_dir_list.buttons[i].color_normal = button_normal_color;
        prj_dir_list.buttons[i].name = prj_dir_list.dir_name_list[i];
        prj_dir_list.buttons[i].rect.x = 50;
        int _h = layout_sub_win[1].rect.h / prj_dir_list.prj_count;
        _h = __min(_h, LAYOUT_SUB_WIN_0_HEIGHT);
        prj_dir_list.buttons[i].rect.h = _h;
        prj_dir_list.buttons[i].rect.y = LAYOUT_SUB_WIN_0_HEIGHT + _h * i;
        prj_dir_list.buttons[i].rect.w = layout_sub_win[1].rect.w - 50 * 2;
        prj_dir_list.buttons[i].offset_to_parent.x = prj_dir_list.buttons[i].rect.x;
        prj_dir_list.buttons[i].offset_to_parent.y = prj_dir_list.buttons[i].rect.y;
        prj_dir_list.buttons[i].rect.x = window_sub.rect.x + prj_dir_list.buttons[i].offset_to_parent.x;
        prj_dir_list.buttons[i].rect.y = window_sub.rect.y + prj_dir_list.buttons[i].offset_to_parent.y;
    }
    prj_dir_list.buttons[0].activation = 1;
    prj_dir_list.index = 0;
    return 0;
}

//获取工程目录
void GetProjectPathTotal()
{
    if (window_sub.type != OPEN)
        return;
    else if (prj_dir_list.prj_count <= 0)
        return;
    free(cur_project_path);
    cur_project_path = NULL;
    if (prj_dir_list.prj_count <= 0)
        return;
    cur_project_path = (char *)malloc(sizeof(char) * PRJ_PATH_STR_LENGTH);
    for (size_t i = 0; i < PRJ_PATH_STR_LENGTH; i++)
    {
        cur_project_path[i] = '\0';
    }
    strcat(cur_project_path, PRJ_PATH);
    strcat(cur_project_path, prj_name);
}

static void ShowWarning()
{
    ShowCharacter(window_main.render, warning, FONT_SIZE_SUB_WIN_TIPS,
                  460,
                  LAYOUT_SPACING + FONT_SIZE_SUB_WIN_TIPS * 0.5 + 8,
                  255, 0, 100,
                  0);
}

//绘制画笔选择的边框
static void DrawBrushSelect()
{
    if (!canvas_tile.texture || brush.draw_collision_box == 1)
        return;

    SDL_Rect tmp;
    tmp.x = brush.copyRect.x * *canvas_tile.scale;
    tmp.y = brush.copyRect.y * *canvas_tile.scale;
    tmp.w = brush.copyRect.w * *canvas_tile.scale;
    tmp.h = brush.copyRect.h * *canvas_tile.scale;
    tmp.x += canvas_tile.rect.x;
    tmp.y += canvas_tile.rect.y;
    Vector p1, p2;
    p1.x = __max(tmp.x, layout_editor[2].rect.x + 1);
    p1.y = __max(tmp.y, layout_editor[2].rect.y + 1);
    p2.x = __min(tmp.x + tmp.w, layout_editor[2].rect.x + layout_editor[2].rect.w - 1);
    p2.y = __min(tmp.y + tmp.h, layout_editor[2].rect.y + layout_editor[2].rect.h - 1);
    if (p1.x < p2.x && p1.y < p2.y)
    {
        SDL_Rect _rect;
        _rect.x = p1.x;
        _rect.y = p1.y;
        _rect.w = p2.x - p1.x;
        _rect.h = p2.y - p1.y;
        SDL_SetRenderDrawBlendMode(window_main.render, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(window_main.render, 0xff, 0xff, 0xff, 0x88);
        SDL_RenderFillRect(window_main.render, &_rect);
        SDL_SetRenderDrawBlendMode(window_main.render, SDL_BLENDMODE_NONE);
        SDL_SetRenderDrawColor(window_main.render, 0, 0xff, 0, 0xff);
        for (size_t i = 0; i < 2; i++)
        {
            _rect.x += i;
            _rect.y += i;
            _rect.w -= 2 * i;
            _rect.h -= 2 * i;
            SDL_RenderDrawRect(window_main.render, &_rect);
        }
    }
}

//绘制画笔的copy rect in map
void DrawBrushCopyRectInMap()
{
    if (c_box_lists.count == 0)
        return;
    if (brush.draw_collision_box == 0 || brush.startCopyInMap == 0)
        return;
    SDL_Rect tmp;
    tmp.x = brush.copyRectInMap.x * *canvas_map.scale;
    tmp.y = brush.copyRectInMap.y * *canvas_map.scale;
    tmp.w = brush.copyRectInMap.w * *canvas_map.scale;
    tmp.h = brush.copyRectInMap.h * *canvas_map.scale;
    tmp.x += canvas_map.rect.x;
    tmp.y += canvas_map.rect.y;
    Vector p1, p2;
    p1.x = __max(tmp.x, layout_editor[3].rect.x + 1);
    p1.y = __max(tmp.y, layout_editor[3].rect.y + 1);
    p2.x = __min(tmp.x + tmp.w, layout_editor[3].rect.x + layout_editor[3].rect.w - 1);
    p2.y = __min(tmp.y + tmp.h, layout_editor[3].rect.y + layout_editor[3].rect.h - 1);
    if (p1.x < p2.x && p1.y < p2.y)
    {
        SDL_Rect _rect;
        _rect.x = p1.x;
        _rect.y = p1.y;
        _rect.w = p2.x - p1.x;
        _rect.h = p2.y - p1.y;
        SDL_Color col = c_box_lists.lists[c_box_lists.index]->color;
        SDL_SetRenderDrawBlendMode(window_main.render, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(window_main.render, col.r, col.g, col.b, 0x88);
        SDL_RenderFillRect(window_main.render, &_rect);
        SDL_SetRenderDrawBlendMode(window_main.render, SDL_BLENDMODE_NONE);
        SDL_SetRenderDrawColor(window_main.render, col.r, col.g, col.b, 0xff);
        for (size_t i = 0; i < 2; i++)
        {
            _rect.x += i;
            _rect.y += i;
            _rect.w -= 2 * i;
            _rect.h -= 2 * i;
            SDL_RenderDrawRect(window_main.render, &_rect);
        }
    }
}

//在map上绘制画笔边框
void DrawBrushBorderInMap()
{
    if (brush.draw_collision_box)
        return;
    if (!layout_editor[3].activation || !canvas_tile.texture || can_grab_layout)
        return;

    SDL_SetRenderDrawColor(window_main.render, 0, 0xff, 0, 0xff);
    SDL_Rect tmp;
    tmp.x = canvas_map.rect.x + brush.posInMap.x * *canvas_map.scale;
    tmp.y = canvas_map.rect.y + brush.posInMap.y * *canvas_map.scale;
    tmp.w = brush.posInMap.w * *canvas_map.scale;
    tmp.h = brush.posInMap.h * *canvas_map.scale;
    SDL_Texture *brush_texture = SDL_CreateTexture(window_main.render,
                                                   SDL_PIXELFORMAT_RGBA8888,
                                                   SDL_TEXTUREACCESS_TARGET,
                                                   brush.copyRect.w * *canvas_map.scale,
                                                   brush.copyRect.h * *canvas_map.scale);
    SDL_SetRenderTarget(window_main.render, brush_texture);
    SDL_RenderCopy(window_main.render, canvas_tile.texture, &brush.copyRect, NULL);

    SDL_SetRenderTarget(window_main.render, NULL);
    SDL_SetTextureBlendMode(brush_texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(brush_texture, 0xff);

    Vector p1, p2;
    p1.x = __max(tmp.x, layout_editor[3].rect.x + 1);
    p1.y = __max(tmp.y, layout_editor[3].rect.y + 1);
    p2.x = __min(tmp.x + tmp.w, layout_editor[3].rect.x + layout_editor[3].rect.w - 1);
    p2.y = __min(tmp.y + tmp.h, layout_editor[3].rect.y + layout_editor[3].rect.h - 1);

    if (p1.x > p2.x || p1.y > p2.y)
    {
        SDL_DestroyTexture(brush_texture);
        brush_texture = NULL;
        SDL_SetRenderDrawBlendMode(window_main.render, SDL_BLENDMODE_NONE);
        return;
    }

    SDL_Rect dst;
    dst.x = p1.x;
    dst.y = p1.y;
    dst.w = p2.x - p1.x;
    dst.h = p2.y - p1.y;

    SDL_Rect src;
    src.x = dst.x - tmp.x;
    src.y = dst.y - tmp.y;
    src.w = dst.w;
    src.h = dst.h;

    SDL_RenderCopy(window_main.render, brush_texture, &src, &dst);

    for (size_t i = 0; i < 2; i++)
    {
        SDL_RenderDrawRect(window_main.render, &dst);
        dst.x++;
        dst.y++;
        dst.w -= 2;
        dst.h -= 2;
    }

    SDL_SetRenderDrawBlendMode(window_main.render, SDL_BLENDMODE_NONE);
    SDL_DestroyTexture(brush_texture);
    brush_texture = NULL;
}

//渲染队列
void RenderQueue()
{
    if (is_refresh_window)
    {
        DrawWindowBgColor(NULL);
        DrawLayoutColor(layout_editor, LAYOUT_EDITOR_COUNT, layout_bg_color);
        DrawPalette();
        DrawCanvas(canvas_map, 3, 1);
        if (brush.draw_collision_box == 0)
            DrawCanvas(canvas_tile, 2, 0);

        CaoNiMa(); //==============
        DrawButtons(button_menu, BUTTON_MENU_COUNT, FONT_SIZE_LAYOUT_MENU_BUTTON, 0);
        DrawButtons(button_tile, 2, FONT_SIZE_LAYOUT_MENU_BUTTON, 1);
        DrawBrushSelect();
        ShowCollisionBoxes();
        ShowHilightCollisionBox();
        DrawBrushCopyRectInMap();
        DrawBrushBorderInMap();
        BrushDraw();
        DrawButtons(brush_mode_button, 2, FONT_SIZE_SUB_WIN_TIPS, 1);
        ShowCollisionBoxEditor();

        if (window_sub.open)
        {
            DrawWindowBgColor(&window_sub.rect);
            DrawLayoutColor(layout_sub_win, LAYOUT_COUNT_SUB_WIN, window_bg_color);
            DrawButtons(window_sub.two_button, 2, FONT_SIZE_OK_BUTTON, 1);
            DrawTitle();
            if (window_sub.text_boxes)
                DrawTextBoxes(window_sub.text_boxes, window_sub.text_boxes_count);
            if (window_sub.buttons)
                DrawButtons(window_sub.buttons, window_sub.button_count, FONT_SIZE_LAYOUT_MENU_BUTTON, 1);
            if (window_sub.input_boxes)
                DrawInputBoxes(window_sub.input_boxes, window_sub.input_boxes_count);
            if (window_sub.type == OPEN && prj_dir_list.prj_count > 0)
                DrawProjectList();
        }

        if (show_warning)
            ShowWarning();

        SDL_RenderPresent(window_main.render);
        is_refresh_window = 0;
    }
}

#endif