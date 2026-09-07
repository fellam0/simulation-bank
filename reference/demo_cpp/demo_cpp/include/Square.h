#ifndef SQUARE_H
#define SQUARE_H

#include "Rectangle.h"

class Square : public Rectangle
{
    public:
        Square(int x, int y, int distance, int height);
        virtual void show();

    protected:

    private:
};

#endif // SQUARE_H
