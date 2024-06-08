#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Button.cpp>
#include <Preferences.h>
#include <esp_random.h>
#include <SerialObserver.cpp>
#include <LCDObserver.cpp>

#define DISPLAY_TIMEOUT 5000
String machineList[] = {"Mid 12", "Mid 7", "Kamery", "Radary","Mid 10", "Mid 8","Mid 13","SMT"};
String reasonList[] = {"pasta sie przeterminowala","nie ma kalibracji na piecu","IPTE przyjechalo", "ITAC", "blad skrecania", "bezpiecznik wyskoczyl w piecu","zainstalowano McAfee","brakuje maski w Core2","nie uzywamy TestStanda"};

LiquidCrystal_I2C lcd(0x27, 16, 2);
Preferences preferences;

auto pin = 23;
auto currentTime = 0;
const int DEBOUNCE = 15;
bool currentButtonState;
unsigned long backlightStartTime = 0;

Button *button;
LCDObserver *lcdObserve;
SerialObserver *serialObserve;
SpecificMessageGenerator messageGenerator(machineList, sizeof(machineList) / sizeof(machineList[0]), reasonList, sizeof(reasonList) / sizeof(reasonList[0]));

void setup() {
  Wire.begin(); 
  pinMode(pin, INPUT_PULLUP); 
  Serial.begin(9600);
  currentButtonState = digitalRead(pin) ; 
  button = new Button(DEBOUNCE,&preferences,currentButtonState,currentButtonState);
  lcdObserve = new LCDObserver(&lcd,button,&messageGenerator,DISPLAY_TIMEOUT);
  serialObserve = new SerialObserver(button);
  lcd.begin(16, 2);
  backlightStartTime = millis();
  lcdObserve->displayString("Emergency Button");
}

void loop() {
  currentTime = millis(); 
  button->checkState(digitalRead(pin));
  lcdObserve->run(currentTime);
}
