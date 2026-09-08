#pragma once

#include <string>

#include "simulation/Bank.h"
#include "simulation/SimulationEntry.h"
#include "simulation/StatisticManager.h"

// Access point of all simulation-related information (matches the provided
// class diagram: Simulation -> Bank/StatisticManager/SimulationEntry,
// uses SimulationUtility, creates AbstractClient/AbstractOperation).
class Simulation {
public:
    explicit Simulation(SimulationEntry entry);

    // Runs the simulation to completion in one call (matches the diagram's
    // simulate(): void).
    void simulate();

    // Advances the simulation by a single time unit; returns false once the
    // configured duration has been reached. This is an extension beyond the
    // diagram needed to drive the real-time GUI tick by tick (requirement 3)
    // while keeping the exact same logic simulate() uses internally.
    bool step();

    int currentTime() const;
    bool isFinished() const;

    Bank& bank();
    const StatisticManager& statisticManager() const;
    const SimulationEntry& entry() const;

    std::string resultsSummary() const;

private:
    void updateBank();
    void handleNewClientArrival();
    void serveClient(Cashier& cashier, std::shared_ptr<AbstractClient> client);

    SimulationEntry entry_;
    Bank bank_;
    StatisticManager statisticManager_;
    int currentTime_ = 0;
    int nextClientId_ = 1;
};
