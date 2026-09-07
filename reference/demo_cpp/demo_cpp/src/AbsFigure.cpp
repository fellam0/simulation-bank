#include "AbsFigure.h"
#include <iostream>

AbsFigure::AbsFigure(int x, int y, int distance) : m_distance(distance)
{
    m_point = new Point(x, y);
}

AbsFigure::~AbsFigure()
{
    delete m_point;
}

void AbsFigure::show()
{

    std::cout << "generic figure" << std::endl;
}

