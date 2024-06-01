#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

auto button = 23;

auto currentTime = 0;
const auto debounce =30;
bool currentButtonState;
unsigned long backlightStartTime = 0; // Stores backlight turn-on time

void setup() {
  Wire.begin(); 

  pinMode(button, INPUT_PULLUP); // Set the button pin as input with pull-up resistor
  Serial.begin(9600); // Start serial communication at 115200 baud rate

  // Initialize the LCD
  lcd.begin(16, 2);
  lcd.backlight(); // Turn backlight on initially
  lcd.setCursor(0, 0);
  lcd.print("Latching Button");
  delay(500);
  currentButtonState = digitalRead(button) == LOW; // Read actual button state (LOW for pressed)

  backlightStartTime = millis(); // Record backlight turn-on time
}

void loop() {
  currentTime = millis(); // Keep track of time for debouncing (if needed)
  static bool lastButtonState = HIGH; // Store previous button state
  currentButtonState = digitalRead(button) == LOW; // Read actual button state (LOW for pressed)

  // Check for button state change and debounce time (optional)
  if ((currentButtonState != lastButtonState )&& (millis() >= debounce)) {
    // Debounce logic can be added here (see previous discussions)

    lastButtonState = currentButtonState; // Update lastButtonState

    if (currentButtonState) {
      lcd.backlight();
      Serial.println("not pressed");
      lcd.clear();
      lcd.print("not pressed");
      backlightStartTime = millis(); // Reset backlight timer on button press
    } else {
      lcd.backlight();
      Serial.println("pressed");
      lcd.clear();
      lcd.print("pressed");
      backlightStartTime = millis(); // Reset backlight timer on button press
    }
  }
  // Turn off backlight after 5 seconds
  if (millis() - backlightStartTime >= 5000) {
    lcd.clear();
    lcd.noBacklight(); // Turn backlight off
  }

  delay(1); // Short delay to reduce CPU load
}
