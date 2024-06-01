#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ButtonSubject.cpp>

LiquidCrystal_I2C lcd(0x27, 16, 2);

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




auto pin = 23;
auto currentTime = 0;
const int debounce =30;
Button button(debounce);
LCDObserver lcdObserve(&lcd,&button);
SerialObserver serialObserve(&button);

bool currentButtonState;
unsigned long backlightStartTime = 0;

void setup() {
  Wire.begin(); 

  pinMode(pin, INPUT_PULLUP); 
  Serial.begin(9600);

  lcd.begin(16, 2);
  lcd.backlight(); 
  lcd.setCursor(0, 0);
  lcd.print("Latching Button");
  delay(500);
  currentButtonState = digitalRead(pin) ; 
  backlightStartTime = millis(); 
}

void loop() {
  currentTime = millis(); 
  button.checkState(digitalRead(pin) );

  if (millis() - lcdObserve.getlastBacklightOnTime() >= 5000) {
    lcd.clear();
    lcd.noBacklight(); 
  }

  delay(1); 
}
