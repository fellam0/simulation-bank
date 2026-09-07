#include <iostream>
#include <vector>

#include "Point.h"
#include "AbsFigure.h"
#include "Disk.h"
#include "Rectangle.h"
#include "Square.h"

using namespace std;

int main()
{
    vector<AbsFigure*> figures;

    figures.push_back(new Disk(1,1,5));
    figures.push_back(new Rectangle(10,10,5,6));
    figures.push_back(new Square(20,20,9,9));

    for (int i = 0; i< figures.size(); i++)
    {
        cout << "*************" << endl;
        figures[i]->show();
        cout << "surface : " << figures[i]->surface() << endl;
        cout << "perimeter : " << figures[i]->perimeter() << endl;
    }

    figures.clear();
    return 0;
}
