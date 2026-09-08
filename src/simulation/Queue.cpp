#include "simulation/Queue.h"

#include <algorithm>

bool Queue::isEmpty() const {
    return clients_.empty();
}

std::size_t Queue::size() const {
    return clients_.size();
}

void Queue::addQueueLast(std::shared_ptr<AbstractClient> client) {
    clients_.push_back(std::move(client));
}

std::shared_ptr<AbstractClient> Queue::getQueueFirst() {
    if (clients_.empty()) {
        return nullptr;
    }
    auto client = clients_.front();
    clients_.pop_front();
    return client;
}

std::shared_ptr<AbstractClient> Queue::findPriorityClient() const {
    auto it = std::find_if(clients_.begin(), clients_.end(),
        [](const std::shared_ptr<AbstractClient>& c) { return c->isPriority(); });
    return it == clients_.end() ? nullptr : *it;
}

void Queue::removePriorityClient(const std::shared_ptr<AbstractClient>& client) {
    auto it = std::find(clients_.begin(), clients_.end(), client);
    if (it != clients_.end()) {
        clients_.erase(it);
    }
}

void Queue::updateClientPatience() {
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
