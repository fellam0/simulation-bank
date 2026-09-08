#include <QCoreApplication>
#include <QDir>
#include <QFile>

#include <cmath>
#include <iostream>
#include <string>

#include "client/Client.h"
#include "client/Consultation.h"
#include "client/Transfer.h"
#include "client/VIPClient.h"
#include "client/Withdraw.h"
#include "persistence/StatisticRepository.h"
#include "simulation/Bank.h"
#include "simulation/Queue.h"
#include "simulation/Simulation.h"
#include "simulation/SimulationUtility.h"
#include "simulation/StatisticManager.h"

namespace {

int g_total = 0;
int g_failures = 0;

void check(bool condition, const std::string& description) {
    g_total++;
    if (condition) {
        std::cout << "[PASS] " << description << "\n";
    } else {
        g_failures++;
        std::cout << "[FAIL] " << description << "\n";
    }
}

void checkNear(double actual, double expected, double tolerance, const std::string& description) {
    check(std::fabs(actual - expected) <= tolerance,
          description + " (attendu ~" + std::to_string(expected) + ", obtenu " + std::to_string(actual) + ")");
}

void testClockBoundaries() {
    std::cout << "\n-- Horloge : 0..duree inclus --\n";
    SimulationEntry entry;
    entry.setSimulationDuration(10);
    entry.setCashierCount(1);
    entry.setClientArrivalInterval(1000);
    entry.setMinServiceTime(1);
    entry.setMaxServiceTime(1);

    Simulation sim(entry);
    int ticks = 0;
    while (sim.step()) {
        ticks++;
    }
    check(ticks == 11, "step() traite exactement duree+1 = 11 instants (0..10 inclus)");
    check(sim.isFinished(), "La simulation est marquee terminee apres la derniere iteration");
}

void testArrivalTiming() {
    std::cout << "\n-- Arrivees uniformement reparties tous les N --\n";
    SimulationEntry entry;
    entry.setSimulationDuration(20);
    entry.setCashierCount(100);
    entry.setClientArrivalInterval(5);
    entry.setMinServiceTime(1000);
    entry.setMaxServiceTime(1000);
    entry.setPriorityClientRate(0.0);

    Simulation sim(entry);
    sim.simulate();

    int busyCashiers = 0;
    for (Cashier& cashier : sim.bank().getCashiers()) {
        if (!cashier.isFree()) {
            busyCashiers++;
        }
    }
    check(busyCashiers == 5, "5 arrivees attendues aux instants 0,5,10,15,20 (intervalle=5, duree=20)");
}

void testFreeCashierAndQueueing() {
    std::cout << "\n-- Caissier libre prioritaire, sinon file d'attente --\n";
    SimulationEntry entry;
    entry.setSimulationDuration(5);
    entry.setCashierCount(1);
    entry.setClientArrivalInterval(1);
    entry.setMinServiceTime(5);
    entry.setMaxServiceTime(5);
    entry.setPriorityClientRate(0.0);

    Simulation sim(entry);
    sim.step();
    check(!sim.bank().getCashiers()[0].isFree(), "Le premier client arrivant sert immediatement le caissier libre");
    check(sim.bank().getQueue().isEmpty(), "File vide tant qu'un caissier est libre");

    sim.step();
    check(sim.bank().getQueue().size() == 1, "Le client suivant est mis en file quand le caissier est occupe");
}

void testCashierCountConfigurable() {
    std::cout << "\n-- Nombre de caissiers parametrable --\n";
    for (int n : {1, 3, 7}) {
        Bank bank(n);
        check(bank.getCashiers().size() == static_cast<std::size_t>(n),
              "Bank(" + std::to_string(n) + ") cree bien " + std::to_string(n) + " caissiers");
    }
}

void testServiceTimeBounds() {
    std::cout << "\n-- Temps de service aleatoire entre min et max --\n";
    const int kMin = 3, kMax = 7, kSamples = 5000;
    bool allInRange = true;
    int minSeen = 1000, maxSeen = -1000;
    for (int i = 0; i < kSamples; i++) {
        int t = SimulationUtility::getRandomServiceTime(kMin, kMax);
        if (t < kMin || t > kMax) allInRange = false;
        minSeen = std::min(minSeen, t);
        maxSeen = std::max(maxSeen, t);
    }
    check(allInRange, "Tous les temps de service generes restent dans [min,max] sur " + std::to_string(kSamples) + " tirages");
    check(minSeen == kMin, "La borne min (" + std::to_string(kMin) + ") est atteinte sur l'echantillon");
    check(maxSeen == kMax, "La borne max (" + std::to_string(kMax) + ") est atteinte sur l'echantillon");
}

void testOperationDistribution() {
    std::cout << "\n-- 3 types d'operation, probabilite 1/3 chacun --\n";
    const int kSamples = 30000;
    int consultation = 0, transfer = 0, withdraw = 0;
    for (int i = 0; i < kSamples; i++) {
        auto op = SimulationUtility::getRandomOperation();
        if (dynamic_cast<Consultation*>(op.get())) consultation++;
        else if (dynamic_cast<Transfer*>(op.get())) transfer++;
        else if (dynamic_cast<Withdraw*>(op.get())) withdraw++;
    }
    double expected = kSamples / 3.0;
    double tolerance = kSamples * 0.03; // 3% de marge statistique
    checkNear(consultation, expected, tolerance, "Proportion de Consultation proche de 1/3");
    checkNear(transfer, expected, tolerance, "Proportion de Transfer proche de 1/3");
    checkNear(withdraw, expected, tolerance, "Proportion de Withdraw proche de 1/3");
}

void testVipRateConfigurable() {
    std::cout << "\n-- Taux de clients VIP parametrable --\n";
    const double kRate = 0.3;
    const int kSamples = 30000;
    int hits = 0;
    for (int i = 0; i < kSamples; i++) {
        if (SimulationUtility::isPriorityClient(kRate)) hits++;
    }
    double observed = static_cast<double>(hits) / kSamples;
    checkNear(observed, kRate, 0.02, "Taux VIP observe proche du taux configure (" + std::to_string(kRate) + ")");
}

void testImpatienceForNonUrgentOnly() {
    std::cout << "\n-- Impatience : seulement pour les operations non urgentes --\n";

    Client consultationClient(1, 0, std::make_unique<Consultation>(5), 3);
    check(consultationClient.isPatient(), "Client Consultation patient initialement (patience=3)");
    consultationClient.reducePatience();
    consultationClient.reducePatience();
    check(consultationClient.isPatient(), "Client Consultation encore patient apres 2 reductions (patience=1)");
    consultationClient.reducePatience();
    check(!consultationClient.isPatient(), "Client Consultation devient impatient une fois la patience epuisee");

    Client transferClient(2, 0, std::make_unique<Transfer>(5), 0);
    check(transferClient.isPatient(), "Client Virement (urgent) reste patient meme avec patience=0");

    Client withdrawClient(3, 0, std::make_unique<Withdraw>(5), 0);
    check(withdrawClient.isPatient(), "Client Retrait (urgent) reste patient meme avec patience=0");

    VIPClient vipConsultation(4, 0, std::make_unique<Consultation>(5), 1);
    vipConsultation.reducePatience();
    check(!vipConsultation.isPatient(), "Un VIP avec une Consultation peut quand meme devenir impatient (VIP = priorite, pas patience infinie)");
}

void testVipPriorityClosestToHead() {
    std::cout << "\n-- VIP prioritaire : le plus proche de la tete de file --\n";
    Queue queue;
    auto normalA = std::make_shared<Client>(1, 0, std::make_unique<Consultation>(1), 5);
    auto vipX = std::make_shared<VIPClient>(2, 0, std::make_unique<Consultation>(1), 5);
    auto normalB = std::make_shared<Client>(3, 0, std::make_unique<Consultation>(1), 5);
    auto vipY = std::make_shared<VIPClient>(4, 0, std::make_unique<Consultation>(1), 5);

    queue.addQueueLast(normalA);
    queue.addQueueLast(vipX);
    queue.addQueueLast(normalB);
    queue.addQueueLast(vipY);

    auto found = queue.findPriorityClient();
    check(found != nullptr && found->getId() == 2, "findPriorityClient() renvoie le VIP le plus proche de la tete (VIP_X, pas VIP_Y)");

    queue.removePriorityClient(found);
    check(queue.size() == 3, "removePriorityClient() retire bien le VIP trouve");

    auto second = queue.findPriorityClient();
    check(second != nullptr && second->getId() == 4, "Le VIP restant (VIP_Y) est trouve apres retrait du premier");
}

void testFifoWhenNoVip() {
    std::cout << "\n-- FIFO quand aucun VIP en file --\n";
    Queue queue;
    auto clientA = std::make_shared<Client>(1, 0, std::make_unique<Consultation>(1), 5);
    auto clientB = std::make_shared<Client>(2, 0, std::make_unique<Consultation>(1), 5);
    queue.addQueueLast(clientA);
    queue.addQueueLast(clientB);

    check(queue.findPriorityClient() == nullptr, "Aucun VIP trouve quand la file n'en contient pas");
    auto first = queue.getQueueFirst();
    check(first != nullptr && first->getId() == 1, "getQueueFirst() respecte l'ordre FIFO");
}

void testStatisticsMath() {
    std::cout << "\n-- Calculs statistiques --\n";
    StatisticManager stats;

    auto client1 = std::make_shared<Client>(1, 0, std::make_unique<Consultation>(4), 5);
    client1->setServiceStartTime(2);
    client1->setDepartureTime(6);
    stats.registerServedClient(client1);

    auto client2 = std::make_shared<Client>(2, 5, std::make_unique<Consultation>(4), 5);
    client2->setServiceStartTime(5);
    client2->setDepartureTime(9);
    stats.registerServedClient(client2);

    auto client3 = std::make_shared<Client>(3, 1, std::make_unique<Consultation>(4), 5);
    stats.registerNonServedClient(client3);

    checkNear(stats.averageClientWaitingTime(), 1.0, 0.001, "Temps d'attente moyen = ((2-0)+(5-5))/2 = 1.0");
    checkNear(stats.averageClientServiceTime(), 4.0, 0.001, "Temps de service moyen = ((6-2)+(9-5))/2 = 4.0");
    checkNear(stats.clientSatisfactionRate(), 200.0 / 3.0, 0.001, "Taux de satisfaction = 2 servis / 3 total = 66.67%");

    for (int i = 0; i < 10; i++) stats.recordSimulationTick();
    for (int i = 0; i < 5; i++) stats.recordOccupiedCashier();
    checkNear(stats.averageCashierOccupationRate(1), 50.0, 0.001, "Taux d'occupation = (5*100/10)/1 caissier = 50%");
}

void testDatabaseRoundTrip() {
    std::cout << "\n-- Persistance SQLite : ecriture puis relecture --\n";
    const QString dbPath = QDir::temp().filePath("simulation_bank_spec_test.db");
    QFile::remove(dbPath);

    StatisticRepository repository;
    check(repository.open(dbPath), "Ouverture/creation de la base SQLite temporaire");

    SimulationEntry entry;
    entry.setSimulationDuration(42);
    entry.setCashierCount(4);
    entry.setPriorityClientRate(0.25);

    StatisticManager stats;
    auto client1 = std::make_shared<VIPClient>(1, 0, std::make_unique<Transfer>(3), 5);
    client1->setServiceStartTime(0);
    client1->setDepartureTime(3);
    stats.registerServedClient(client1);

    int runId = repository.saveRun(entry, stats);
    check(runId > 0, "saveRun() renvoie un identifiant de run valide");

    auto runs = repository.listRuns();
    auto it = std::find_if(runs.begin(), runs.end(), [runId](const RunSummary& r) { return r.id == runId; });
    check(it != runs.end(), "Le run enregistre est bien relu par listRuns()");
    if (it != runs.end()) {
        check(it->entry.getSimulationDuration() == 42, "La duree relue correspond a celle enregistree");
        check(it->entry.getCashierCount() == 4, "Le nombre de caissiers relu correspond");
        checkNear(it->entry.getPriorityClientRate(), 0.25, 0.001, "Le taux VIP relu correspond");
        check(it->servedCount == 1, "Le nombre de clients servis relu correspond");
    }

    auto clients = repository.clientsForRun(runId);
    check(clients.size() == 1, "clientsForRun() relit bien le detail par client");
    if (clients.size() == 1) {
        check(clients[0].clientType == "VIP", "Le type de client (VIP) est correctement relu");
        check(clients[0].operationType == "Virement", "Le type d'operation est correctement relu");
        check(clients[0].served, "Le flag 'servi' est correctement relu");
    }

    QFile::remove(dbPath);
}

}

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);

    testClockBoundaries();
    testArrivalTiming();
    testFreeCashierAndQueueing();
    testCashierCountConfigurable();
    testServiceTimeBounds();
    testOperationDistribution();
    testVipRateConfigurable();
    testImpatienceForNonUrgentOnly();
    testVipPriorityClosestToHead();
    testFifoWhenNoVip();
    testStatisticsMath();
    testDatabaseRoundTrip();

    std::cout << "\n==========================================\n";
    std::cout << g_total - g_failures << " / " << g_total << " tests reussis\n";
    if (g_failures > 0) {
        std::cout << g_failures << " ECHEC(S)\n";
    }
    std::cout << "==========================================\n";

    return g_failures == 0 ? 0 : 1;
}
