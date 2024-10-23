#include <EEPROM.h>

// Define the device pins
const int devicePin1 = 4; // Device connected to D4
const int devicePin2 = 5; // Device connected to D5
const int devicePin3 = 6; // Device connected to D6

// Define logic levels for Normally Open (NO) and Normally Closed (NC)
const int NO_STATE = LOW;  // NO devices are activated by LOW signal
const int NC_STATE = HIGH; // NC devices are activated by HIGH signal

// Number of devices
const int numDevices = 3;

// Arrays to store device pins and states
int devicePins[numDevices] = {devicePin1, devicePin2, devicePin3};

// Device states: true if device is ON, false if OFF
bool deviceStates[numDevices] = {false, false, false};

// Remaining times for devices (in milliseconds)
unsigned long remainingTimes[numDevices] = {0, 0, 0};

// EEPROM addresses for stored data
const int EEPROM_STATE_ADDR = 0; // Boolean device states (1 byte per device)
const int EEPROM_TIME_ADDR = EEPROM_STATE_ADDR + numDevices; // Remaining times (4 bytes per device)

unsigned long previousMillis = 0; // To track elapsed time

void setup() {
  // Initialize Serial communication
  Serial.begin(9600);

  // Initialize the device pins as outputs and set them to OFF
  for (int i = 0; i < numDevices; i++) {
    pinMode(devicePins[i], OUTPUT);
    digitalWrite(devicePins[i], NO_STATE); // Set initial state to OFF
  }

  // Read device states and remaining times from EEPROM
  readDeviceStatesFromEEPROM();
  readRemainingTimesFromEEPROM();

  // Restore device states
  for (int i = 0; i < numDevices; i++) {
    if (deviceStates[i]) {
      // Turn on the device
      digitalWrite(devicePins[i], NC_STATE);
    }
  }

  previousMillis = millis(); // Initialize previousMillis
}

void loop() {
  unsigned long currentMillis = millis();
  unsigned long elapsedTime = currentMillis - previousMillis;
  previousMillis = currentMillis;

  // Update remaining times and check if any devices need to be turned off
  for (int i = 0; i < numDevices; i++) {
    if (deviceStates[i]) { // Device is currently ON
      if (elapsedTime > 0) {
        if (remainingTimes[i] >= elapsedTime) {
          remainingTimes[i] -= elapsedTime;
        } else {
          remainingTimes[i] = 0;
        }
      }

      if (remainingTimes[i] <= 0) {
        // Time to turn off the device
        digitalWrite(devicePins[i], NO_STATE);
        deviceStates[i] = false;

        // Update EEPROM
        updateDeviceStateInEEPROM(i);
        updateRemainingTimeInEEPROM(i);

        // Send acknowledgment
        Serial.println("Device " + String(i + 1) + " has been turned off.");
      } else {
        // Update EEPROM with new remaining time
        updateRemainingTimeInEEPROM(i);
      }
    }
  }

  // Check for new serial input
  if (Serial.available() > 0) {
    // Read the incoming string until newline character
    String inputString = Serial.readStringUntil('\n');
    inputString.trim(); // Remove any leading/trailing whitespace

    // Parse the input string to extract device and duration
    int commaIndex = inputString.indexOf(',');
    if (commaIndex > 0) {
      String deviceStr = inputString.substring(0, commaIndex);
      String durationStr = inputString.substring(commaIndex + 1);

      int deviceNumber = deviceStr.toInt();
      int durationSeconds = durationStr.toInt();

      // Validate device number and duration
      int deviceIndex = deviceNumber - 1; // Adjust for zero-based index
      if (deviceIndex >= 0 && deviceIndex < numDevices && durationSeconds > 0) {
        // Turn on the device
        digitalWrite(devicePins[deviceIndex], NC_STATE);
        deviceStates[deviceIndex] = true;
        remainingTimes[deviceIndex] = (unsigned long)durationSeconds * 1000UL;

        // Update EEPROM
        updateDeviceStateInEEPROM(deviceIndex);
        updateRemainingTimeInEEPROM(deviceIndex);

        // Send acknowledgment
        Serial.println("Device " + String(deviceNumber) + " activated for " + String(durationSeconds) + " seconds.");
      } else {
        // Invalid device number or duration
        Serial.println("Invalid command. Please check the device number and duration.");
      }
    } else {
      // Invalid input format
      Serial.println("Invalid command format. Use: DeviceNumber,DurationInSeconds");
    }
  }
}

// Function to read device states from EEPROM
void readDeviceStatesFromEEPROM() {
  for (int i = 0; i < numDevices; i++) {
    deviceStates[i] = EEPROM.read(EEPROM_STATE_ADDR + i);
  }
}

// Function to read remaining times from EEPROM
void readRemainingTimesFromEEPROM() {
  for (int i = 0; i < numDevices; i++) {
    unsigned long time = 0;
    for (int j = 0; j < 4; j++) {
      byte data = EEPROM.read(EEPROM_TIME_ADDR + i * 4 + j);
      time |= ((unsigned long)data << (8 * j));
    }
    remainingTimes[i] = time;
  }
}

// Function to update a device state in EEPROM
void updateDeviceStateInEEPROM(int deviceIndex) {
  EEPROM.update(EEPROM_STATE_ADDR + deviceIndex, deviceStates[deviceIndex]);
}

// Function to update a remaining time in EEPROM
void updateRemainingTimeInEEPROM(int deviceIndex) {
  unsigned long time = remainingTimes[deviceIndex];
  for (int j = 0; j < 4; j++) {
    byte data = (time >> (8 * j)) & 0xFF;
    EEPROM.update(EEPROM_TIME_ADDR + deviceIndex * 4 + j, data);
  }
}
