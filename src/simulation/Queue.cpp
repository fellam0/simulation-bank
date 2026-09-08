#include "simulation/Queue.h"

#include <algorithm>

bool Queue::isEmpty() const {
    return clients_.empty();
}

std::size_t Queue::size() const {
    return clients_.size();
}

void Queue::pushBack(std::shared_ptr<AbstractClient> client) {
    clients_.push_back(std::move(client));
}

std::shared_ptr<AbstractClient> Queue::takeNextToServe() {
    if (clients_.empty()) {
        return nullptr;
    }

    auto priorityIt = std::find_if(clients_.begin(), clients_.end(),
        [](const std::shared_ptr<AbstractClient>& c) { return c->isPriority(); });

    if (priorityIt != clients_.end()) {
        auto client = *priorityIt;
        clients_.erase(priorityIt);
        return client;
    }

    auto client = clients_.front();
    clients_.pop_front();
    return client;
}

void Queue::updatePatience() {
    for (auto& client : clients_) {
        client->reducePatience();
    }
}

std::vector<std::shared_ptr<AbstractClient>> Queue::removeImpatientClients() {
    std::vector<std::shared_ptr<AbstractClient>> removed;

    auto it = std::remove_if(clients_.begin(), clients_.end(),
        [&removed](const std::shared_ptr<AbstractClient>& c) {
            if (!c->isPatient()) {
                removed.push_back(c);
                return true;
            }
            return false;
        });
    clients_.erase(it, clients_.end());

    return removed;
}

const std::deque<std::shared_ptr<AbstractClient>>& Queue::clients() const {
    return clients_;
}
