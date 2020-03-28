#ifndef _COLLISION_BOX_C_
#define _COLLISION_BOX_C_

#include <SDL2/SDL.h>
#include <stdio.h>
#include <time.h>
#include "attribute.c"
#include "my_math.c"

//显示一个碰撞盒
static void ShowACollisionBox(SDL_Rect _c_box, Uint8 _r, Uint8 _g, Uint8 _b)
{
    if (canvas_map.texture == NULL)
        return;
    SDL_Rect src_rec;

    src_rec.x = _c_box.x * *canvas_map.scale;
    src_rec.y = _c_box.y * *canvas_map.scale;
    src_rec.w = _c_box.w * *canvas_map.scale;
    src_rec.h = _c_box.h * *canvas_map.scale;
    src_rec.x += canvas_map.rect.x;
    src_rec.y += canvas_map.rect.y;

    Vector point1, point2;
    point1.x = __max(src_rec.x, layout_editor[3].rect.x + 1);
    point1.y = __max(src_rec.y, layout_editor[3].rect.y + 1);
    point2.x = __min(src_rec.x + src_rec.w, layout_editor[3].rect.x + layout_editor[3].rect.w - 1);
    point2.y = __min(src_rec.y + src_rec.h, layout_editor[3].rect.y + layout_editor[3].rect.h - 1);
    if (point1.x >= point2.x || point1.y >= point2.y)
        return;
    SDL_Rect rect;
    rect.x = point1.x;
    rect.y = point1.y;
    rect.w = point2.x - point1.x;
    rect.h = point2.y - point1.y;

    SDL_Color color;
    color.r = _r;
    color.g = _g;
    color.b = _b;
    color.a = 0xff;
    SDL_SetRenderDrawColor(window_main.render, color.r, color.g, color.b, color.a);
    for (size_t i = 0; i < 2; i++)
    {
        SDL_RenderDrawRect(window_main.render, &rect);
        rect.x++;
        rect.y++;
        rect.w -= 2;
        rect.h -= 2;
    }
    SDL_SetRenderDrawBlendMode(window_main.render, SDL_BLENDMODE_BLEND);
    color.a = 0x77;
    SDL_SetRenderDrawColor(window_main.render, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(window_main.render, &rect);
    SDL_SetRenderDrawBlendMode(window_main.render, SDL_BLENDMODE_NONE);
}

//高亮显示一个碰撞盒
static void ShowAHilightCollisionBox(SDL_Rect _c_box, Uint8 _r, Uint8 _g, Uint8 _b)
{
    if (canvas_map.texture == NULL)
        return;
    SDL_Rect src_rec;

    src_rec.x = _c_box.x * *canvas_map.scale + 2;
    src_rec.y = _c_box.y * *canvas_map.scale + 2;
    src_rec.w = _c_box.w * *canvas_map.scale - 4;
    src_rec.h = _c_box.h * *canvas_map.scale - 4;
    src_rec.x += canvas_map.rect.x;
    src_rec.y += canvas_map.rect.y;

    Vector point1, point2;
    point1.x = __max(src_rec.x, layout_editor[3].rect.x + 1);
    point1.y = __max(src_rec.y, layout_editor[3].rect.y + 1);
    point2.x = __min(src_rec.x + src_rec.w, layout_editor[3].rect.x + layout_editor[3].rect.w - 1);
    point2.y = __min(src_rec.y + src_rec.h, layout_editor[3].rect.y + layout_editor[3].rect.h - 1);
    if (point1.x >= point2.x || point1.y >= point2.y)
        return;
    SDL_Rect rect;
    rect.x = point1.x;
    rect.y = point1.y;
    rect.w = point2.x - point1.x;
    rect.h = point2.y - point1.y;

    SDL_Color color;
    color.r = _r;
    color.g = _g;
    color.b = _b;
    color.a = 0xff;
    SDL_SetRenderDrawColor(window_main.render, color.r, color.g, color.b, color.a);
    for (size_t i = 0; i < 2; i++)
    {
        SDL_RenderDrawRect(window_main.render, &rect);
        rect.x++;
        rect.y++;
        rect.w -= 2;
        rect.h -= 2;
    }
}

//渲染队列里高亮显示碰撞盒
void ShowHilightCollisionBox()
{
    if (c_box_lists.count == 0 || brush.draw_collision_box == 0)
        return;
    if (c_box_lists.lists[c_box_lists.index]->count == 0)
        return;
    SDL_Rect hi_rect = *(c_box_lists.lists[c_box_lists.index]->rects[c_box_lists.lists[c_box_lists.index]->index]);
    ShowAHilightCollisionBox(hi_rect, 0xff, 0xff, 0xff);
}

//_tmp：字的位置，_index：碰撞盒索引
static void ShowBoxData(SDL_Rect _tmp, int _index)
{
    char num[6];
    sprintf(num, "%d", _index);
    int font_size = tile_size / 2 * *canvas_map.scale;
    _tmp.x *= *canvas_map.scale;
    _tmp.y *= *canvas_map.scale;
    _tmp.x += canvas_map.rect.x;
    _tmp.y += canvas_map.rect.y;
    if (_tmp.x >= layout_editor[3].rect.x && _tmp.x < layout_editor[3].rect.x + layout_editor[3].rect.w - 1 &&
        _tmp.y + font_size * 0.5 > layout_editor[3].rect.y && _tmp.y + font_size * 0.5 < layout_editor[3].rect.y + layout_editor[3].rect.h - 1)
    {
        SDL_SetRenderDrawColor(window_main.render, 0xff, 0xff, 0xff, 0xff);
        SDLTest_DrawString(window_main.render, _tmp.x, _tmp.y, num);
    }
}

//显示所有碰撞盒
void ShowCollisionBoxes()
{
    if (brush.draw_collision_box == 0 || c_box_lists.count == 0)
        return;
    SDL_Rect _tmp;
    //当前被选中的索引
    int index = c_box_lists.index;

    //不solo绘制这些
    if (c_box_lists.solo == 0)
    {
        for (size_t i = 0; i < c_box_lists.count; i++)
        {
            if (i == index)
                continue;
            for (size_t j = 0; j < c_box_lists.lists[i]->count; j++)
            {
                _tmp.x = c_box_lists.lists[i]->rects[j]->x;
                _tmp.y = c_box_lists.lists[i]->rects[j]->y;
                _tmp.w = c_box_lists.lists[i]->rects[j]->w;
                _tmp.h = c_box_lists.lists[i]->rects[j]->h;
                ShowACollisionBox(_tmp,
                                  c_box_lists.lists[i]->color.r,
                                  c_box_lists.lists[i]->color.g,
                                  c_box_lists.lists[i]->color.b);
                ShowBoxData(_tmp, j);
            }
        }
    }
    //不管solo不solo，都要绘制当前选中的index
    for (size_t i = 0; i < c_box_lists.lists[index]->count; i++)
    {
        _tmp.x = c_box_lists.lists[index]->rects[i]->x;
        _tmp.y = c_box_lists.lists[index]->rects[i]->y;
        _tmp.w = c_box_lists.lists[index]->rects[i]->w;
        _tmp.h = c_box_lists.lists[index]->rects[i]->h;
        ShowACollisionBox(_tmp,
                          c_box_lists.lists[index]->color.r,
                          c_box_lists.lists[index]->color.g,
                          c_box_lists.lists[index]->color.b);
        ShowBoxData(_tmp, i);
    }
}

//获取map中当前被选中的碰撞盒索引，1：刷新屏幕，0不刷新
void GetACollisionBoxListIndex()
{
    if (brush.draw_collision_box == 0 || c_box_lists.count == 0 || layoutIndex != 3)
        return;
    if (mouse_position_cur_local.x == mouse_position_pre_local.x &&
        mouse_position_cur_local.y == mouse_position_pre_local.y)
        return;
    SDL_Rect _tmp;
    //当前被选中的索引
    int index = c_box_lists.index;
    int pre_index = c_box_lists.lists[index]->index;
    for (size_t i = 0; i < c_box_lists.lists[index]->count; i++)
    {
        _tmp.x = c_box_lists.lists[index]->rects[i]->x;
        _tmp.y = c_box_lists.lists[index]->rects[i]->y;
        _tmp.w = c_box_lists.lists[index]->rects[i]->w;
        _tmp.h = c_box_lists.lists[index]->rects[i]->h;
        _tmp.x *= *canvas_map.scale;
        _tmp.y *= *canvas_map.scale;
        _tmp.w *= *canvas_map.scale;
        _tmp.h *= *canvas_map.scale;
        _tmp.x += canvas_map.rect.x;
        _tmp.y += canvas_map.rect.y;
        if (IsPointInRect(mouse_position_cur_local.x, mouse_position_cur_local.y, _tmp))
        {
            c_box_lists.lists[index]->index = i;
            break;
        }
    }
    if (pre_index != c_box_lists.lists[index]->index)
        is_refresh_window = 1;
}

//初始化碰撞盒列表，256种
int InitCollisionBoxList()
{
    c_box_lists.lists = (C_Box_List **)malloc(sizeof(C_Box_List *) * MAX_COLLISION_BOX_KIND);
    if (c_box_lists.lists == NULL)
        return -1;
    for (size_t i = 0; i < MAX_COLLISION_BOX_KIND; i++)
        c_box_lists.lists[i] = NULL;
    return 0;
}

//释放c_box_lists.lists
void FreeCollisionBoxLists()
{
    if (c_box_lists.count > 0)
    {
        for (int i = 0; i < c_box_lists.count; i++)
        {
            if (c_box_lists.lists[i])
            {
                for (int j = 0; j < c_box_lists.lists[i]->count; j++)
                {
                    free(c_box_lists.lists[i]->rects[j]);
                    c_box_lists.lists[i]->rects[j] = NULL;
                }
                free(c_box_lists.lists[i]);
                c_box_lists.lists[i] = NULL;
            }
        }
        c_box_lists.count = 0;
        c_box_lists.index = 0;
        c_box_lists.solo = 0;
        is_refresh_window = 1;
    }
}

//创建一个种类的碰撞盒指针，并初始化碰撞盒的MAX_COLLISION_BOX_COUNT_ONE_KIND个rect指针，赋值为NULL
//-1：列表已经满了，-2：数值重复了，-3：申请内存失败，-4：申请内存失败
int CreateCollisionBoxList(char _value)
{
    if (c_box_lists.count + 1 > MAX_COLLISION_BOX_KIND)
        return -1;
    if (c_box_lists.count > 0)
    {
        for (int i = 0; i < c_box_lists.count; i++)
        {
            if (c_box_lists.lists[i]->value == _value)
                return -2;
        }
    }
    c_box_lists.lists[c_box_lists.count] = (C_Box_List *)malloc(sizeof(C_Box_List));
    if (c_box_lists.lists[c_box_lists.count] == NULL)
        return -3;
    SDL_Color col;
    if (c_box_lists.count == 0)
        time(&seed);
    //3791
    seed += 128;
    srand(seed);
    col.r = rand() % 256;
    col.g = rand() % 256;
    col.b = rand() % 256;
    col.a = 0xff;

    c_box_lists.lists[c_box_lists.count]->color.r = col.r;
    c_box_lists.lists[c_box_lists.count]->color.g = col.g;
    c_box_lists.lists[c_box_lists.count]->color.b = col.b;
    c_box_lists.lists[c_box_lists.count]->color.a = col.a;
    c_box_lists.lists[c_box_lists.count]->count = 0;
    c_box_lists.lists[c_box_lists.count]->value = _value;
    c_box_lists.lists[c_box_lists.count]->rects = (SDL_Rect **)malloc(sizeof(SDL_Rect *) * MAX_COLLISION_BOX_COUNT_ONE_KIND);
    if (c_box_lists.lists[c_box_lists.count]->rects == NULL)
        return -4;
    for (size_t i = 0; i < MAX_COLLISION_BOX_COUNT_ONE_KIND; i++)
        c_box_lists.lists[c_box_lists.count]->rects[i] = NULL;
    c_box_lists.count++;
    c_box_lists.index = c_box_lists.count - 1;
    project_changed = 1;
    return 0;
}

//删除一种类型的碰撞盒
static int DestroyCollisionBoxList(int _index)
{
    if (_index < 0 || _index > MAX_COLLISION_BOX_KIND - 1)
        return -1;
    if (c_box_lists.lists[_index] == NULL)
        return -2;

    C_Box_List *tmp = c_box_lists.lists[_index];

    for (size_t i = _index; i < c_box_lists.count; i++)
    {
        if (i == c_box_lists.count - 1)
        {
            c_box_lists.lists[i] = NULL;
            break;
        }
        c_box_lists.lists[i] = c_box_lists.lists[i + 1];
    }

    for (size_t i = 0; i < tmp->count; i++)
    {
        free(tmp->rects[i]);
        tmp->rects[i] = NULL;
    }
    free(tmp->rects);
    tmp->rects = NULL;
    free(tmp);
    c_box_lists.count--;
    c_box_lists.index = __min(c_box_lists.index, c_box_lists.count - 1);
    c_box_lists.index = __max(c_box_lists.index, 0);
    project_changed = 1;
    return 0;
}

//给列表添加一个碰撞盒，返回0代表成功
int AddACollisionBox(C_Box_List *_list, int _x, int _y, int _w, int _h)
{
    if (_list == NULL)
        return -1;
    if (_list->count + 1 > MAX_COLLISION_BOX_COUNT_ONE_KIND)
        return -2;
    _list->rects[_list->count] = (SDL_Rect *)malloc(sizeof(SDL_Rect));
    if (_list->rects[_list->count] == NULL)
        return -3;
    _list->rects[_list->count]->x = _x;
    _list->rects[_list->count]->y = _y;
    _list->rects[_list->count]->w = _w;
    _list->rects[_list->count]->h = _h;
    _list->count++;
    _list->index = _list->count - 1;
    project_changed = 1;
    return 0;
}

//从列表删除一个碰撞盒
int DelACollisionBox(C_Box_List *_list, int _index)
{
    if (_list == NULL)
        return -1;
    if (_index < 0 || _index > MAX_COLLISION_BOX_COUNT_ONE_KIND - 1)
        return -2;
    if (_list->count == 0)
        return -3;
    free(_list->rects[_index]);
    if (_index < _list->count - 1)
    {
        for (size_t i = _index; i < _list->count - 1; i++)
            _list->rects[i] = _list->rects[i + 1];
        _list->rects[_list->count - 1] = NULL;
    }
    else if (_index == _list->count - 1)
        _list->index--;
    _list->count--;
    project_changed = 1;
    return 0;
}

//显示碰撞框编辑区
void ShowCollisionBoxEditor()
{
    if (c_box_lists.count == 0)
        return;
    if (brush.draw_collision_box == 0 || canvas_map.texture == NULL)
        return;

    int h_count = BOXCOLLISION_EDITOR_H_COUNT;
    int v_count = BOXCOLLISION_EDITOR_V_COUNT;
    float size_w = (layout_editor[2].rect.w + FLOAT) / h_count;
    float size_h = (layout_editor[2].rect.h + FLOAT) / v_count;
    int x = 0, y = 0;
    SDL_Rect rect;
    for (size_t i = 0; i < c_box_lists.count; i++)
    {
        SDL_Color col = c_box_lists.lists[i]->color;
        SDL_SetRenderDrawColor(window_main.render, col.r, col.g, col.b, 0xff);
        rect.x = x * size_w;
        rect.y = y * size_h;
        rect.w = size_w;
        rect.h = size_h;
        rect.x += layout_editor[2].rect.x + 1;
        rect.y += layout_editor[2].rect.y + 1;
        SDL_RenderFillRect(window_main.render, &rect);
        SDL_SetRenderDrawColor(window_main.render, window_bg_color.r, window_bg_color.g, window_bg_color.b, window_bg_color.a);
        SDL_RenderDrawRect(window_main.render, &rect);
        //显示文字背景
        SDL_SetRenderDrawColor(window_main.render, 0xff, 0xff, 0xff, 0xff);
        SDL_Rect bg_rect;
        if (c_box_lists.solo && c_box_lists.index == i)
        {
            bg_rect.x = rect.x + 4;
            bg_rect.y = rect.y + 4;
            bg_rect.w = 33;
            bg_rect.h = 9;
            SDL_RenderFillRect(window_main.render, &bg_rect);
        }
        bg_rect.x = rect.x + size_w * 0.4 - 1;
        bg_rect.y = rect.y + size_h * 0.4 - 1;
        bg_rect.w = 9;
        bg_rect.h = 9;
        SDL_RenderFillRect(window_main.render, &bg_rect);
        //显示数值
        SDL_SetRenderDrawColor(window_main.render, 0, 0, 0, 0xff);
        SDLTest_DrawCharacter(window_main.render, rect.x + size_w * 0.4, rect.y + size_h * 0.4, c_box_lists.lists[i]->value);
        //显示solo标识
        if (c_box_lists.solo && c_box_lists.index == i)
            SDLTest_DrawString(window_main.render, rect.x + 5, rect.y + 5, "Solo");
        //高亮显示
        if (c_box_lists.index == i)
        {
            SDL_SetRenderDrawColor(window_main.render, 0xff, 0xff, 0xff, 0xff);
            for (size_t i = 0; i < 4; i++)
            {
                SDL_RenderDrawRect(window_main.render, &rect);
                rect.x++;
                rect.y++;
                rect.w -= 2;
                rect.h -= 2;
            }
        }
        x++;
        if (x >= h_count)
        {
            x = 0;
            y++;
        }
    }
}

//选择c_box_lists.index
void SelectCollisionBoxListsIndex()
{
    if (c_box_lists.count == 0)
        return;
    if (brush.draw_collision_box == 0 || canvas_map.texture == NULL)
        return;

    int h_count = BOXCOLLISION_EDITOR_H_COUNT;
    int v_count = BOXCOLLISION_EDITOR_V_COUNT;
    float size_w = (layout_editor[2].rect.w + FLOAT) / h_count;
    float size_h = (layout_editor[2].rect.h + FLOAT) / v_count;
    int x = 0, y = 0;
    SDL_Rect rect;
    for (size_t i = 0; i < c_box_lists.count; i++)
    {
        rect.x = x * size_w;
        rect.y = y * size_h;
        rect.w = size_w;
        rect.h = size_h;
        rect.x += layout_editor[2].rect.x + 1;
        rect.y += layout_editor[2].rect.y + 1;

        if (IsPointInRect(mouse_position_cur_local.x, mouse_position_cur_local.y, rect))
        {
            c_box_lists.index = i;
            break;
        }

        x++;
        if (x >= h_count)
        {
            x = 0;
            y++;
        }
    }
}

//删除选中的碰撞盒列表
void DestroySelectedCollisionBoxLists()
{
    if (c_box_lists.count == 0)
        return;
    if (brush.draw_collision_box == 0 || canvas_map.texture == NULL)
        return;

    int h_count = BOXCOLLISION_EDITOR_H_COUNT;
    int v_count = BOXCOLLISION_EDITOR_V_COUNT;
    float size_w = (layout_editor[2].rect.w + FLOAT) / h_count;
    float size_h = (layout_editor[2].rect.h + FLOAT) / v_count;
    int x = 0, y = 0;
    SDL_Rect rect;
    for (size_t i = 0; i < c_box_lists.count; i++)
    {
        rect.x = x * size_w;
        rect.y = y * size_h;
        rect.w = size_w;
        rect.h = size_h;
        rect.x += layout_editor[2].rect.x + 1;
        rect.y += layout_editor[2].rect.y + 1;

        if (IsPointInRect(mouse_position_cur_local.x, mouse_position_cur_local.y, rect) &&
            c_box_lists.index == i)
        {
            DestroyCollisionBoxList(c_box_lists.index);
            break;
        }

        x++;
        if (x >= h_count)
        {
            x = 0;
            y++;
        }
    }
}

//solo显示被选中的碰撞盒列表
void SoloSelectedCollisionBoxLists()
{
    if (c_box_lists.count == 0)
        return;
    if (brush.draw_collision_box == 0 || canvas_map.texture == NULL)
        return;

    int h_count = BOXCOLLISION_EDITOR_H_COUNT;
    int v_count = BOXCOLLISION_EDITOR_V_COUNT;
    float size_w = (layout_editor[2].rect.w + FLOAT) / h_count;
    float size_h = (layout_editor[2].rect.h + FLOAT) / v_count;
    int x = 0, y = 0;
    SDL_Rect rect;
    for (size_t i = 0; i < c_box_lists.count; i++)
    {
        rect.x = x * size_w;
        rect.y = y * size_h;
        rect.w = size_w;
        rect.h = size_h;
        rect.x += layout_editor[2].rect.x + 1;
        rect.y += layout_editor[2].rect.y + 1;

        if (IsPointInRect(mouse_position_cur_local.x, mouse_position_cur_local.y, rect))
        {
            if (c_box_lists.index == i)
            {
                c_box_lists.solo++;
                c_box_lists.solo %= 2;
            }
            else
            {
                c_box_lists.index = i;
                c_box_lists.solo = 1;
            }
        }

        x++;
        if (x >= h_count)
        {
            x = 0;
            y++;
        }
    }
}

//碰撞框模式时绘制灰色透明遮罩在Map上
void CollisionBoxBlackMaskInMap(Canvas _canvas, SDL_Rect _dstPosition)
{
    if (_canvas.ID == 3 && brush.draw_collision_box)
    {
        SDL_SetRenderDrawBlendMode(window_main.render, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(window_main.render, 0, 0, 0, 0xBA);
        SDL_RenderFillRect(window_main.render, &_dstPosition);
        SDL_SetRenderDrawBlendMode(window_main.render, SDL_BLENDMODE_NONE);
    }
}

#endif