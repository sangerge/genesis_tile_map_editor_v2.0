#ifndef _ARCHIVE_C_
#define _ARCHIVE_C_

#include <SDL2/SDL.h>
#include <stdio.h>
#include "attribute.c"
#include "my_lib.c"
#include <dirent.h>

//创建工程文件夹
static int CreateProjectFolder()
{
    int res = 0;
    //记录当前目录
    char cur_path[INT16_MAX];
    getcwd(cur_path, INT16_MAX);
    //进入工程目录
    res = chdir(PRJ_PATH);
    //创建工程文件夹
    mkdir(prj_name);
    //回到上一层目录
    chdir(cur_path);
    return res;
}

//保存碰撞盒数据
static void SaveCollisionBoxData(FILE *_fp)
{

    //
    fwrite(&(c_box_lists.count), sizeof(int), 1, _fp);
    if (c_box_lists.count <= 0)
        return;
    fwrite(&(c_box_lists.index), sizeof(int), 1, _fp);
    fwrite(&(c_box_lists.solo), sizeof(int), 1, _fp);
    for (int i = 0; i < c_box_lists.count; i++)
    {
        fwrite(&(c_box_lists.lists[i]->count), sizeof(int), 1, _fp);
        fwrite(&(c_box_lists.lists[i]->color), sizeof(SDL_Color), 1, _fp);
        fwrite(&(c_box_lists.lists[i]->index), sizeof(int), 1, _fp);
        fwrite(&(c_box_lists.lists[i]->value), sizeof(char), 1, _fp);
        for (int j = 0; j < c_box_lists.lists[i]->count; j++)
            fwrite(c_box_lists.lists[i]->rects[j], sizeof(SDL_Rect), 1, _fp);
    }
}

//读取碰撞盒数据
static void ReadCollisionBoxData(FILE *_fp)
{
    FreeCollisionBoxLists();
    fread(&(c_box_lists.count), sizeof(int), 1, _fp);
    if (c_box_lists.count <= 0)
        return;
    fread(&(c_box_lists.index), sizeof(int), 1, _fp);
    fread(&(c_box_lists.solo), sizeof(int), 1, _fp);
    c_box_lists.lists = (C_Box_List **)malloc(sizeof(C_Box_List *) * c_box_lists.count);
    for (int i = 0; i < c_box_lists.count; i++)
    {
        c_box_lists.lists[i] = (C_Box_List *)malloc(sizeof(C_Box_List));
        fread(&c_box_lists.lists[i]->count, sizeof(int), 1, _fp);
        fread(&c_box_lists.lists[i]->color, sizeof(SDL_Color), 1, _fp);
        fread(&c_box_lists.lists[i]->index, sizeof(int), 1, _fp);
        fread(&c_box_lists.lists[i]->value, sizeof(char), 1, _fp);
        c_box_lists.lists[i]->rects = (SDL_Rect **)malloc(sizeof(SDL_Rect *) * c_box_lists.lists[i]->count);
        for (int j = 0; j < c_box_lists.lists[i]->count; j++)
        {
            c_box_lists.lists[i]->rects[j] = (SDL_Rect *)malloc(sizeof(SDL_Rect));
            fread(c_box_lists.lists[i]->rects[j], sizeof(SDL_Rect), 1, _fp);
        }
    }
}

