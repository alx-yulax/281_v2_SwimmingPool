#include <iostream>
#include <thread>
#include <assert.h>
#include <mutex>

std::mutex access;


class Swimmer {
    std::string name;
    int speed;
    int resultSec;
public:
    Swimmer(std::string inName, int inSpeed) : name(inName), speed(inSpeed), resultSec(0) {
        assert(inSpeed > 0);
    }

    std::string getName() {
        return name;
    }

    int getSpeed() {
        return speed;
    }

    int getResult() {
        return resultSec;
    }

    void setResult(int inResultSec) {
        assert(inResultSec > 0);
        resultSec = inResultSec;
    }
};

class SwimmingPool {
    Swimmer *swimmers[6] = {nullptr};
public:
    ~SwimmingPool() {
        for (int i = 0; i < 6; ++i) {
            delete swimmers[i];
            swimmers[i] = nullptr;
        }
    }

    void load(int laneNumber, std::string name, int speed) {
        assert(laneNumber >= 0 && laneNumber < 6);
        swimmers[laneNumber] = new Swimmer(name, speed);
    }

    bool scoring(int durationSec, int i) {
        int swam = swimmers[i]->getSpeed() * durationSec;
        if (swimmers[i]->getResult() == 0) {
            if (swam >= 100) {
                swimmers[i]->setResult(durationSec);
                access.lock();
                std::cout << swimmers[i]->getName() << " finished" << std::endl;
                access.unlock();
            } else {
                access.lock();
                std::cout << swimmers[i]->getName() << " swam " << swam << " m." << std::endl;
                access.unlock();
                return true;
            }
        } else {
            access.lock();
            std::cout << swimmers[i]->getName() << " finished" << std::endl;
            access.unlock();
        }
        return false;
    }

    void showResults() {
        for (int i = 0; i < 6 - 1; i++) {
            for (int j = 0; j < 6 - i - 1; j++) {
                if (swimmers[j]->getResult() > swimmers[j + 1]->getResult()) {
                    Swimmer *temp = swimmers[j];
                    swimmers[j] = swimmers[j + 1];
                    swimmers[j + 1] = temp;
                }
            }
        }
        std::cout << std::endl;
        std::cout << "----------------- competition results: -------------------" << std::endl;
        int place = 0, time = 0;
        for (int i = 0; i < 6; ++i) {
            if (time != swimmers[i]->getResult()) {
                time = swimmers[i]->getResult();
                place++;
            }
            std::cout << "#" << place << "  " << swimmers[i]->getName() << "  " << swimmers[i]->getResult() << " sec."
                      << std::endl;
        }
    }
};

void competition(SwimmingPool *swimmingPool, int i) {
    int durationSec = 0;
    do {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        durationSec++;

    } while (swimmingPool->scoring(durationSec, i));
}

int main() {
    SwimmingPool *swimmingPool = new SwimmingPool();
    std::thread threads[6];
    for (int i = 0; i < 6; ++i) {
        std::string name;
        int speed;
        std::cout << "Swimmer #" << i << " Input name and speed: ";
        std::cin >> name >> speed;
        swimmingPool->load(i, name, speed);
    }

    for (int i = 0; i < 6; ++i) {
        threads[i] = std::thread(competition, swimmingPool, i);
    }

    for (int i = 0; i < 6; ++i) {
        threads[i].join();
    }

    swimmingPool->showResults();

    delete swimmingPool;
    swimmingPool = nullptr;
    return 0;
}
