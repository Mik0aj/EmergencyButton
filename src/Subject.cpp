#ifndef SUBJECT
#define SUBJECT
#include <Arduino.h>
#include <LinkedList.cpp>
#include <Observer.cpp>

class Subject {
    LinkedList<Observer> observers;

public:
    void subscribe(Observer* obs) {
        observers.append(obs);
    }

    void unsubscribe(Observer* obs) {
        observers.remove(obs);
    }

    void notifyPressed() {
        for (auto& obs : observers) {
            obs->notifyPressed();
        }
    }

    void notifyReleased() {
        for (auto& obs : observers) {
            obs->notifyReleased();
        }
    }
};



#endif /* SUBJECT */