//保存工程
void SaveProject()
{
    if (prj_name[0] == '\0' || cur_project_path[0] == '\0')
    {
        StartWarning("No Project to save!");
        return;
    }
    if (CreateProjectFolder() == -1)
    {
        StartWarning("Project Folder Create Failed !");
        return;
    }
    //记录当前目录
    char cur_path[INT16_MAX];
    getcwd(cur_path, INT16_MAX);

    //进入工程目录
    chdir(cur_project_path);

    //保存
    {
        //
        FILE *fp = fopen(SAVE_NAME, "wb");
        if (!fp)
        {
            StartWarning("Save Failed !");
            chdir(cur_path);
            return;
        }
        //保存布局
        fwrite(&layout_editor[1].rect, sizeof(SDL_Rect), 1, fp);
        fwrite(&layout_editor[2].rect, sizeof(SDL_Rect), 1, fp);
        fwrite(&layout_editor[3].rect, sizeof(SDL_Rect), 1, fp);
        //tile_size
        fwrite(&tile_size, sizeof(int), 1, fp);
        //map_size
        fwrite(&map_size, sizeof(Vector), 1, fp);
        //工程名字
        fwrite(&prj_name, sizeof(char), MAX_STRING_LENGTH, fp);
        //工程路径
        if (cur_project_path)
        {
            for (int i = 0; i < PRJ_PATH_STR_LENGTH; i++)
                fwrite(&cur_project_path[i], sizeof(char), 1, fp);
        }

        //是否显示map网格
        fwrite(&canvas_map.show_grid, sizeof(int), 1, fp);
        //网格颜色索引
        fwrite(&grid_color_index, sizeof(int), 1, fp);
        //是否开启动态网格
        fwrite(&open_dynamic_grid, sizeof(int), 1, fp);
        fwrite(&open_dynamic_grid_pre, sizeof(int), 1, fp);
        //动态网格数据
        fwrite(&dynamicMapSize.x, sizeof(int), 1, fp);
        fwrite(&dynamicMapSize.y, sizeof(int), 1, fp);
        //地图网格数量
        fwrite(&canvas_map.h_count, sizeof(int), 1, fp);
        fwrite(&canvas_map.v_count, sizeof(int), 1, fp);

        {
            //是否有tile
            int tile_exist = canvas_tile.texture == NULL ? 0 : 1;
            fwrite(&tile_exist, sizeof(int), 1, fp);
            //保存tile
            if (tile_exist)
            {
                //保存色表
                {
                    if (pal)
                    {
                        int ncolors = pal->ncolors;
                        fwrite(&ncolors, sizeof(int), 1, fp);
                        int refcount = pal->refcount;
                        fwrite(&refcount, sizeof(int), 1, fp);
                        Uint32 version = pal->version;
                        fwrite(&version, sizeof(Uint32), 1, fp);
                        for (int i = 0; i < pal->ncolors; i++)
                        {
                            SDL_Color col = pal->colors[i];
                            fwrite(&col, sizeof(SDL_Color), 1, fp);
                        }
                    }
                }

                //保存tile数据
                fwrite(&canvas_tile.rect.x, sizeof(int), 1, fp);
                fwrite(&canvas_tile.rect.y, sizeof(int), 1, fp);
                fwrite(&canvas_tile.rect.w, sizeof(int), 1, fp);
                fwrite(&canvas_tile.rect.h, sizeof(int), 1, fp);
                fwrite(&canvas_tile.show_grid, sizeof(int), 1, fp);
                fwrite(&canvas_tile.h_count, sizeof(int), 1, fp);
                fwrite(&canvas_tile.v_count, sizeof(int), 1, fp);
                int scale = *canvas_tile.scale;
                fwrite(&scale, sizeof(int), 1, fp);
                fwrite(&pitch, sizeof(int), 1, fp);
                for (int i = 0; i < canvas_tile.h_count * tile_size * canvas_tile.v_count * tile_size; i++)
                    fwrite(&pixels[i], sizeof(Uint32), 1, fp);

                //保存Brush数据
                fwrite(&brush.copyRect, sizeof(SDL_Rect), 1, fp);
                fwrite(&brush.copyRectInMap, sizeof(SDL_Rect), 1, fp);
                fwrite(&brush.draw_collision_box, sizeof(int), 1, fp);
                fwrite(&brush.endPos, sizeof(Vector), 1, fp);
                fwrite(&brush.endPosInMap, sizeof(Vector), 1, fp);
                fwrite(&brush.posInMap, sizeof(SDL_Rect), 1, fp);
                fwrite(&brush.posInMap_pre, sizeof(SDL_Rect), 1, fp);
                fwrite(&brush.startCopy, sizeof(int), 1, fp);
                fwrite(&brush.startCopyInMap, sizeof(int), 1, fp);
                fwrite(&brush.startPaste, sizeof(int), 1, fp);
                fwrite(&brush.startPos, sizeof(Vector), 1, fp);
                fwrite(&brush.startPosInMap, sizeof(Vector), 1, fp);
            }
        }

        //保存地图数据
        SDL_SetRenderTarget(window_main.render, canvas_map.texture);
        int width = canvas_map.h_count * tile_size;
        int height = canvas_map.v_count * tile_size;

        SDL_Rect rect;
        rect.x = 0;
        rect.y = 0;
        rect.w = width;
        rect.h = height;
        Uint32 *pixels_map = (Uint32 *)malloc(sizeof(Uint32) * width * height);
        //每行像素所占的字节数
        int pitch = width * sizeof(Uint32);
        int res = SDL_RenderReadPixels(window_main.render, &rect, SDL_PIXELFORMAT_RGBA8888, pixels_map, pitch);
        SDL_SetRenderTarget(window_main.render, NULL);
        if (res == 0)
        {
            //pitch
            fwrite(&pitch, sizeof(int), 1, fp);
            //pixels
            for (int i = 0; i < width * height; i++)
            {
                fwrite(&pixels_map[i], sizeof(Uint32), 1, fp);
            }
        }
        free(pixels_map);
        pixels_map = NULL;
        //地图坐标
        fwrite(&canvas_map.rect.x, sizeof(int), 1, fp);
        fwrite(&canvas_map.rect.y, sizeof(int), 1, fp);
        //地图缩放
        int scale = *canvas_map.scale;
        fwrite(&scale, sizeof(int), 1, fp);

        //地图宽高
        fwrite(&canvas_map.rect.w, sizeof(int), 1, fp);
        fwrite(&canvas_map.rect.h, sizeof(int), 1, fp);
        //是否显示网格
        fwrite(&canvas_map.show_grid, sizeof(int), 1, fp);
        //============================================碰撞盒数据==================
        SaveCollisionBoxData(fp);
        //

        fclose(fp);
        free(fp);
        fp = NULL;
    }
    //回到上一层目录
    chdir(cur_path);
    project_changed = 0;
    StartWarning("Save Success !");
}

