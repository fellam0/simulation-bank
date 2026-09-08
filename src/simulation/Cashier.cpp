#include "simulation/Cashier.h"

Cashier::Cashier(int id) : id_(id) {}

int Cashier::getId() const {
    return id_;
}

bool Cashier::isFree() const {
    return servingClient_ == nullptr;
}

bool Cashier::serviceFinished() const {
    return servingClient_ != nullptr && remainingServiceTime_ == 0;
}

void Cashier::tick() {
    if (remainingServiceTime_ > 0) {
        remainingServiceTime_--;
    }
}

void Cashier::serve(std::shared_ptr<AbstractClient> client) {
    remainingServiceTime_ = client->getOperation().getServiceTime();
    servingClient_ = std::move(client);
}

std::shared_ptr<AbstractClient> Cashier::getServingClient() const {
    return servingClient_;
}

std::shared_ptr<AbstractClient> Cashier::release() {
    auto client = servingClient_;
    servingClient_.reset();
    return client;
}
