#ifndef _MY_TIMER_C_
#define _MY_TIMER_C_

#include <SDL2/SDL.h>
#include <stdio.h>

typedef struct _timer
{
    int timer_pre;
    int timer;
    int max_timer;
    void (*timer_end_call)();
} Timer;

void InitTimer(Timer *_timer, int _max_timer, void (*_call)())
{
    if (_timer)
    {
        _timer->max_timer = _max_timer;
        _timer->timer = 0;
        _timer->timer_pre = 0;
        _timer->timer_end_call = _call;
    }
}

void StartTimer(Timer *_timer)
{
    if (_timer)
    {
        _timer->timer = _timer->max_timer;
    }
}

void UpdateTimer(Timer *_timer)
{
    if (_timer)
    {
        _timer->timer_pre = _timer->timer;
        if (_timer->timer > 0)
            _timer->timer--;
        else
            _timer->timer = 0;
        if (_timer->timer_pre > 0 && _timer->timer == 0 && _timer->timer_end_call)
            _timer->timer_end_call();
    }
}

#endif