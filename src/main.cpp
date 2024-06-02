#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ButtonSubject.cpp>
#include <Preferences.h>
#include <esp_random.h>
#define CHUNK_SIZE 16
#define DISPLAY_TIMEOUT 5000
LiquidCrystal_I2C lcd(0x27, 16, 2);
Preferences preferences;

class GenerateMessage {
public:
  virtual String getMessage() {
    return "Default message"; 
  }
};

class SpecificMessageGenerator : public GenerateMessage {
private:
  String* machines;
  size_t numMachines;
  String* reasons;
  size_t numReasons;

public:
  // Constructor to initialize with machine and reason arrays
  SpecificMessageGenerator(String machines[], size_t numMachines, String reasons[], size_t numReasons) :
    machines(machines), numMachines(numMachines), reasons(reasons), numReasons(numReasons) {}

  String getMessage() override {
    int randomMachineIndex = random(0, numMachines);
    int randomReasonIndex = random(0, numReasons);
    // Format the message using placeholders
    return machines[randomMachineIndex] + " nie dziala bo " + reasons[randomReasonIndex];
  }
};

class Button : public Subject {
    bool currentButtonState;
    bool lastButtonState;
    unsigned long debounceTime;
    unsigned long lastDebounceTime;
    Preferences *preferences;
    int pressCount; 

public:
  Button(unsigned long debounceTime, Preferences* preferences) : 
    debounceTime(debounceTime), preferences(preferences), lastDebounceTime(0) {
    Serial.println("Button: Begin preferences"); // Debug message

    preferences->begin("EmergencyButton", false); // Call begin here
    pressCount = preferences->getInt("pressCount", 0); 
  }

    ~Button() {
        preferences->end(); 
    }

    void checkState(bool incomingState) {
        static unsigned long lastCheckTime = 0;
        unsigned long currentTime = millis();
        this->currentButtonState=incomingState;
        if ((currentButtonState != lastButtonState)&&((currentTime-lastDebounceTime)>debounceTime)) {
            if(currentButtonState == LOW ){
                pressCount++; // Increment the count
                preferences->putInt("pressCount", pressCount);
            }
            notifyAll(); // Notify all observers about the change
            lastDebounceTime = currentTime;
        }
        lastButtonState = currentButtonState;

    }

    bool getCurrentButtonState() {
        return currentButtonState;
    }
    int getPressCount() {
        return pressCount;
    }
};

class LCDObserver : public Observer {
private:
    LiquidCrystal_I2C* lcd;
    Button* button;
    int lastBacklightOnTime;
    GenerateMessage* messageGenerator; 


public:
    LCDObserver(LiquidCrystal_I2C* lcd, Button* button, GenerateMessage* messageGenerator) :
        lcd(lcd), button(button), lastBacklightOnTime(millis()), messageGenerator(messageGenerator) {
        button->subscribe(this); // Subscribe this observer to the button
    }

    ~LCDObserver() {
        button->unsubscribe(this); // Unsubscribe this observer from the button
    }

