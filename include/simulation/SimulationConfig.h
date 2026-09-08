#pragma once

struct SimulationConfig {
    int simulationDuration = 100;
    int cashierCount = 3;
    int minServiceTime = 2;
    int maxServiceTime = 8;
    int clientArrivalInterval = 5;
    double vipClientRate = 0.15;
    int clientPatienceTime = 5;
};
