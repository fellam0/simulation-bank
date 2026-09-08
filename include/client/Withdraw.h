#pragma once

#include "client/AbstractOperation.h"

class Withdraw : public AbstractOperation {
public:
    using AbstractOperation::AbstractOperation;

    bool isUrgent() const override { return true; }
    std::string name() const override { return "Retrait important"; }
};
