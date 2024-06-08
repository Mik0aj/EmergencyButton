#ifndef BUTTON
#define BUTTON

#include "Subject.cpp" // Include Subject header
#include <Preferences.h> // Include Preferences library

class Button : public Subject {
    bool currentButtonState;
    bool lastButtonState;
    unsigned long debounceTime;
    unsigned long lastDebounceTime;
    Preferences* preferences;
    int pressCount;

public:
    Button(unsigned long debounceTime, Preferences* preferences, bool currentButtonState, bool lastButtonState) : 
        debounceTime(debounceTime), preferences(preferences), 
        lastDebounceTime(0), currentButtonState(currentButtonState), lastButtonState(lastButtonState), 
        pressCount(0) {
        preferences->begin("EmergencyButton", false); // Call begin here
        pressCount = preferences->getInt("pressCount", pressCount);
    }

    ~Button() {
        preferences->end(); 
    }

    void checkState(bool incomingState) {
        unsigned long currentTime = millis();
        lastButtonState = currentButtonState;
        currentButtonState = incomingState;
        if ((currentTime - lastDebounceTime) > debounceTime) {
            if (currentButtonState != lastButtonState){
                lastDebounceTime = currentTime; 
                if (currentButtonState == HIGH) {
                    pressCount++; 
                    preferences->putInt("pressCount", pressCount);
                    notifyPressed(); 
                } else {
                    notifyReleased(); 
                }
            }
        }
    }

    bool getCurrentButtonState() {
        return currentButtonState;
    }

    int getPressCount() {
        return pressCount;
    }

    void setPressCount(int newPressCount) {
        preferences->putInt("pressCount", newPressCount);// does now work
    }
};

#endif /* BUTTON */
