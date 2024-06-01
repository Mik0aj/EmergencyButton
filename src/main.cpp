#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

auto button = 23;

auto currentTime = 0;
const auto debounce =30;
bool currentButtonState;
unsigned long backlightStartTime = 0;

void setup() {
  Wire.begin(); 

  pinMode(button, INPUT_PULLUP); 
  Serial.begin(9600);

  lcd.begin(16, 2);
  lcd.backlight(); 
  lcd.setCursor(0, 0);
  lcd.print("Latching Button");
  delay(500);
  currentButtonState = digitalRead(button) == LOW; 
  backlightStartTime = millis(); 
}

void loop() {
  currentTime = millis(); 
  static bool lastButtonState = HIGH; 
  currentButtonState = digitalRead(button) == LOW; 

  if ((currentButtonState != lastButtonState )&& (millis() >= debounce)) {

    lastButtonState = currentButtonState; 

    if (currentButtonState) {
      lcd.backlight();
      Serial.println("not pressed");
      lcd.clear();
      lcd.print("not pressed");
      backlightStartTime = millis(); 
    } else {
      lcd.backlight();
      Serial.println("pressed");
      lcd.clear();
      lcd.print("pressed");
      backlightStartTime = millis(); 
    }
  }
  if (millis() - backlightStartTime >= 5000) {
    lcd.clear();
    lcd.noBacklight(); 
  }

  delay(1); 
}
