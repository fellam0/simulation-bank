#ifndef POINT_H
#define POINT_H

#include <string>

class Point
{
    public:
        Point(int x, int y);
        int getX();
        int getY();
        std::string info();

    private:
        int m_x;
        int m_y;
};

#endif // POINT_H
