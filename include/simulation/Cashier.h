#pragma once

#include <memory>

#include "client/AbstractClient.h"

// Method names follow the provided class diagram (isFree, serve, work,
// serviceFinished, getServingClient, setServingClient). getId() and
// remainingServiceTime() are extras used for display (console/GUI).
class Cashier {
public:
    explicit Cashier(int id);

    int getId() const;
    bool isFree() const;
    bool serviceFinished() const;
    int remainingServiceTime() const;

    void work();
    void serve(std::shared_ptr<AbstractClient> client);
    std::shared_ptr<AbstractClient> getServingClient() const;
    void setServingClient(std::shared_ptr<AbstractClient> client);

private:
    int id_;
    std::shared_ptr<AbstractClient> servingClient_;
    int remainingServiceTime_ = 0;
};
