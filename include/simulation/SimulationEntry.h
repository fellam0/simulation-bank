#pragma once

// Encapsulates simulation entry parameters, matching the provided class diagram.
class SimulationEntry {
public:
    int getSimulationDuration() const { return simulationDuration_; }
    void setSimulationDuration(int value) { simulationDuration_ = value; }

    int getCashierCount() const { return cashierCount_; }
    void setCashierCount(int value) { cashierCount_ = value; }

    int getMinServiceTime() const { return minServiceTime_; }
    void setMinServiceTime(int value) { minServiceTime_ = value; }

    int getMaxServiceTime() const { return maxServiceTime_; }
    void setMaxServiceTime(int value) { maxServiceTime_ = value; }

    int getClientArrivalInterval() const { return clientArrivalInterval_; }
    void setClientArrivalInterval(int value) { clientArrivalInterval_ = value; }

    double getPriorityClientRate() const { return priorityClientRate_; }
    void setPriorityClientRate(double value) { priorityClientRate_ = value; }

    int getClientPatienceTime() const { return clientPatienceTime_; }
    void setClientPatienceTime(int value) { clientPatienceTime_ = value; }

private:
    int simulationDuration_ = 100;
    int cashierCount_ = 3;
    int minServiceTime_ = 2;
    int maxServiceTime_ = 8;
    int clientArrivalInterval_ = 4;
    double priorityClientRate_ = 0.15;
    int clientPatienceTime_ = 5;
};
