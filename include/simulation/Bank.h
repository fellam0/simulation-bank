#pragma once

#include <vector>

#include "simulation/Cashier.h"
#include "simulation/Queue.h"

class Bank {
public:
    explicit Bank(int cashierCount);

    std::vector<Cashier>& getCashiers();
    Queue& getQueue();

    Cashier* getFreeCashier();

private:
    std::vector<Cashier> cashiers_;
    Queue queue_;
};
