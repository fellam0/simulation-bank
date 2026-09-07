#include "Rectangle.h"
#include <iostream>

Rectangle::Rectangle(int x, int y, int distance, int height) : AbsFigure(x, y, distance), m_height(height)
{

}

double Rectangle::surface()
{
    return m_distance * m_height;
}

double Rectangle:: perimeter()
{
    return 2 * (m_distance + m_height);
}

void Rectangle::show()
{
    std::cout << "Rectangle starting from " << m_point->info() << " with width " << m_distance << " and height "<< m_height << std::endl;
}

