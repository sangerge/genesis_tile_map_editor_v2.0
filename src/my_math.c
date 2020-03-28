#ifndef _MY_MATH_C_
#define _MY_MATH_C_

//小数转整数，四舍五入
int FloatToInt(float _value)
{
    int a = _value;
    float _f = _value - a;
    if (_f >= 0.5)
    {
        a++;
    }
    return a;
}

//向量归一化
void VectorNormalize(Vector _vector, float *_dir_x, float *_dir_y)
{
    float _sum = _vector.x + _vector.y;
    *_dir_x = _vector.x / _sum;
    *_dir_y = _vector.y / _sum;
}

//向量乘法
void VectorMult(Vector *_vector, int _value)
{
    _vector->x *= _value;
    _vector->y *= _value;
}

//点是否在矩形之内，是返回1，否返回0
int IsPointInRect(int _point_x, int _point_y, SDL_Rect _rect)
{
    return (_point_x > _rect.x && _point_x < _rect.x + _rect.w &&
            _point_y > _rect.y && _point_y < _rect.y + _rect.h);
}

#endif