#ifndef _WARNING_C_
#define _WARNING_C_

#include "attribute.c"
#include "my_timer.c"

static void SetWarning(char *_content)
{
    free(warning);
    warning = NULL;
    warning = _content;
}

//开始警告，_warning:警告内容
void StartWarning(char *_warning)
{
    show_warning = 1;
    StartTimer(&warning_timer);
    SetWarning(_warning);
    is_refresh_window = 1;
}

#endif