#include "client/AbstractClient.h"

AbstractClient::AbstractClient(int id, int arrivalTime, std::unique_ptr<AbstractOperation> operation, int patienceTime)
    : id_(id), arrivalTime_(arrivalTime), operation_(std::move(operation)), patienceTime_(patienceTime) {}

int AbstractClient::getId() const {
    return id_;
}

int AbstractClient::getArrivalTime() const {
    return arrivalTime_;
}

void AbstractClient::setArrivalTime(int t) {
    arrivalTime_ = t;
}

int AbstractClient::getServiceStartTime() const {
    return serviceStartTime_;
}

void AbstractClient::setServiceStartTime(int t) {
    serviceStartTime_ = t;
}

int AbstractClient::getDepartureTime() const {
    return departureTime_;
}

void AbstractClient::setDepartureTime(int t) {
    departureTime_ = t;
}

AbstractOperation& AbstractClient::getOperation() const {
    return *operation_;
}

void AbstractClient::reducePatience() {
    if (patienceTime_ > 0) {
        patienceTime_--;
    }
}

bool AbstractClient::isPatient() const {
    return patienceTime_ > 0 || operation_->isUrgent();
}

int AbstractClient::getPatienceTime() const {
    return patienceTime_;
}

void AbstractClient::setPatienceTime(int t) {
    patienceTime_ = t;
}

std::string AbstractClient::toString() const {
    return label() + " #" + std::to_string(id_) + " [arrivee=" + std::to_string(arrivalTime_) +
           ", " + operation_->name() + "]";
}
