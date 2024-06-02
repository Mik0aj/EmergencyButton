#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ButtonSubject.cpp>
#include <Preferences.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
Preferences preferences;

class Button : public Subject {
    bool currentButtonState;
    bool lastButtonState;
    unsigned long debounceTime;
    unsigned long lastDebounceTime;

public:
    Button(unsigned long debounceTime) : debounceTime(debounceTime), lastDebounceTime(0) {}

    void checkState(bool incomingState) {
        static unsigned long lastCheckTime = 0;
        unsigned long currentTime = millis();
        this->currentButtonState=incomingState;
        if ((currentButtonState != lastButtonState) ) {


            notifyAll(); // Notify all observers about the change
        }
        lastDebounceTime = currentTime;
        lastButtonState = currentButtonState;

    }

    bool getCurrentButtonState() {
        return currentButtonState;
    }
};

class LCDObserver : public Observer {
private:
    LiquidCrystal_I2C* lcd;
    Button* button;
    int lastBacklightOnTime;

public:
    LCDObserver(LiquidCrystal_I2C* lcd, Button* button) : lcd(lcd), button(button) {
        button->subscribe(this); // Subscribe this observer to the button
    }

    ~LCDObserver() {
        button->unsubscribe(this); // Unsubscribe this observer from the button
    }

    void update() override {
        // Update logic here, e.g., displaying the button state on the LCD
        lcd->backlight(); 
        lastBacklightOnTime=millis();
        lcd->clear();
        lcd->setCursor(0, 0);
        lcd->print(button->getCurrentButtonState()? "Pressed" : "Released");
    }
    int getlastBacklightOnTime(){
      return lastBacklightOnTime;
    }
};

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

    void update() override {
        Serial.println(button->getCurrentButtonState()? "Pressed" : "Released");
    }

};

class NumberOfPresses : public Observer {
private:
    Button* button;
    Preferences* preferences; // Pointer to Preferences instance

public:
    NumberOfPresses(Preferences* preferences,Button* button) : preferences(preferences), button(button){
        button->subscribe(this); // Subscribe this observer to the button
        preferences->begin("buttonPressCounter", false); // Start preferences with a name
    }

    ~NumberOfPresses() {
        button->unsubscribe(this); // Unsubscribe this observer from the button
        preferences->end(); // End preferences when the object is destroyed
    }

    void update() override {
        static int pressCount = preferences->getInt("pressCount", 0); // Load the count from NVS
        if (button->getCurrentButtonState()) { // If the button is pressed
            pressCount++; // Increment the count
            preferences->putInt("pressCount", pressCount); // Save the updated count
            Serial.print("Button counter:");
            Serial.println(pressCount);
        }
    }
};


auto pin = 23;
auto currentTime = 0;
const int DEBOUNCE = 30;
Button button(DEBOUNCE);
LCDObserver lcdObserve(&lcd,&button);
SerialObserver serialObserve(&button);
NumberOfPresses numberOfPresses(&preferences,&button);

bool currentButtonState;
unsigned long backlightStartTime = 0;

void setup() {
  Wire.begin(); 
  preferences.begin("EmergencyButton", false); // Start preferences with a name

  pinMode(pin, INPUT_PULLUP); 
  Serial.begin(9600);

  lcd.begin(16, 2);
  lcd.backlight(); 
  lcd.setCursor(0, 0);
  lcd.print("EmergencyButton");
  delay(500);
  currentButtonState = digitalRead(pin) ; 
  backlightStartTime = millis(); 
}

void loop() {
  currentTime = millis(); 
  button.checkState(digitalRead(pin));

  if (millis() - lcdObserve.getlastBacklightOnTime() >= 5000) {
    lcd.clear();
    lcd.noBacklight(); 
  }

  delay(1); 
}
