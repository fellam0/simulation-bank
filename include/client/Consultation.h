#pragma once

#include "client/AbstractOperation.h"

class Consultation : public AbstractOperation {
public:
    using AbstractOperation::AbstractOperation;

    bool isUrgent() const override { return false; }
    std::string name() const override { return "Consultation"; }
};
