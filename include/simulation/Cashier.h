#pragma once

#include <memory>

#include "client/AbstractClient.h"

class Cashier {
public:
    explicit Cashier(int id);

    int getId() const;
    bool isFree() const;
    bool serviceFinished() const;

    void tick();
    void serve(std::shared_ptr<AbstractClient> client);
    std::shared_ptr<AbstractClient> getServingClient() const;
    std::shared_ptr<AbstractClient> release();

private:
    int id_;
    std::shared_ptr<AbstractClient> servingClient_;
    int remainingServiceTime_ = 0;
};