//加载工程
void LoadProject()
{
    if (cur_project_path[0] == '\0')
    {
        StartWarning("Load Failed !");
        return;
    }
    //记录当前目录
    char cur_path[INT16_MAX];
    getcwd(cur_path, INT16_MAX);
    //进入工程目录
    chdir(cur_project_path);
    {
        FILE *fp = fopen(SAVE_NAME, "rb");
        if (!fp)
        {
            StartWarning("Load Failed !");
            chdir(cur_path);
            return;
        }
        else
        {
            //加载布局
            fread(&layout_editor[1].rect, sizeof(SDL_Rect), 1, fp);
            fread(&layout_editor[2].rect, sizeof(SDL_Rect), 1, fp);
            fread(&layout_editor[3].rect, sizeof(SDL_Rect), 1, fp);
            //tile_size
            fread(&tile_size, sizeof(int), 1, fp);
            //map_size
            fread(&map_size, sizeof(Vector), 1, fp);
            //工程名字
            fread(&prj_name, sizeof(char), MAX_STRING_LENGTH, fp);
            //工程路径
            free(cur_project_path);
            cur_project_path = (char *)malloc(sizeof(char) * PRJ_PATH_STR_LENGTH);
            for (int i = 0; i < PRJ_PATH_STR_LENGTH; i++)
            {
                cur_project_path[i] = '\0';
                fread(&cur_project_path[i], sizeof(char), 1, fp);
            }

            //=================================================
            //是否显示map网格
            fread(&canvas_map.show_grid, sizeof(int), 1, fp);
            //网格颜色索引
            fread(&grid_color_index, sizeof(int), 1, fp);
            //是否开启动态网格
            fread(&open_dynamic_grid, sizeof(int), 1, fp);
            fread(&open_dynamic_grid_pre, sizeof(int), 1, fp);
            //动态网格数据
            fread(&dynamicMapSize.x, sizeof(int), 1, fp);
            fread(&dynamicMapSize.y, sizeof(int), 1, fp);
            //==========================================
            //地图网格数量
            fread(&canvas_map.h_count, sizeof(int), 1, fp);
            fread(&canvas_map.v_count, sizeof(int), 1, fp);

            {
                //是否有tile
                int tile_exist = 0;
                fread(&tile_exist, sizeof(int), 1, fp);
                //读取tile
                if (tile_exist)
                {
                    //读取色表
                    {
                        if (pal)
                        {
                            SDL_FreePalette(pal);
                            pal = NULL;
                        }

                        int ncolors = 0;
                        fread(&ncolors, sizeof(int), 1, fp);
                        pal = SDL_AllocPalette(ncolors);
                        pal->ncolors = ncolors;
                        int refcount = 0;
                        fread(&refcount, sizeof(int), 1, fp);
                        pal->refcount = refcount;
                        Uint32 version = 0;
                        fread(&version, sizeof(Uint32), 1, fp);
                        pal->version = version;
                        for (int i = 0; i < pal->ncolors; i++)
                        {
                            SDL_Color col;
                            fread(&col, sizeof(SDL_Color), 1, fp);
                            pal->colors[i] = col;
                        }
                    }
                    //tile数据
                    fread(&canvas_tile.rect.x, sizeof(int), 1, fp);
                    fread(&canvas_tile.rect.y, sizeof(int), 1, fp);
                    fread(&canvas_tile.rect.w, sizeof(int), 1, fp);
                    fread(&canvas_tile.rect.h, sizeof(int), 1, fp);
                    fread(&canvas_tile.show_grid, sizeof(int), 1, fp);
                    fread(&canvas_tile.h_count, sizeof(int), 1, fp);
                    fread(&canvas_tile.v_count, sizeof(int), 1, fp);
                    int scale = 1;
                    fread(&scale, sizeof(int), 1, fp);
                    switch (scale)
                    {
                    case 1:
                        canvas_tile.scale = &scale_data[0];
                        break;
                    case 2:
                        canvas_tile.scale = &scale_data[1];
                        break;
                    case 4:
                        canvas_tile.scale = &scale_data[2];
                        break;
                    case 8:
                        canvas_tile.scale = &scale_data[3];
                        break;
                    }

                    fread(&pitch, sizeof(int), 1, fp);
                    free(pixels);
                    int w_tile = canvas_tile.h_count * tile_size;
                    int h_tile = canvas_tile.v_count * tile_size;
                    pixels = (Uint32 *)malloc(sizeof(Uint32) * w_tile * h_tile);
                    for (int i = 0; i < w_tile * h_tile; i++)
                        fread(&pixels[i], sizeof(Uint32), 1, fp);
                    canvas_tile.texture = SDL_CreateTexture(window_main.render,
                                                            SDL_PIXELFORMAT_RGBA8888,
                                                            SDL_TEXTUREACCESS_TARGET,
                                                            w_tile,
                                                            h_tile);
                    SDL_UpdateTexture(canvas_tile.texture, NULL, pixels, pitch);

                    //读取画笔数据
                    fread(&brush.copyRect, sizeof(SDL_Rect), 1, fp);
                    fread(&brush.copyRectInMap, sizeof(SDL_Rect), 1, fp);
                    fread(&brush.draw_collision_box, sizeof(int), 1, fp);
                    fread(&brush.endPos, sizeof(Vector), 1, fp);
                    fread(&brush.endPosInMap, sizeof(Vector), 1, fp);
                    fread(&brush.posInMap, sizeof(SDL_Rect), 1, fp);
                    fread(&brush.posInMap_pre, sizeof(SDL_Rect), 1, fp);
                    fread(&brush.startCopy, sizeof(int), 1, fp);
                    fread(&brush.startCopyInMap, sizeof(int), 1, fp);
                    fread(&brush.startPaste, sizeof(int), 1, fp);
                    fread(&brush.startPos, sizeof(Vector), 1, fp);
                    fread(&brush.startPosInMap, sizeof(Vector), 1, fp);
                }
            }

            //保存地图数据
            int width = canvas_map.h_count * tile_size;
            int height = canvas_map.v_count * tile_size;

            SDL_Rect rect;
            rect.x = 0;
            rect.y = 0;
            rect.w = width;
            rect.h = height;
            //每行像素所占的字节数
            int pitch = 0;
            //pitch
            fread(&pitch, sizeof(int), 1, fp);
            Uint32 *pixels_map = (Uint32 *)malloc(sizeof(Uint32) * width * height);
            //pixels_map
            for (int i = 0; i < width * height; i++)
            {
                fread(&pixels_map[i], sizeof(Uint32), 1, fp);
            }

            // { //测试用pixel和pitch截图
            //     SDL_FreeSurface(canvas_map.surface);
            //     canvas_map.surface = SDL_CreateRGBSurfaceWithFormatFrom(pixels_map,
            //                                                             width,
            //                                                             height,
            //                                                             32,
            //                                                             pitch,
            //                                                             SDL_PIXELFORMAT_RGBA8888);
            //     if (canvas_map.surface)
            //         SDL_SaveBMP(canvas_map.surface, "aaa.bmp");
            // }
            chdir(cur_path);
            SDL_DestroyTexture(canvas_map.texture);
            canvas_map.texture = NULL;
            // canvas_map.texture = SDL_CreateTextureFromSurface(window_main.render, canvas_map.surface);
            canvas_map.texture = SDL_CreateTexture(window_main.render,
                                                   SDL_PIXELFORMAT_RGBA8888,
                                                   SDL_TEXTUREACCESS_TARGET,
                                                   rect.w, rect.h);
            SDL_SetTextureBlendMode(canvas_map.texture, SDL_BLENDMODE_BLEND);

            SDL_UpdateTexture(canvas_map.texture, NULL, pixels_map, pitch);
            free(pixels_map);
            pixels_map = NULL;
            //地图坐标
            fread(&canvas_map.rect.x, sizeof(int), 1, fp);
            fread(&canvas_map.rect.y, sizeof(int), 1, fp);
            //地图缩放
            int scale = 1;
            fread(&scale, sizeof(int), 1, fp);
            switch (scale)
            {
            case 1:
                canvas_map.scale = &scale_data[0];
                break;
            case 2:
                canvas_map.scale = &scale_data[1];
                break;
            case 4:
                canvas_map.scale = &scale_data[2];
                break;
            case 8:
                canvas_map.scale = &scale_data[3];
                break;
            }
            //地图宽高
            fread(&canvas_map.rect.w, sizeof(int), 1, fp);
            fread(&canvas_map.rect.h, sizeof(int), 1, fp);
            //是否显示网格
            fread(&canvas_map.show_grid, sizeof(int), 1, fp);

            //读取碰撞盒数据
            ReadCollisionBoxData(fp);
        }
        fclose(fp);
        fp = NULL;
    }
    //回到上一层目录
    chdir(cur_path);
    project_changed = 0;
}

