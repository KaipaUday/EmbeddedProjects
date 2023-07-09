
#include <ShiftRegister74HC595.h>

const int audioPin = A0; //A0 sensor analog ping

const int clearPin = 5;   // Arduino pin 5 connected to Pin 10, SRCLR(Clear/Reset) of 74HC595
const int serialData = 6; // Arduino pin 6 connected to Pin 14, SER(serial input) of 74HC595
const int shiftClock = 7; // Arduino pin 7 connected to Pin 11, SRCLK(shift clock) of 74HC595
const int latchClock = 8; // Arduino pin 8 connected to Pin 12, RCLK(storage/latch clock) of 74HC595 ]

#define CntSiftRegisters 10 //equal to leds 
#define FILTER_ORDER 2  // Order of the filter 
#define BUFFER_SIZE (FILTER_ORDER + 1)   // Buffer size = filter order + 1, Buffer size, more buffersize delyed dying down

float inputBuffer[BUFFER_SIZE] = {0};
float filterCoefficients[FILTER_ORDER] = {0.999, -0.999
};

const int numSamples = 128;
double samples[numSamples]; // signal samples
double highPassedVal;
ShiftRegister74HC595<CntSiftRegisters> shiftRegister(serialData, shiftClock, latchClock);

void setup()
{ 
  //define pin types
  pinMode(clearPin, OUTPUT);
  pinMode(shiftClock, OUTPUT);
  pinMode(latchClock, OUTPUT);
  pinMode(serialData, OUTPUT);

  Serial.begin(115200);

  //clear/shift previous data if any
  digitalWrite(clearPin, LOW);  // Pin is active-low, this clears the shift register
  digitalWrite(clearPin, HIGH); // Clear pin is inactive
}


// Function to process the audio signal using the FIR filter
double processFIRFilter(double inputSample) {
  // Shift samples in the buffer
  for (int i = BUFFER_SIZE - 1; i >= 1; i--) {
    inputBuffer[i] = inputBuffer[i - 1];
  }
  
  // Add the current input sample to the buffer
  inputBuffer[0] = inputSample;
  
  // Apply the filter
  double outputSample = 0;
  for (int i = 0; i < FILTER_ORDER; i++) {
    outputSample += filterCoefficients[i] * inputBuffer[i];
  }
  
  return outputSample;
}


void loop(){
  for (int i = 0; i < numSamples; i++) //120us time elapsed=> 4khz is max frequency can be picked
  {
    samples[i] = analogRead(audioPin);
  }
  double filteredValue=0;
  for (int i = 0; i < numSamples; i++)
  { 
    highPassedVal = processFIRFilter(samples[i]);  // Replace A0 with your desired analog input pin
    highPassedVal*=8; //amplify filtered value
    filteredValue += highPassedVal * highPassedVal;
  }
  filteredValue /= numSamples;
  double rms = sqrt(filteredValue);
  // Serial.println(rms);
  int val= round(log10(rms)/log10(2)); //log2(x)= log10(x)/log10(2)

  // Light up the LED bar based on the scaled value- 
  for (int i = 0; i < CntSiftRegisters; i++) {
    if (i < val) {
      shiftRegister.set(i, HIGH);  // Turn on the segment
    } else {
      shiftRegister.set(i, LOW);   // Turn off the segment
    }
  }

}