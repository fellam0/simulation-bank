#ifndef DISK_H
#define DISK_H

#include "AbsFigure.h"

class Disk : public AbsFigure
{
    public:
        Disk(int x, int y, int distance);
        virtual double surface();
        virtual double perimeter();
        virtual void show();

    protected:

    private:
};

#endif // DISK_H
