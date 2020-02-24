#ifndef COLORSCHEME_H
#define COLORSCHEME_H

#include<QColor>

class ColorScheme
{
public:
    explicit ColorScheme();
    QColor getColor(float v,float min,float max,int n);
};

#endif // COLORSCHEME_H
