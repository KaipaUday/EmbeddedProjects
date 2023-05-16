#include <SPI.h>

// Constants
const int soundSensorPin = A0;      // Sound sensor analog pin
const int dataPin = 2;              // Shift register data pin (SER)
const int latchPin = 3;             // Shift register latch pin (RCLK)
const int clockPin = 4;             // Shift register clock pin (SRCLK)
const int numRegisters = 2;         // Number of shift registers
const int numLeds = 16;             // Number of LEDs in the bar

// Variables
int audioBuffer[128];               // Audio buffer for storing samples
int bufferIndex = 0;                // Index for the current audio buffer position

// Shift Register Variables
byte registers[numRegisters];       // Shift register values

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);

  // Initialize sound sensor
  pinMode(soundSensorPin, INPUT);

  // Initialize shift register pins
  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);

  // Clear shift registers and LEDs
  clearLEDBar();

  // Initialize shift register values
  for (int i = 0; i < numRegisters; i++) {
    registers[i] = 0;
  }
}
void updateShiftRegisters() {
  // Iterate through each shift register
  for (int i = numRegisters - 1; i >= 0; i--) {
    // Activate the latch pin to prevent flickering
    digitalWrite(latchPin, LOW);

    // Shift out the data for the current register
    shiftOut(dataPin, clockPin, MSBFIRST, registers[i]);

    // Deactivate the latch pin to update the outputs
    digitalWrite(latchPin, HIGH);
  }
}


void loop() {
  // Read sound sensor value
  int soundValue = analogRead(soundSensorPin);

  // Apply high-pass filter to eliminate rumbling detection
  soundValue = highPassFilter(soundValue);

  // Store sound value in the audio buffer
  audioBuffer[bufferIndex] = soundValue;

  // Calculate RMS of the audio buffer
  float rms = calculateRMS();

  // Update LED bar based on RMS level
  updateLEDBar(rms);

  // Increment buffer index
  bufferIndex = (bufferIndex + 1) % 128;

  // Delay for stability
  delay(10);
}
void resetLED(int index) {
  // Reset the corresponding LED in the bar
  int shiftRegisterIndex = index / 8;
  int bitIndex = index % 8;

  bitClear(registers[shiftRegisterIndex], bitIndex);
}


int highPassFilter(int value) {
  // Apply high-pass filter to the input value
  // You can implement your own filter here if needed
  return value;
}

float calculateRMS() {
  // Calculate the root mean square (RMS) of the audio buffer
  long sum = 0;

  // Sum the squares of all samples in the buffer
  for (int i = 0; i < 128; i++) {
    sum += audioBuffer[i] * audioBuffer[i];
  }

  // Calculate the RMS value
  float rms = sqrt(sum / 128.0);

  return rms;
}

void updateLEDBar(float rms) {
  // Convert RMS value to the number of LEDs to be turned on
  int numLEDs = map(rms, 0, 1023, 0, numLeds);

  // Clear shift registers and LEDs
  clearLEDBar();

  // Set the corresponding number of LEDs in the bar
  for (int i = 0; i < numLEDs; i++) {
    setLED(i);
  }

  // Update the shift registers to display the LEDs
  updateShiftRegisters();
}

void clearLEDBar() {
  // Clear shift registers and turn off all LEDs
  for (int i = 0; i < numRegisters * 8; i++) {
    resetLED(i);
  }

  // Update the shift registers to clear the LEDs
  updateShiftRegisters();
}

void setLED(int index) {
  // Set the corresponding LED in the bar
  int shiftRegisterIndex = index / 8;
  int bitIndex = index % 8;

  bitSet(registers[shiftRegisterIndex], bitIndex);
}
