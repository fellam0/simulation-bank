#pragma once

#include <deque>
#include <memory>
#include <vector>

#include "client/AbstractClient.h"

// FIFO queue for clients, shared by all cashiers. Method names follow the
// provided class diagram; size()/clients() are extras used by the GUI.
class Queue {
public:
    bool isEmpty() const;
    std::size_t size() const;

    void addQueueLast(std::shared_ptr<AbstractClient> client);
    std::shared_ptr<AbstractClient> getQueueFirst();

    // Peeks (does not remove) the VIP client closest to the head of the queue.
    std::shared_ptr<AbstractClient> findPriorityClient() const;
    void removePriorityClient(const std::shared_ptr<AbstractClient>& client);

    void updateClientPatience();
    std::vector<std::shared_ptr<AbstractClient>> removeImpatientClients();

    const std::deque<std::shared_ptr<AbstractClient>>& clients() const;

private:
    std::deque<std::shared_ptr<AbstractClient>> clients_;
};
