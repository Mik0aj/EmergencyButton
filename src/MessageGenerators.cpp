#ifndef MESSAGE_GENERATORS
#define MESSAGE_GENERATORS
#include <Arduino.h>

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
#endif //MESSAGE_GENERATORS
