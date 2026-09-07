#include "Disk.h"
#include <cmath>
#include <iostream>

Disk::Disk(int x, int y, int distance) : AbsFigure(x, y, distance)
{

}

double Disk::surface()
{
    return M_PI * m_distance * m_distance;
}

double Disk::perimeter()

{
    return 2 * M_PI * m_distance;
}

void Disk::show()
{
    std::cout << "Disk starting from " << m_point->info() << " with radius " << m_distance << std::endl;
}
