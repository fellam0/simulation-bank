#include "Point.h"

Point::Point(int x, int y) : m_x(x), m_y(y)
{

}

int Point:: getX()
{
    return m_x;
}

int Point:: getY()
{
    return m_y;
}

std::string Point::info()
{
    return "Point(" + std::to_string(m_x) + " " + std::to_string(m_y) + ")";
}
