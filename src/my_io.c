#ifndef _MY_IO_C_
#define _MY_IO_C_

#include <SDL2/SDL.h>
#include <math.h>

static char SDLK_To_Char(SDL_Keycode _key, int _onlyNumber)
{
    char _char = '\0';
    switch (_key)
    {
    case SDLK_1:
        _char = '1';
        break;
    case SDLK_2:
        _char = '2';
        break;
    case SDLK_3:
        _char = '3';
        break;
    case SDLK_4:
        _char = '4';
        break;
    case SDLK_5:
        _char = '5';
        break;
    case SDLK_6:
        _char = '6';
        break;
    case SDLK_7:
        _char = '7';
        break;
    case SDLK_8:
        _char = '8';
        break;
    case SDLK_9:
        _char = '9';
        break;
    case SDLK_0:
        _char = '0';
        break;
    }

    if (_onlyNumber)
        return _char;

    switch (_key)
    {
    case SDLK_a:
        _char = 'A';
        break;
    case SDLK_b:
        _char = 'B';
        break;
    case SDLK_c:
        _char = 'C';
        break;
    case SDLK_d:
        _char = 'D';
        break;
    case SDLK_e:
        _char = 'E';
        break;
    case SDLK_f:
        _char = 'F';
        break;
    case SDLK_g:
        _char = 'G';
        break;
    case SDLK_h:
        _char = 'H';
        break;
    case SDLK_i:
        _char = 'I';
        break;
    case SDLK_j:
        _char = 'J';
        break;
    case SDLK_k:
        _char = 'K';
        break;
    case SDLK_l:
        _char = 'L';
        break;
    case SDLK_m:
        _char = 'M';
        break;
    case SDLK_n:
        _char = 'N';
        break;
    case SDLK_o:
        _char = 'O';
        break;
    case SDLK_p:
        _char = 'P';
        break;
    case SDLK_q:
        _char = 'Q';
        break;
    case SDLK_r:
        _char = 'R';
        break;
    case SDLK_s:
        _char = 'S';
        break;
    case SDLK_t:
        _char = 'T';
        break;
    case SDLK_u:
        _char = 'U';
        break;
    case SDLK_v:
        _char = 'V';
        break;
    case SDLK_w:
        _char = 'W';
        break;
    case SDLK_x:
        _char = 'X';
        break;
    case SDLK_y:
        _char = 'Y';
        break;
    case SDLK_z:
        _char = 'Z';
        break;
    case 45:
        _char = '_';
        break;
    case 46:
        _char = '.';
        break;
    }

    return _char;
}

int Char_To_Int(char _char)
{
    switch (_char)
    {
    case '1':
        return 1;
    case '2':
        return 2;
    case '3':
        return 3;
    case '4':
        return 4;
    case '5':
        return 5;
    case '6':
        return 6;
    case '7':
        return 7;
    case '8':
        return 8;
    case '9':
        return 9;
    default:
        return 0;
    }
}

//char*转int，前提是char*里都是数字
int CharStarToInt(char *_numArray)
{
    int length = 0;
    while (_numArray[length] != '\0' && _numArray[length] != ' ')
    {
        length++;
    }
    int sum = 0;
    if (length > 0)
    {
        for (int i = 0; i < length; i++)
        {
            int value = Char_To_Int(_numArray[i]);
            if (i < length - 1)
                sum += value * pow(10, length - 1 - i);
            else
                sum += value;
        }
    }
    return sum > 999999 ? 999999 : sum;
}

//接收键盘输入
void Scanf_s(char *_str, int _maxStrLength, int *_curser, SDL_Event _event, int _onlyNumber)
{
    if (_event.key.keysym.sym == SDLK_BACKSPACE && *_curser > 0)
    {
        (*_curser)--;
        _str[*_curser] = '\0';
    }
    char tmp = SDLK_To_Char(_event.key.keysym.sym, _onlyNumber);

    if (tmp != '\0' && *_curser < _maxStrLength - 1)
    {
        _str[*_curser] = tmp;
        (*_curser)++;
        _str[*_curser] = '\0';
    }
}

//用鼠标右键删除输入
void DeleteCharacter(SDL_Event _event, int *_curser, char *_str)
{
    if (_event.button.button == 3 && *_curser > 0)
    {
        (*_curser)--;
        _str[*_curser] = '\0';
    }
}

#endif