#ifndef ABSFIGURE_H
#define ABSFIGURE_H

#include "Point.h"

class AbsFigure
{
    public:
        AbsFigure(int x, int y, int distance);
        virtual ~AbsFigure();
        virtual double surface() = 0;
        virtual double perimeter() = 0;
        virtual void show();

    protected:
        Point *m_point;
        int m_distance;
    private:

};

#endif // ABSFIGURE_H
