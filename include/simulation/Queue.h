#pragma once

#include <deque>
#include <memory>
#include <vector>

#include "client/AbstractClient.h"

class Queue {
public:
    bool isEmpty() const;
    std::size_t size() const;

    void pushBack(std::shared_ptr<AbstractClient> client);

    // Returns the closest-to-front VIP client if any, otherwise the client at
    // the front of the queue. Removes the returned client from the queue.
    std::shared_ptr<AbstractClient> takeNextToServe();

    void updatePatience();
    std::vector<std::shared_ptr<AbstractClient>> removeImpatientClients();

    const std::deque<std::shared_ptr<AbstractClient>>& clients() const;

private:
    std::deque<std::shared_ptr<AbstractClient>> clients_;
};
