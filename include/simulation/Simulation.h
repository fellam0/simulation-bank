#pragma once

#include <string>

#include "simulation/Bank.h"
#include "simulation/SimulationConfig.h"
#include "simulation/StatisticManager.h"

class Simulation {
public:
    explicit Simulation(SimulationConfig config);

    // Advances the simulation by one time unit. Returns false once the
    // configured duration has been reached (nothing to do afterwards).
    bool step();

    int currentTime() const;
    bool isFinished() const;

    Bank& bank();
    const StatisticManager& statisticManager() const;
    const SimulationConfig& config() const;

    std::string resultsSummary() const;

private:
    void serveClient(Cashier& cashier, std::shared_ptr<AbstractClient> client);

    SimulationConfig config_;
    Bank bank_;
    StatisticManager statisticManager_;
    int currentTime_ = 0;
    int nextClientId_ = 1;
};
