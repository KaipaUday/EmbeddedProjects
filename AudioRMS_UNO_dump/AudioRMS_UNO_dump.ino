
int clearPin = 5; //Arduino pin 5 connected to Pin 10, SRCLR(Clear/Reset) of 74HC595
int serialData = 6;  //Arduino pin 6 connected to Pin 14, SER(serial input) of 74HC595
int shiftClock = 7;  //Arduino pin 7 connected to Pin 11, SRCLK(shift clock) of 74HC595
int latchClock = 8;  //Arduino pin 8 connected to Pin 12, RCLK(storage/latch clock) of 74HC595 ]
const int audioPin = A0;

const int numSamples = 5; // change to get smooth value
float min= 99999; // used to store the min sound value recorded
float max=0;  // used for max sound 

void setup() {   //runs once at startup
   //set pins to output so you can control the shift register
  pinMode(clearPin, OUTPUT);
  pinMode(shiftClock, OUTPUT);
  pinMode(latchClock, OUTPUT);
  pinMode(serialData, OUTPUT);
  Serial.begin(9600);
  digitalWrite(clearPin, LOW); //Pin is active-low, this clears the shift register
  digitalWrite(clearPin, HIGH); //Clear pin is inactive
}

void showRMS(int rms){
  digitalWrite(latchClock, LOW);     // take the latchClock low so the LEDs don't change while you're sending in bits:
  shiftOut(serialData, shiftClock, MSBFIRST, rms);      // shift out the bits
  digitalWrite(latchClock, HIGH);     //take the latch pin high so the LEDs will light up
}
int spreadValue(float value, float minRange, float maxRange, int spreadMin, int spreadMax) {
    // Map the value from the input range to the spread range
    float mappedValue =  ((float)(value - minRange) / (float)(maxRange - minRange))* (float)(spreadMax - spreadMin);
    int spreadValue = int(mappedValue);
    Serial.println(mappedValue);

    // to have lower continous incremental instead of discrete.
    if (spreadValue > 511)
        return 1023;
    else if (spreadValue > 255)
        return 511;
    else if (spreadValue > 127)
        return 255;
    else if (spreadValue > 63)
        return 127;
    else if (spreadValue > 31)
        return 63;
    else if (spreadValue > 15)
        return 31;
    else if (spreadValue > 7)
        return 15;
    else if (spreadValue > 3)
        return 7;
    else if (spreadValue > 1)
        return 3;
    else if (spreadValue > 0)
        return 1;
    else
        return 0;
}

void loop() {
// level = analogRead(Sensor);

  long int sum = 0;     // Variable to accumulate the sum of squared samples

  // Collect samples
  for (int i = 0; i < numSamples; i++) {
    long int sample = analogRead(audioPin);
    if(sample>max){
      max=sample;
    }else if(sample<min){
      min=sample;
    }
    sum += sample * sample;  // Square and accumulate the sample
    delay(1);  // Small delay between samples
  }
  // Calculate RMS value
  float average = sum / numSamples;
  float rms = sqrt(average);

  delay(100);  // Delay between measurements
  int SpreadBinary= spreadValue(rms,min,max,0,1023);
  showRMS(SpreadBinary);

}

// #include <SPI.h>

// // Constants
// const int soundSensorPin = A0;      // Sound sensor analog pin
// const int dataPin = 2;              // Shift register data pin (SER)
// const int latchPin = 3;             // Shift register latch pin (RCLK)
// const int clockPin = 4;             // Shift register clock pin (SRCLK)
// const int numRegisters = 2;         // Number of shift registers
// const int numLeds = 16;             // Number of LEDs in the bar

// // Variables
// int audioBuffer[128];               // Audio buffer for storing samples
// int bufferIndex = 0;                // Index for the current audio buffer position

// // Shift Register Variables
// byte registers[numRegisters];       // Shift register values

// void setup() {
//   // Initialize serial communication for debugging
//   Serial.begin(9600);

//   // Initialize sound sensor
//   pinMode(soundSensorPin, INPUT);

//   // Initialize shift register pins
//   pinMode(dataPin, OUTPUT);
//   pinMode(latchPin, OUTPUT);
//   pinMode(clockPin, OUTPUT);

//   // Clear shift registers and LEDs
//   clearLEDBar();

//   // Initialize shift register values
//   for (int i = 0; i < numRegisters; i++) {
//     registers[i] = 0;
//   }
// }
// void updateShiftRegisters() {
//   // Iterate through each shift register
//   for (int i = numRegisters - 1; i >= 0; i--) {
//     // Activate the latch pin to prevent flickering
//     digitalWrite(latchPin, LOW);

//     // Shift out the data for the current register
//     shiftOut(dataPin, clockPin, MSBFIRST, registers[i]);

//     // Deactivate the latch pin to update the outputs
//     digitalWrite(latchPin, HIGH);
//   }
// }


// void loop() {
//   // Read sound sensor value
//   int soundValue = analogRead(soundSensorPin);

//   // Apply high-pass filter to eliminate rumbling detection
//   soundValue = highPassFilter(soundValue);
//   Serial.println(soundValue);
//   // Store sound value in the audio buffer
//   audioBuffer[bufferIndex] = soundValue;
//   Serial.println(int(audioBuffer));
//   // Calculate RMS of the audio buffer
//   float rms = calculateRMS();
//   Serial.println(rms);

//   // Update LED bar based on RMS level
//   updateLEDBar(int(rms));

//   // Increment buffer index
//   bufferIndex = (bufferIndex + 1) % 128;

//   // Delay for stability
//   delay(1000);
// }
// void resetLED(int index) {
//   // Reset the corresponding LED in the bar
//   int shiftRegisterIndex = index / 8;
//   int bitIndex = index % 8;

//   bitClear(registers[shiftRegisterIndex], bitIndex);
// }


// int highPassFilter(int value) {
//   // Apply high-pass filter to the input value
//   // You can implement your own filter here if needed
//   return value;
// }

// float calculateRMS() {
//   // Calculate the root mean square (RMS) of the audio buffer
//   long sum = 0;

//   // Sum the squares of all samples in the buffer
//   for (int i = 0; i < 128; i++) {
//     sum += audioBuffer[i] * audioBuffer[i];
//   }

//   // Calculate the RMS value
//   float rms = sqrt(sum / 128.0);

//   return rms;
// }

// void updateLEDBar(float rms) {
//   // Convert RMS value to the number of LEDs to be turned on
//   int numLEDs = map(rms, 0, 1023, 0, numLeds);

//   // Clear shift registers and LEDs
//   clearLEDBar();

//   // Set the corresponding number of LEDs in the bar
//   for (int i = 0; i < numLEDs; i++) {
//     setLED(i);
//   }

//   // Update the shift registers to display the LEDs
//   updateShiftRegisters();
// }

// void clearLEDBar() {
//   // Clear shift registers and turn off all LEDs
//   for (int i = 0; i < numRegisters * 8; i++) {
//     resetLED(i);
//   }

//   // Update the shift registers to clear the LEDs
//   updateShiftRegisters();
// }

// void setLED(int index) {
//   // Set the corresponding LED in the bar
//   int shiftRegisterIndex = index / 8;
//   int bitIndex = index % 8;

//   bitSet(registers[shiftRegisterIndex], bitIndex);
// }
