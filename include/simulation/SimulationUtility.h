#pragma once

#include <memory>

#include "client/AbstractOperation.h"

// Static helpers used by Simulation to generate randomness, matching the
// provided class diagram.
class SimulationUtility {
public:
    static int getRandomServiceTime(int min, int max);
    static bool isPriorityClient(double priorityClientRate);
    static std::unique_ptr<AbstractOperation> getRandomOperation();
};