    void update() override {
        // Update logic here, e.g., displaying the button state on the LCD
        lcd->backlight(); 
        lastBacklightOnTime = millis();
        lcd->clear();
        lcd->setCursor(0, 0);
        
        if (button->getCurrentButtonState()) {
            String message = messageGenerator->getMessage();
            int chunkCount = 0;
            int maxChunks = (message.length() + CHUNK_SIZE - 1) / CHUNK_SIZE;
            String* chunks = new String[maxChunks];
            splitStringIntoChunks(message, chunks, &chunkCount);
            
            for (int i = 0; i < chunkCount; i += 2) {
                lcd->clear();  // Clear the display before printing new lines
                lcd->setCursor(0, 0);
                lcd->print(chunks[i]);
                
                if (i + 1 < chunkCount) {
                    lcd->setCursor(0, 1);  // Correctly setting cursor for second line
                    lcd->print(chunks[i + 1]);
                }
                
                delay(DISPLAY_TIMEOUT / chunkCount);
            }
            
            delete[] chunks;
        } else {
            auto pressCount=button->getPressCount();
            if (pressCount==10 || pressCount==25 || pressCount==50 || pressCount%100==0){
                String message = "Gratulacje z okzaji " + String(pressCount) + " awarii";
                int chunkCount = 0;
                int maxChunks = (message.length() + CHUNK_SIZE - 1) / CHUNK_SIZE;
                String* chunks = new String[maxChunks];
                splitStringIntoChunks(message, chunks, &chunkCount);
                for (int i = 0; i < chunkCount; i += 2) {
                    lcd->clear();  // Clear the display before printing new lines
                    lcd->setCursor(0, 0);
                    lcd->print(chunks[i]);
                    
                    if (i + 1 < chunkCount) {
                        lcd->setCursor(0, 1);  // Correctly setting cursor for second line
                        lcd->print(chunks[i + 1]);
                    }
                    
                    delay(DISPLAY_TIMEOUT / chunkCount);
                }
            }
            else{
                String message = "Odnotowano " + String(pressCount) + " awarii";
                int chunkCount = 0;
                int maxChunks = (message.length() + CHUNK_SIZE - 1) / CHUNK_SIZE;
                String* chunks = new String[maxChunks];
                splitStringIntoChunks(message, chunks, &chunkCount);

                for (int i = 0; i < chunkCount; i += 2) {
                    lcd->clear();  // Clear the display before printing new lines
                    lcd->setCursor(0, 0);
                    lcd->print(chunks[i]);
                    
                    if (i + 1 < chunkCount) {
                        lcd->setCursor(0, 1);  // Correctly setting cursor for second line
                        lcd->print(chunks[i + 1]);
                    }
                    
                    delay(DISPLAY_TIMEOUT / chunkCount);
                }
            }
        }
    }
    void splitStringIntoChunks(const String& str, String* chunks, int* chunkCount) {
            // Calculate the length of the input string
            int strLen = str.length();

            // Calculate the number of chunks needed
            *chunkCount = (strLen + CHUNK_SIZE - 1) / CHUNK_SIZE;

            // Split the string into chunks
            for (int i = 0; i < *chunkCount; ++i) {
                chunks[i] = str.substring(i * CHUNK_SIZE, i * CHUNK_SIZE + CHUNK_SIZE);
            }
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
        if(button->getCurrentButtonState()){
            Serial.println("Pressed");
        }
        else{
            Serial.print("Released ");
            Serial.println(button->getPressCount());
        }
    }

};

auto pin = 23;
auto currentTime = 0;
const int DEBOUNCE = 10;
Button *button;
LCDObserver *lcdObserve;
SerialObserver *serialObserve;
String machineList[] = {"Mid 12", "Mid 7", "Kamery", "Radary"};
String reasonList[] = {"IPTE przyjechalo", "ITAC", "blad skrecania", "bezpiecznik wyskoczyl","zainstalowano McAfee","brak wartości dodanej"};

SpecificMessageGenerator messageGenerator(machineList, sizeof(machineList) / sizeof(machineList[0]), reasonList, sizeof(reasonList) / sizeof(reasonList[0]));

bool currentButtonState;
unsigned long backlightStartTime = 0;

void setup() {
  Wire.begin(); 
  pinMode(pin, INPUT_PULLUP); 
  Serial.begin(9600);
  button = new Button(DEBOUNCE,&preferences);
  lcdObserve = new LCDObserver(&lcd,button,&messageGenerator);
  serialObserve = new SerialObserver(button);
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
  button->checkState(digitalRead(pin));
  if (millis() - lcdObserve->getlastBacklightOnTime() >= DISPLAY_TIMEOUT) {
    lcd.clear();
    lcd.noBacklight(); 
  }
}
