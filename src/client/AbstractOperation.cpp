#include "client/AbstractOperation.h"

AbstractOperation::AbstractOperation(int serviceTime) : serviceTime_(serviceTime) {}

int AbstractOperation::getServiceTime() const {
    return serviceTime_;
}

void AbstractOperation::setServiceTime(int serviceTime) {
    serviceTime_ = serviceTime;
}
