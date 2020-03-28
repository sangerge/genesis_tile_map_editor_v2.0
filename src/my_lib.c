#ifndef _MY_LIB_C_
#define _MY_LIB_C_

#include <SDL2/SDL.h>
#include <stdio.h>
#include "attribute.c"
#include "warning.c"
#include "my_math.c"
#include "collision_box.c"

void ShowCharacter(SDL_Renderer *_render, char *_str, Uint8 _font_pixel_size, int _x, int _y, Uint8 _r, Uint8 _g, Uint8 _b, int _bg_transparent);
void CaoNiMa();

//绘制画布边框
static void DrawCanvasBorder(SDL_Rect _dstposition)
{
    SDL_Rect _rect;
    _rect.x = _dstposition.x;
    _rect.y = _dstposition.y;
    _rect.w = _dstposition.w + 1;
    _rect.h = _dstposition.h + 1;
    SDL_SetRenderDrawColor(window_main.render, 0, 0, 0xff, 0xff);
    SDL_RenderDrawRect(window_main.render, &_rect);
}

//载入Tile
void LoadTile()
{
    if (prj_name[0] == '\0')
    {
        StartWarning("Create Project First !");
        return;
    }
    FILE *fp = fopen("./Tile/tile.bmp", "r");
    if (!fp)
    {
        StartWarning("\"./Tile/tile.bmp\" Not Found !");
        return;
    }
    fclose(fp);
    free(fp);
    fp = NULL;
    if (canvas_tile.surface)
    {
        SDL_FreeSurface(canvas_tile.surface);
        canvas_tile.surface = NULL;
    }
    canvas_tile.surface = SDL_LoadBMP("./Tile/tile.bmp");
    if (canvas_tile.surface == NULL)
    {
        StartWarning("Load \"tile.bmp\" Failed !");
        return;
    }
    else if (canvas_tile.surface->w % tile_size != 0 || canvas_tile.surface->h % tile_size != 0)
    {
        StartWarning("BMP size must be a multiple of 'tile_size' !");
        SDL_FreeSurface(canvas_tile.surface);
        canvas_tile.surface = NULL;
        return;
    }
    else if (canvas_tile.surface->w > MAX_CANVAS_SIZE || canvas_tile.surface->h > MAX_CANVAS_SIZE)
    {
        StartWarning("Max tile size is 8000 x 8000 pixel !!");
        SDL_FreeSurface(canvas_tile.surface);
        canvas_tile.surface = NULL;
        return;
    }
    //
    //8bpp测试
    if (canvas_tile.surface->format->BitsPerPixel != 8)
    {
        //tile.bmp必须是8bpp,64色模式
        StartWarning("tile.bmp must be 8bpp and less than 65 colors.");
        return;
    }

    //检测调色板颜色数
    int color_count = canvas_tile.surface->format->palette->ncolors;
    if (color_count > MAX_PALETTE_COLOR)
    {
        StartWarning("Palette max colors is 64 !!");
        SDL_FreeSurface(canvas_tile.surface);
        canvas_tile.surface = NULL;
        return;
    }

    //获取调色板
    if (pal)
    {
        SDL_FreePalette(pal);
        pal = NULL;
    }
    pal = (SDL_Palette *)malloc(sizeof(SDL_Palette));
    pal->colors = (SDL_Color *)malloc(sizeof(SDL_Color) * color_count);
    pal->ncolors = color_count;
    pal->refcount = canvas_tile.surface->format->palette->refcount;
    pal->version = canvas_tile.surface->format->palette->version;

    for (size_t i = 0; i < color_count; i++)
        pal->colors[i] = canvas_tile.surface->format->palette->colors[i];

    //=====================================
    canvas_tile.scale = &scale_data[0];
    canvas_tile.rect.x = layout_editor[2].rect.x + 1;
    canvas_tile.rect.y = layout_editor[2].rect.y + 1;
    canvas_tile.h_count = canvas_tile.surface->w / tile_size;
    canvas_tile.v_count = canvas_tile.surface->h / tile_size;
    canvas_tile.rect.w = canvas_tile.h_count * tile_size * *canvas_tile.scale;
    canvas_tile.rect.h = canvas_tile.v_count * tile_size * *canvas_tile.scale;
    if (canvas_tile.texture)
    {
        SDL_DestroyTexture(canvas_tile.texture);
        canvas_tile.texture = NULL;
    }
    //生成贴图
    SDL_Texture *tmp_texture = SDL_CreateTextureFromSurface(window_main.render, canvas_tile.surface);

    canvas_tile.texture = SDL_CreateTexture(window_main.render,
                                            SDL_PIXELFORMAT_RGBA8888,
                                            SDL_TEXTUREACCESS_TARGET,
                                            canvas_tile.h_count * tile_size,
                                            canvas_tile.v_count * tile_size);
    SDL_SetTextureBlendMode(canvas_tile.texture, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(window_main.render, canvas_tile.texture);
    SDL_RenderCopy(window_main.render, tmp_texture, NULL, NULL);
    pitch = canvas_tile.h_count * tile_size * sizeof(Uint32);
    free(pixels);
    pixels = (Uint32 *)malloc(sizeof(Uint32) * canvas_tile.h_count * tile_size * canvas_tile.v_count * tile_size);
    SDL_RenderReadPixels(window_main.render, NULL, SDL_PIXELFORMAT_RGBA8888, pixels, pitch);
    SDL_SetRenderTarget(window_main.render, NULL);
    SDL_DestroyTexture(tmp_texture);
    tmp_texture = NULL;

    //用完surface删除
    if (canvas_tile.surface)
    {
        SDL_FreeSurface(canvas_tile.surface);
        canvas_tile.surface = NULL;
    }

    dynamicMapSize.x = tile_size;
    dynamicMapSize.y = tile_size;
    open_dynamic_grid = 1;
    open_dynamic_grid_pre = 1;
    project_changed = 1;

    StartWarning("tile load success !!");

    //测试pixels和pitch能不能正常工作
    // SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormatFrom(pixels,
    //                                                           canvas_tile.h_count * tile_size,
    //                                                           canvas_tile.v_count * tile_size,
    //                                                           8,
    //                                                           pitch,
    //                                                           SDL_PIXELFORMAT_RGBA8888);
    // if (surface)
    // {
    //     SDL_SaveBMP(surface, "lalala.bmp");
    //     SDL_FreeSurface(surface);
    //     surface = NULL;
    // }
}
//重置画布
void FreeCanvas(Canvas *_canvas)
{
    //清除surface
    if (_canvas->surface)
    {
        SDL_FreeSurface(_canvas->surface);
        _canvas->surface = NULL;
    }

    //清除调色板
    if (pal)
    {
        if (pal->colors)
        {
            free(pal->colors);
            pal->colors = NULL;
        }
        SDL_FreePalette(pal);
        pal = NULL;
    }
    //清除贴图
    if (_canvas->texture)
    {
        SDL_DestroyTexture(_canvas->texture);
        _canvas->texture = NULL;
    }

    //重置基础数据
    _canvas->rect.x = 0;
    _canvas->rect.y = 0;
    _canvas->h_count = 0;
    _canvas->v_count = 0;
    _canvas->rect.w = 0;
    _canvas->rect.h = 0;
    _canvas->scale = &scale_data[0];
    _canvas->show_grid = 1;
}

//创建地图
void CreateMap()
{
    dynamicMapSize.x = tile_size;
    dynamicMapSize.y = tile_size;
    open_dynamic_grid = 1;
    open_dynamic_grid_pre = 1;
    // canvas_map.ID = 3; //布局索引
    //用来释放内存时记录数量
    int _v_count = canvas_map.v_count;
    canvas_map.rect.x = layout_editor[3].rect.x + 1;
    canvas_map.rect.y = layout_editor[3].rect.y + 1;
    canvas_map.h_count = map_size.x;
    canvas_map.v_count = map_size.y;
    canvas_map.rect.w = canvas_map.h_count * tile_size * *canvas_map.scale;
    canvas_map.rect.h = canvas_map.v_count * tile_size * *canvas_map.scale;

    if (canvas_map.texture)
    {
        SDL_DestroyTexture(canvas_map.texture);
        canvas_map.texture = NULL;
    }

    canvas_map.texture = SDL_CreateTexture(window_main.render,
                                           SDL_PIXELFORMAT_RGBA8888,
                                           SDL_TEXTUREACCESS_TARGET,
                                           canvas_map.h_count * tile_size,
                                           canvas_map.v_count * tile_size);
    SDL_SetTextureBlendMode(canvas_map.texture, SDL_BLENDMODE_BLEND);

    StartWarning("Create Map Success !!");

    is_refresh_window = 1;
    project_changed = 1;
}

//绘制网格，_canvas:画布，_rect_cut:画布和布局相交的部分，_dynamic：是否是动态网格
void DrawGrids(Canvas _canvas, SDL_Rect _dstposition, int _dynamic)
{
    if (!_canvas.texture)
        return;
    if (!_canvas.show_grid)
        return;
    SDL_SetRenderDrawBlendMode(window_main.render, SDL_BLENDMODE_BLEND);
    if (grid_color_index)
        SDL_SetRenderDrawColor(window_main.render, grid_color_black.r, grid_color_black.g, grid_color_black.b, grid_color_black.a);
    else
        SDL_SetRenderDrawColor(window_main.render, grid_color_white.r, grid_color_white.g, grid_color_white.b, grid_color_white.a);

    Vector point1, point2;
    point1.x = _dstposition.x;
    point2.x = _dstposition.x + _dstposition.w - 1;

    Vector dynamicMapSize_tmp;
    dynamicMapSize_tmp.x = dynamicMapSize.x;
    dynamicMapSize_tmp.y = dynamicMapSize.y;
    if (open_dynamic_grid == 0)
    {
        dynamicMapSize_tmp.x = tile_size;
        dynamicMapSize_tmp.y = tile_size;
    }
    int grid_size = _dynamic ? dynamicMapSize_tmp.y : tile_size;

    //横向绘制
    for (size_t i = 0; i <= _canvas.v_count; i++)
    {
        int _y = _canvas.rect.y + grid_size * *_canvas.scale * i;
        if (_y < _dstposition.y || _y > _dstposition.y + _dstposition.h)
            continue;
        SDL_RenderDrawLine(window_main.render, point1.x, _y, point2.x, _y);
    }

    point1.y = _dstposition.y;
    point2.y = _dstposition.y + _dstposition.h - 1;
    grid_size = _dynamic ? dynamicMapSize_tmp.x : tile_size;
    //纵向绘制
    for (size_t i = 0; i <= _canvas.h_count; i++)
    {
        int _x = _canvas.rect.x + grid_size * *_canvas.scale * i;
        if (_x < _dstposition.x || _x > _dstposition.x + _dstposition.w)
            continue;
        SDL_RenderDrawLine(window_main.render, _x, point1.y, _x, point2.y);
    }

    SDL_SetRenderDrawBlendMode(window_main.render, SDL_BLENDMODE_NONE);
}

void DrawCanvas(Canvas _canvas, int _layout_index, int _dynamic_grid)
{
    if (!_canvas.texture)
        return;

    //找出两个点确定相交部分的矩形
    Vector point1, point2;
    point1.x = __max(_canvas.rect.x, layout_editor[_layout_index].rect.x + 1);
    point1.y = __max(_canvas.rect.y, layout_editor[_layout_index].rect.y + 1);
    point2.x = __min(_canvas.rect.x + _canvas.rect.w, layout_editor[_layout_index].rect.x + layout_editor[_layout_index].rect.w - 2);
    point2.y = __min(_canvas.rect.y + _canvas.rect.h, layout_editor[_layout_index].rect.y + layout_editor[_layout_index].rect.h - 2);
    if (point1.x > point2.x || point1.y > point2.y)
        return;
    //计算出从画布上裁剪出来的部分
    SDL_Rect rect_cut;
    rect_cut.x = __max(layout_editor[_layout_index].rect.x + 1 - _canvas.rect.x, 0);
    rect_cut.y = __max(layout_editor[_layout_index].rect.y + 1 - _canvas.rect.y, 0);
    rect_cut.w = abs(point1.x - point2.x);
    rect_cut.h = abs(point1.y - point2.y);
    //根据裁剪的大小创建贴图
    SDL_Texture *texture_cut = SDL_CreateTexture(window_main.render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, rect_cut.w, rect_cut.h);
    SDL_SetTextureBlendMode(texture_cut, SDL_BLENDMODE_BLEND);
    //再用一个rect来计算粘贴的位置
    SDL_Rect dstPosition;
    dstPosition.x = __max(_canvas.rect.x, layout_editor[_layout_index].rect.x + 1);
    dstPosition.y = __max(_canvas.rect.y, layout_editor[_layout_index].rect.y + 1);
    dstPosition.w = rect_cut.w;
    dstPosition.h = rect_cut.h;

    //缩放到1倍大小找到源texture的位置
    SDL_Rect rect_src;

    float _tmp = (rect_cut.x + 0.0001) / *_canvas.scale;
    rect_src.x = FloatToInt(_tmp);
    _tmp = (rect_cut.y + 0.0001) / *_canvas.scale;
    rect_src.y = FloatToInt(_tmp);
    _tmp = (rect_cut.w + 0.0001) / *_canvas.scale;
    rect_src.w = FloatToInt(_tmp);
    _tmp = (rect_cut.h + 0.0001) / *_canvas.scale;
    rect_src.h = FloatToInt(_tmp);

    SDL_SetRenderTarget(window_main.render, texture_cut);
    SDL_RenderCopy(window_main.render, _canvas.texture, &rect_src, &dstPosition);
    SDL_SetRenderTarget(window_main.render, NULL);
    DrawGrids(_canvas, dstPosition, _dynamic_grid);
    DrawCanvasBorder(dstPosition);
    CollisionBoxBlackMaskInMap(_canvas, dstPosition);
    SDL_SetRenderTarget(window_main.render, NULL);
    SDL_DestroyTexture(texture_cut);
    texture_cut = NULL;
}

void DrawPalette()
{
    if (pal == NULL || canvas_tile.texture == NULL)
        return;
    int size_w = (layout_editor[1].rect.w - 2) / 16;
    int size_h = (layout_editor[1].rect.h - 2) / 4;
    SDL_Rect pos;
    pos.w = size_w;
    pos.h = size_h;

    int x = 0;
    int y = 0;
    int _count = pal->ncolors > 64 ? 64 : pal->ncolors;
    for (size_t i = 0; i < 64; i++)
    {
        if (i > 0 && i % 16 == 0)
        {
            x = 0;
            y++;
        }
        pos.x = layout_editor[1].rect.x + size_w * x + 1;
        pos.y = layout_editor[1].rect.y + size_h * y + 1;
        if (i < _count)
        {
            SDL_SetRenderDrawColor(window_main.render, pal->colors[i].r, pal->colors[i].g, pal->colors[i].b, pal->colors[i].a);
            SDL_RenderFillRect(window_main.render, &pos);
        }
        else
        {
            SDL_Rect _no;
            _no.x = pos.x + pos.w * 0.4;
            _no.y = pos.y + pos.h * 0.4;
            _no.w = pos.w * 0.2;
            _no.h = pos.h * 0.2;
            SDL_SetRenderDrawColor(window_main.render, 0, 0, 0, 0xff);
            SDL_RenderFillRect(window_main.render, &_no);
        }

        SDL_SetRenderDrawColor(window_main.render, window_bg_color.r, window_bg_color.g, window_bg_color.b, window_bg_color.a);
        SDL_RenderDrawRect(window_main.render, &pos);
        x++;
    }
}

//获取鼠标到画布的偏移值
void GetMouseToCanvasOffset()
{
    if (grabCanvas)
    {
        offset_mouse_to_canvas.x = grabCanvas->rect.x - mouse_position_cur_local.x;
        offset_mouse_to_canvas.y = grabCanvas->rect.y - mouse_position_cur_local.y;
    }
}
//画布跟随鼠标
void CanvasFollowMouse()
{
    if (grabCanvas && (mouse_position_cur_local.x != mouse_position_pre_local.x || mouse_position_cur_local.y != mouse_position_pre_local.y))
    {
        grabCanvas->rect.x = mouse_position_cur_local.x + offset_mouse_to_canvas.x;
        grabCanvas->rect.y = mouse_position_cur_local.y + offset_mouse_to_canvas.y;
        is_refresh_window = 1;
    }
}

//重置画布位置
void ResetCanvasPosition()
{
    // 如果抓取住画布的话，那么就重设画布的位置
    if (grabCanvas)
    {
        grabCanvas->rect.x = layout_editor[grabCanvas->ID].rect.x + 1;
        grabCanvas->rect.y = layout_editor[grabCanvas->ID].rect.y + 1;
        grabCanvas = NULL;
        is_refresh_window = 1;
        return;
    }
    // 如果没有抓取住画布的话，就按鼠标所在的布局
    else
    {
        if (layoutIndex == 3 && canvas_map.texture)
        {
            canvas_map.rect.x = layout_editor[3].rect.x + 1;
            canvas_map.rect.y = layout_editor[3].rect.y + 1;
            is_refresh_window = 1;
        }
        else if (layoutIndex == 2 && canvas_tile.texture)
        {
            canvas_tile.rect.x = layout_editor[2].rect.x + 1;
            canvas_tile.rect.y = layout_editor[2].rect.y + 1;
            is_refresh_window = 1;
        }
    }
}

//开始拷贝tile画布
void BrushGetStartPos()
{
    if (!canvas_tile.texture)
        return;
    //计算鼠标在tile画布上的位置，并且malloc数组并赋值
    if (!layout_editor[2].activation)
        return;
    brush.startPos.x = (mouse_position_cur_local.x - canvas_tile.rect.x) / (tile_size * *canvas_tile.scale);
    brush.startPos.y = (mouse_position_cur_local.y - canvas_tile.rect.y) / (tile_size * *canvas_tile.scale);

    if (brush.startPos.x < 0)
        brush.startPos.x = 0;
    if (brush.startPos.y < 0)
        brush.startPos.y = 0;
    if (brush.startPos.x > canvas_tile.h_count - 1)
        brush.startPos.x = canvas_tile.h_count - 1;
    if (brush.startPos.y > canvas_tile.v_count - 1)
        brush.startPos.y = canvas_tile.v_count - 1;

    //计算完之后打开拷贝开关
    brush.startCopy = 1;
}

//开始拷贝tile画布在Map上
void BrushGetStartPosInMap()
{
    if (!canvas_map.texture)
        return;
    //计算鼠标在tile画布上的位置，并且malloc数组并赋值
    if (!layout_editor[3].activation)
        return;
    brush.startPosInMap.x = (mouse_position_cur_local.x - canvas_map.rect.x) / (tile_size * *canvas_map.scale);
    brush.startPosInMap.y = (mouse_position_cur_local.y - canvas_map.rect.y) / (tile_size * *canvas_map.scale);

    if (brush.startPosInMap.x < 0)
        brush.startPosInMap.x = 0;
    if (brush.startPosInMap.y < 0)
        brush.startPosInMap.y = 0;
    if (brush.startPosInMap.x > canvas_map.h_count - 1)
        brush.startPosInMap.x = canvas_map.h_count - 1;
    if (brush.startPosInMap.y > canvas_map.v_count - 1)
        brush.startPosInMap.y = canvas_map.v_count - 1;

    //计算完之后打开拷贝开关
    brush.startCopyInMap = 1;
}

//更新画笔的拷贝矩形数据以及动态地图tilesize
static void UpdateBrushCopyRect()
{
    if (brush.endPos.x >= brush.startPos.x)
        brush.copyRect.x = brush.startPos.x * tile_size;
    else
        brush.copyRect.x = brush.endPos.x * tile_size;
    brush.copyRect.w = (abs(brush.startPos.x - brush.endPos.x) + 1) * tile_size;

    if (brush.endPos.y >= brush.startPos.y)
        brush.copyRect.y = brush.startPos.y * tile_size;
    else
        brush.copyRect.y = brush.endPos.y * tile_size;
    brush.copyRect.h = (abs(brush.endPos.y - brush.startPos.y) + 1) * tile_size;

    dynamicMapSize.x = brush.copyRect.w;
    dynamicMapSize.y = brush.copyRect.h;
}

//更新画笔的拷贝矩形数据InMap
static void UpdateBrushCopyRectInMap()
{
    if (brush.endPosInMap.x >= brush.startPosInMap.x)
        brush.copyRectInMap.x = brush.startPosInMap.x * tile_size;
    else
        brush.copyRectInMap.x = brush.endPosInMap.x * tile_size;
    brush.copyRectInMap.w = (abs(brush.startPosInMap.x - brush.endPosInMap.x) + 1) * tile_size;

    if (brush.endPosInMap.y >= brush.startPosInMap.y)
        brush.copyRectInMap.y = brush.startPosInMap.y * tile_size;
    else
        brush.copyRectInMap.y = brush.endPosInMap.y * tile_size;
    brush.copyRectInMap.h = (abs(brush.endPosInMap.y - brush.startPosInMap.y) + 1) * tile_size;
}

//计算画笔的endpos
void BrushGetEndPos()
{
    if (!brush.startCopy || !canvas_tile.texture)
        return;
    //只有画笔startCopy时才会执行持续copy
    brush.endPos.x = (mouse_position_cur_local.x - canvas_tile.rect.x) / (tile_size * *canvas_tile.scale);
    brush.endPos.y = (mouse_position_cur_local.y - canvas_tile.rect.y) / (tile_size * *canvas_tile.scale);
    if (brush.endPos.x < 0)
        brush.endPos.x = 0;
    if (brush.endPos.y < 0)
        brush.endPos.y = 0;
    if (brush.endPos.x > canvas_tile.h_count - 1)
        brush.endPos.x = canvas_tile.h_count - 1;
    if (brush.endPos.y > canvas_tile.v_count - 1)
        brush.endPos.y = canvas_tile.v_count - 1;
    UpdateBrushCopyRect();
}

//计算画笔的endpos在Map上
void BrushGetEndPosInMap()
{
    if (!brush.startCopyInMap || !canvas_map.texture)
        return;
    //只有画笔startCopyInMap时才会执行持续copy
    brush.endPosInMap.x = (mouse_position_cur_local.x - canvas_map.rect.x) / (tile_size * *canvas_map.scale);
    brush.endPosInMap.y = (mouse_position_cur_local.y - canvas_map.rect.y) / (tile_size * *canvas_map.scale);
    if (brush.endPosInMap.x < 0)
        brush.endPosInMap.x = 0;
    if (brush.endPosInMap.y < 0)
        brush.endPosInMap.y = 0;
    if (brush.endPosInMap.x > canvas_map.h_count - 1)
        brush.endPosInMap.x = canvas_map.h_count - 1;
    if (brush.endPosInMap.y > canvas_map.v_count - 1)
        brush.endPosInMap.y = canvas_map.v_count - 1;
    UpdateBrushCopyRectInMap();
}

//重设画笔数据
void ResetBrush()
{
    brush.copyRect.x = 0;
    brush.copyRect.y = 0;
    brush.copyRect.w = tile_size;
    brush.copyRect.h = tile_size;
    brush.copyRectInMap.x = 0;
    brush.copyRectInMap.y = 0;
    brush.copyRectInMap.w = 0;
    brush.copyRectInMap.h = 0;
    brush.startPos.x = 0;
    brush.startPos.y = 0;
    brush.startPosInMap.x = 0;
    brush.startPosInMap.y = 0;
    brush.startCopy = 0;
    brush.startCopyInMap = 0;
    brush.startPaste = 0;
    brush.posInMap.x = 0;
    brush.posInMap.y = 0;
    brush.posInMap_pre.x = 0;
    brush.posInMap_pre.y = 0;
    brush.endPos.x = 0;
    brush.endPos.y = 0;
    brush.endPosInMap.x = 0;
    brush.endPosInMap.y = 0;
    brush.draw_collision_box = 0;
}

//画笔绘制到map.texture
static void BrushDraw()
{
    if (!brush.startPaste)
        return;
    SDL_SetRenderTarget(window_main.render, canvas_map.texture);
    SDL_RenderCopy(window_main.render, canvas_tile.texture, &brush.copyRect, &brush.posInMap);
    SDL_SetRenderTarget(window_main.render, NULL);
    project_changed = 1;
}

//获取画笔在map上的位置
void GetBrushPosInMap()
{
    //更新鼠标在map的位置，网格大小根据选定的网格大小而定
    if (!canvas_map.texture || !canvas_map.texture)
        return;
    brush.posInMap_pre.x = brush.posInMap.x;
    brush.posInMap_pre.y = brush.posInMap.y;
    Vector dynamicMapSize_tmp;
    dynamicMapSize_tmp.x = dynamicMapSize.x;
    dynamicMapSize_tmp.y = dynamicMapSize.y;
    if (open_dynamic_grid == 0)
    {
        dynamicMapSize_tmp.x = tile_size;
        dynamicMapSize_tmp.y = tile_size;
    }
    brush.posInMap.x = (mouse_position_cur_local.x - canvas_map.rect.x) / (dynamicMapSize_tmp.x * *canvas_map.scale);
    brush.posInMap.y = (mouse_position_cur_local.y - canvas_map.rect.y) / (dynamicMapSize_tmp.y * *canvas_map.scale);

    int _min = open_dynamic_grid ? -1 : brush.copyRect.w / tile_size * -1;

    if (mouse_position_cur_local.x - canvas_map.rect.x < 0)
    {
        if (open_dynamic_grid)
            brush.posInMap.x = _min;
        else
        {
            brush.posInMap.x--;
            if (brush.posInMap.x < _min)
                brush.posInMap.x = _min;
        }
    }

    _min = open_dynamic_grid ? -1 : brush.copyRect.h / tile_size * -1;

    if (mouse_position_cur_local.y - canvas_map.rect.y < 0)
    {
        if (open_dynamic_grid)
            brush.posInMap.y = _min;
        else
        {
            brush.posInMap.y--;
            if (brush.posInMap.y < _min)
                brush.posInMap.y = _min;
        }
    }

    int tile_count = canvas_map.rect.w / (dynamicMapSize_tmp.x * *canvas_map.scale);
    if (brush.posInMap.x > tile_count)
    {
        brush.posInMap.x = tile_count;
    }
    tile_count = canvas_map.rect.h / (dynamicMapSize_tmp.y * *canvas_map.scale);
    if (brush.posInMap.y > tile_count)
    {
        brush.posInMap.y = tile_count;
    }
    brush.posInMap.x *= dynamicMapSize_tmp.x;
    brush.posInMap.y *= dynamicMapSize_tmp.y;
    brush.posInMap.w = brush.copyRect.w;
    brush.posInMap.h = brush.copyRect.h;
    if (brush.posInMap.x != brush.posInMap_pre.x || brush.posInMap.y != brush.posInMap_pre.y)
        is_refresh_window = 1;
}

//切换动态网格开关
void SwitchDynamicGrid()
{
    if (!canvas_map.texture)
        return;
    if (brush.draw_collision_box)
        return;
    open_dynamic_grid++;
    open_dynamic_grid %= 2;
    is_refresh_window = 1;
}

#endif