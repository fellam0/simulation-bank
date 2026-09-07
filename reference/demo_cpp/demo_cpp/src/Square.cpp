#include "Square.h"
#include <iostream>

Square::Square(int x, int y, int distance, int height) : Rectangle(x, y, distance, height)
{
}

void Square::show()
{
    std::cout << "Square starting from " << m_point->info() << " with side " << m_distance << std::endl;
}
