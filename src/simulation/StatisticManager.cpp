#include "simulation/StatisticManager.h"

void StatisticManager::registerServedClient(std::shared_ptr<AbstractClient> client) {
    servedClients_.push_back(std::move(client));
}

void StatisticManager::registerNonServedClient(std::shared_ptr<AbstractClient> client) {
    nonServedClients_.push_back(std::move(client));
}

void StatisticManager::recordSimulationTick() {
    simulationDuration_++;
}

void StatisticManager::recordOccupiedCashier() {
    occupiedCashierTicks_++;
}

int StatisticManager::servedClientCount() const {
    return static_cast<int>(servedClients_.size());
}

int StatisticManager::nonServedClientCount() const {
    return static_cast<int>(nonServedClients_.size());
}

double StatisticManager::averageClientWaitingTime() const {
    if (servedClients_.empty()) {
        return 0.0;
    }
    long long totalWaitingTime = 0;
    for (const auto& client : servedClients_) {
        totalWaitingTime += client->getServiceStartTime() - client->getArrivalTime();
    }
    return static_cast<double>(totalWaitingTime) / servedClients_.size();
}

double StatisticManager::averageClientServiceTime() const {
    if (servedClients_.empty()) {
        return 0.0;
    }
    long long totalServiceTime = 0;
    for (const auto& client : servedClients_) {
        totalServiceTime += client->getDepartureTime() - client->getServiceStartTime();
    }
    return static_cast<double>(totalServiceTime) / servedClients_.size();
}

double StatisticManager::averageCashierOccupationRate(int cashierCount) const {
    if (simulationDuration_ == 0 || cashierCount == 0) {
        return 0.0;
    }
    return (occupiedCashierTicks_ * 100.0 / simulationDuration_) / cashierCount;
}

double StatisticManager::clientSatisfactionRate() const {
    int total = servedClientCount() + nonServedClientCount();
    if (total == 0) {
        return 100.0;
    }
    return servedClientCount() * 100.0 / total;
}

const std::vector<std::shared_ptr<AbstractClient>>& StatisticManager::servedClients() const {
    return servedClients_;
}

const std::vector<std::shared_ptr<AbstractClient>>& StatisticManager::nonServedClients() const {
    return nonServedClients_;
}
