#ifndef BUTTON_STATE
#define BUTTON_STATE

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

    void notifyAll() {
        for (auto& obs : observers) {
            obs->update();
        }
    }
};



#endif /* BUTTON_STATE */
