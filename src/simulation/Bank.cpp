#include "simulation/Bank.h"

Bank::Bank(int cashierCount) {
    cashiers_.reserve(cashierCount);
    for (int id = 1; id <= cashierCount; id++) {
        cashiers_.emplace_back(id);
    }
}

std::vector<Cashier>& Bank::getCashiers() {
    return cashiers_;
}

Queue& Bank::getQueue() {
    return queue_;
}

Cashier* Bank::getFreeCashier() {
    for (auto& cashier : cashiers_) {
        if (cashier.isFree()) {
            return &cashier;
        }
    }
    return nullptr;
}
