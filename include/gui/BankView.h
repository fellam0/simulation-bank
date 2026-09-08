#pragma once

#include <QWidget>

#include "simulation/Simulation.h"

// Draws the current state of a Simulation's Bank: one box per cashier
// (colored by what it is serving) and the waiting queue as a row of tokens
// (VIP clients highlighted). Call update() after each Simulation::step().
class BankView : public QWidget {
    Q_OBJECT

public:
    explicit BankView(QWidget* parent = nullptr);

    void attachSimulation(Simulation* simulation);

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    Simulation* simulation_ = nullptr;
};