//保存截图
static void SaveTextureToBMP(Canvas _canvas, char *_BMP_name)
{
    if (_canvas.texture == NULL)
        return;
    SDL_SetRenderTarget(window_main.render, _canvas.texture);
    int width = _canvas.h_count * tile_size;
    int height = _canvas.v_count * tile_size;

    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = width;
    rect.h = height;
    Uint32 *pixels = (Uint32 *)malloc(sizeof(Uint32) * width * height);
    //每行像素所占的字节数
    int pitch = width * sizeof(Uint32);
    int res = SDL_RenderReadPixels(window_main.render, &rect, SDL_PIXELFORMAT_RGBA8888, pixels, pitch);
    if (res == 0)
    {
        SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormatFrom(pixels, width, height, 8, pitch, SDL_PIXELFORMAT_RGBA8888);

        if (surface)
        {
            char file_path[256] = {'\0'};
            strcat(file_path, cur_project_path);
            strcat(file_path, "/");
            strcat(file_path, _BMP_name);
            SDL_SaveBMP(surface, file_path);
            SDL_FreeSurface(surface);
            surface = NULL;
        }
    }
    free(pixels);
    pixels = NULL;
    SDL_SetRenderTarget(window_main.render, NULL);
    is_refresh_window = 1;
}

//导出数据
void ExportData()
{
    if (prj_name[0] == '\0' || cur_project_path[0] == '\0')
    {
        StartWarning("No Project !");
        return;
    }
    //检测工程目录是否存在
    char _cur_path[256] = {'\0'};
    getcwd(_cur_path, 256);
    int res = chdir(cur_project_path);
    if (res == -1)
    {
        StartWarning("Please Save First !");
        chdir(_cur_path);
        return;
    }
    chdir(_cur_path);

    //导出截图
    if (canvas_map.texture)
        SaveTextureToBMP(canvas_map, EXPORT_MAP_NAME);
    //导出碰撞盒信息
    if (c_box_lists.count <= 0)
    {
        StartWarning("Did Not Export Collision Box Data !");
        return;
    }
    //获取带文件名的工程完整路径
    char _path[256] = {'\0'};
    strcat(_path, cur_project_path);
    strcat(_path, "/");
    strcat(_path, EXPORT_MAP_DATA);

    //判断每一个碰撞盒列表里是不是都有碰撞盒
    int output = 0;
    for (int i = 0; i < c_box_lists.count; i++)
    {
        if (c_box_lists.lists[i]->count > 0)
        {
            output = 1;
            break;
        }
    }
    if (output == 0)
    {
        StartWarning("Did Not Draw Any Collision Box !");
        return;
    }

    //写数据
    FILE *fp = fopen(_path, "w");
    if (fp == NULL)
    {
        StartWarning("Create \'map_data.c\' failed !");
        fclose(fp);
        free(fp);
        fp = NULL;
        return;
    }
    //计算碰撞盒种类
    int box_kind = 0;
    for (int i = 0; i < c_box_lists.count; i++)
    {
        if (c_box_lists.lists[i]->count > 0)
            box_kind++;
    }

    fputs("//**世嘉MD Tile地图编辑器_v2.0**\n\n", fp);
    fputs("//B站搜索“桑二哥”关注作者动态。\n\n//微博@桑二哥\n\n", fp);
    fprintf(fp, "//地图宽%d像素，高%d像素，tile块尺寸是%d x %d。\n\n", map_size.x * tile_size, map_size.y * tile_size, tile_size, tile_size);
    fputs("#ifndef _MAP_DATA_C_\n#define _MAP_DATA_C_\n", fp);
    fputs("\n", fp);
    fputs("#include <genesis.h>\n", fp);
    fputs("\n", fp);
    fputs("typedef struct _box{\n", fp);
    fputs("\tchar value;\n", fp);
    fputs("\tint count;\n", fp);
    fputs("\tBoxCollision* box_collisions;\n", fp);
    fputs("}Box;\n\n", fp);

    fprintf(fp, "Box map_box_data[%d] =\n{\n", box_kind);

    for (int i = 0; i < c_box_lists.count; i++)
    {
        if (c_box_lists.lists[i]->count <= 0)
            continue;
        fputs("\t{", fp);
        fprintf(fp, "'%c',%d", c_box_lists.lists[i]->value, c_box_lists.lists[i]->count);
        fputs("},\n", fp);
    }
    fputs("};\n", fp);
    fputs("\n", fp);

    //===========写入矩形数据
    for (int i = 0; i < c_box_lists.count; i++)
    {
        if (c_box_lists.lists[i]->count <= 0)
            continue;
        fprintf(fp, "static BoxCollision box_data_%c[%d] = {\n", c_box_lists.lists[i]->value, c_box_lists.lists[i]->count);
        for (int j = 0; j < c_box_lists.lists[i]->count; j++)
        {
            fputs("\t{", fp);
            fprintf(fp, "%d,", c_box_lists.lists[i]->rects[j]->x);
            fprintf(fp, "%d,", c_box_lists.lists[i]->rects[j]->y);
            fprintf(fp, "%d,", c_box_lists.lists[i]->rects[j]->w);
            fprintf(fp, "%d", c_box_lists.lists[i]->rects[j]->h);
            fputs("},\n", fp);
        }
        fputs("};\n", fp);
    }

    //写入初始化方法
    fputs("static void InitBox(Box* _box,BoxCollision* _box_collision_array){\n", fp);
    fputs("\t_box->box_collisions = (BoxCollision*)MEM_alloc(sizeof(BoxCollision) * _box->count);\n", fp);
    fputs("\t_box->box_collisions = _box_collision_array;\n}\n", fp);
    //写入初始化所有Box的方法
    fputs("void InitAllBoxes(){\n", fp);
    char value_name[15];
    char **index_name = (char **)malloc(sizeof(char *) * box_kind);

    int j = 0;
    for (int i = 0; i < c_box_lists.count; i++)
    {
        if (c_box_lists.lists[i]->count <= 0)
            continue;
        memset(&value_name[0], 0, 15);

        strcat(value_name, "box_data_");
        strcat(value_name, &(c_box_lists.lists[i]->value));
        int count = strlen(value_name);
        index_name[j] = (char *)malloc(sizeof(char) * (count + 1));
        for (int m = 0; m < count + 1; m++)
            index_name[j][m] = '\0';
        strcat(index_name[j], value_name);
        if (j + 1 < box_kind)
            j++;
    }

    for (int i = 0; i < box_kind; i++)
        fprintf(fp, "\tInitBox(&map_box_data[%d],%s);\n", i, index_name[i]);
    for (int i = 0; i < box_kind; i++)
    {
        free(index_name[i]);
        index_name[i] = NULL;
    }
    free(index_name);
    index_name = NULL;

    fputs("}\n\n", fp);
    fputs("#endif", fp);

    fclose(fp);
    free(fp);
    fp = NULL;
    StartWarning("Export Success !");
}
#endif