#pragma once

#include <memory>
#include <vector>

#include "client/AbstractClient.h"

class StatisticManager {
public:
    void registerServedClient(std::shared_ptr<AbstractClient> client);
    void registerNonServedClient(std::shared_ptr<AbstractClient> client);

    void recordSimulationTick();
    void recordOccupiedCashier();

    int servedClientCount() const;
    int nonServedClientCount() const;

    double averageClientWaitingTime() const;
    double averageClientServiceTime() const;
    double averageCashierOccupationRate(int cashierCount) const;
    double clientSatisfactionRate() const;

    const std::vector<std::shared_ptr<AbstractClient>>& servedClients() const;
    const std::vector<std::shared_ptr<AbstractClient>>& nonServedClients() const;

private:
    std::vector<std::shared_ptr<AbstractClient>> servedClients_;
    std::vector<std::shared_ptr<AbstractClient>> nonServedClients_;
    int simulationDuration_ = 0;
    int occupiedCashierTicks_ = 0;
};
