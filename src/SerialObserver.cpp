#ifndef SERIAL_OBSERVER
#define SERIAL_OBSERVER

#include <Observer.cpp>
#include <Arduino.h>
#include <Button.cpp>

class SerialObserver : public Observer {
private:
    Button* button;

public:
    SerialObserver(Button* button) :  button(button) {
        button->subscribe(this); // Subscribe this observer to the button
    }

    ~SerialObserver() {
        button->unsubscribe(this); // Unsubscribe this observer from the button
    }

    void notifyPressed() override {
        Serial.println("Pressed");

    }

    void notifyReleased() override {
        Serial.print("Released ");
        Serial.println(button->getPressCount());
    }

};
#endif //SERIAL_OBSERVER