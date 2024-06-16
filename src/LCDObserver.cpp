#ifndef LCD_OBSERVER
#define LCD_OBSERVER

#include <Observer.cpp>
#include <Arduino.h>
#include <Button.cpp>
#include <LiquidCrystal_I2C.h>
#include <MessageGenerators.cpp>

class LCDObserver : public Observer {
private:
    LiquidCrystal_I2C* lcd;
    Button* button;
    GenerateMessage* messageGenerator; 
    int timeWhenBaclightWasTriggered;
    int displayTimeout;
    String* chunks = nullptr;
    int chunkCount = 0;
    int maxChunks = 0;
    int currentChunkIndex = 0;
    int numberOfDisplayColumns = 16;
    int numberOfDisplayRows=2;

public:
    LCDObserver(LiquidCrystal_I2C* lcd, Button* button, GenerateMessage* messageGenerator,int displayTimeout) :
        lcd(lcd), button(button), messageGenerator(messageGenerator), timeWhenBaclightWasTriggered(millis()), displayTimeout(displayTimeout) {
        if (button)
            button->subscribe(this); 
    }

    ~LCDObserver() {
        delete[] chunks; 
        if (button)
            button->unsubscribe(this); 
    }

    void notifyPressed() override {
        lcd->backlight(); 
        timeWhenBaclightWasTriggered = millis();
        lcd->clear();
        lcd->setCursor(0, 0);
        auto message = messageGenerator->getMessage();
        updateChunks(message);
    }

    void notifyReleased() override {
        lcd->backlight(); 
        timeWhenBaclightWasTriggered = millis();
        lcd->clear();
        lcd->setCursor(0, 0);
        int pressCount = button->getPressCount();
        String message;
        if (isMilestone(pressCount)) {
            message = "Gratulacje z okzaji " + String(pressCount) + " awarii";
        } else {
            message = "Odnotowano " + String(pressCount) + " awarii";
        }
        updateChunks(message);
    }

    void setDisplayTimeout(int displayTimeout) {
        this->displayTimeout = displayTimeout;
    }
    
    void run(int timer) {
        if (chunks != nullptr) {
            auto BacklightOnTime = (timer - timeWhenBaclightWasTriggered);
            if (BacklightOnTime >= displayTimeout) {
                clearAndDisableBacklight();
            } else if (currentChunkIndex < maxChunks && BacklightOnTime > ((displayTimeout / maxChunks) * currentChunkIndex)) {
                lcd->clear();  
                lcd->setCursor(0, 0);
                lcd->print(chunks[currentChunkIndex++]);
                if (currentChunkIndex < maxChunks) {
                    lcd->setCursor(0, 1);
                    lcd->print(chunks[currentChunkIndex++]);
                }
            }
        }
    }

    void displayString(const String& str){
        lcd->backlight(); 
        timeWhenBaclightWasTriggered = millis();
        updateChunks(str);
    }

private:

    void updateChunks(const String& str) {
        clearChunks();
        int strLen = str.length();
        chunkCount = (strLen + numberOfDisplayColumns*numberOfDisplayRows - 1) / (numberOfDisplayColumns*numberOfDisplayRows);
        chunks = new String[chunkCount * numberOfDisplayRows]; 
        for (int i = 0; i < chunkCount; ++i) {
            chunks[i * 2] = str.substring(i * numberOfDisplayColumns*numberOfDisplayRows, i * numberOfDisplayColumns*numberOfDisplayRows + numberOfDisplayColumns);
            chunks[i * 2 + 1] = str.substring(i * numberOfDisplayColumns*numberOfDisplayRows + numberOfDisplayColumns, (i + 1) * numberOfDisplayColumns*numberOfDisplayRows);
        }
        maxChunks = chunkCount * numberOfDisplayRows; 
        currentChunkIndex = 0;
    }

    void clearChunks() {
        delete[] chunks;
        chunks = nullptr;
        chunkCount = 0;
        maxChunks = 0;
        currentChunkIndex = 0;
    }

    bool isMilestone(int pressCount) {
        // return pressCount == 10 || pressCount == 25 || pressCount == 50 || pressCount % 100 == 0;
        return pressCount % 10 == 0;
    }

    void clearAndDisableBacklight() {
        lcd->clear();
        lcd->noBacklight();
        clearChunks();
    }
};

#endif //LCD_OBSERVER
