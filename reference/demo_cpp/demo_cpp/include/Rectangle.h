#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "AbsFigure.h"

class Rectangle : public AbsFigure
{
    public:
        Rectangle(int x, int y, int distance, int height);
        virtual double surface();
        virtual double perimeter();
        virtual void show();

    protected:
        int m_height;
    private:

};

#endif // RECTANGLE_H
