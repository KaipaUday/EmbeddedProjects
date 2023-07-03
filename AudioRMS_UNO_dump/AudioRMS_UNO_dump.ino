
#include <ShiftRegister74HC595.h>
// #define NUM_BANDS 2
#define numRegisters 10
// Constants for filter coefficients and buffer size
#define FILTER_ORDER 2  // Order of the filter (number of taps)
#define BUFFER_SIZE (FILTER_ORDER + 1)   // Buffer size = filter order + 1, Buffer size, more buffersize delyed dying down
// Buffer to store previous input samples
float inputBuffer[BUFFER_SIZE] = {0};
float filterCoefficients[FILTER_ORDER] = {0.999, -0.999
};
int clearPin = 5;   // Arduino pin 5 connected to Pin 10, SRCLR(Clear/Reset) of 74HC595
int serialData = 6; // Arduino pin 6 connected to Pin 14, SER(serial input) of 74HC595
int shiftClock = 7; // Arduino pin 7 connected to Pin 11, SRCLK(shift clock) of 74HC595
int latchClock = 8; // Arduino pin 8 connected to Pin 12, RCLK(storage/latch clock) of 74HC595 ]
const int audioPin = A0;

const int numSamples = 128; //  256- works. should be power of 2, maybe need to check for overflow of var for higher num.
double  filteredValue=0.0;
long min=999999;
long max=-1;
int sensorPin = A0;        //pin number to use the ADC
int sensorValue = 0;      //initialization of sensor variable, equivalent to EMA Y
double samples[numSamples];
double highPassedVal;
// double inputSample[numSamples];
//Global Variables
// float inputBuffer[BUFFER_SIZE] = {0};
// float EMA_a= 0.1;    //initialization of EMA alpha
// int EMA_S= 0;        //initialization of EMA S
// int highpass = 0;
// int SpreadBinary[2];
// const int rmsSample = 8;

// int bandValues[] = {0,0};
// arduinoFFT FFT = arduinoFFT(samples, isamples, numSamples, 8900);
ShiftRegister74HC595<numRegisters> shiftRegister(serialData, shiftClock, latchClock);

void setup()
{ // runs once at startup
  // set pins to output so you can control the shift register
  pinMode(clearPin, OUTPUT);
  pinMode(shiftClock, OUTPUT);
  pinMode(latchClock, OUTPUT);
  pinMode(serialData, OUTPUT);
  Serial.begin(115200);
  digitalWrite(clearPin, LOW);  // Pin is active-low, this clears the shift register
  digitalWrite(clearPin, HIGH); // Clear pin is inactive
  
  // EMA_S = analogRead(sensorPin);      //set EMA S for t=1
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
  Serial.println(rms);
  int val= round(log10(rms)/log10(2)); //log2(x)= log10(x)/log10(2)


  // Light up the LED bar based on the scaled value
  for (int i = 0; i < numRegisters; i++) {
    if (i < val) {
      shiftRegister.set(i, HIGH);  // Turn on the segment
    } else {
      shiftRegister.set(i, LOW);   // Turn off the segment
    }
  }

}

// void showRMS(int rms, int rms2)
// {
//   digitalWrite(latchClock, LOW);                    // take the latchClock low so the LEDs don't change while you're sending in bits:
//   shiftOut(serialData, shiftClock, MSBFIRST, rms2); // shift out the bits
//   shiftOut(serialData, shiftClock, MSBFIRST, rms);  // shift out the bits
//   digitalWrite(latchClock, HIGH);                   // take the latch pin high so the LEDs will light up

// }
// void spreadValue(float value, int SpreadBinary[])
// {

//   // Map the value from the input range to the spread range
//   // float mappedValue = ((float)(value - minRange) / (float)(maxRange - minRange)) * (float)(spreadMax - spreadMin);
//   // int spreadValue = int(mappedValue);
//   int spreadValue= int(value);

//   // to have lower continous incremental instead of discrete.
//   if (spreadValue > 511)
//   {
//     SpreadBinary[0] = 255;
//     SpreadBinary[1] = 3;
//     return;
//   }
//   else if (spreadValue > 255)
//   {
//     SpreadBinary[0] = 255;
//     SpreadBinary[1] = 1;
//     return;
//   }
//   else if (spreadValue > 127)
//   {
//     SpreadBinary[0] = 255;
//     SpreadBinary[1] = 0;
//     return;

//   }
//   else if (spreadValue > 63)
//   {
//     SpreadBinary[0] = 127;
//     SpreadBinary[1] = 0;
//   return;

//   }
//   else if (spreadValue > 31)
//   {
//     SpreadBinary[0] = 63;
//     SpreadBinary[1] = 0;
//   return;

//   }
//   else if (spreadValue > 15)
//   {
//     SpreadBinary[0] = 31;
//     SpreadBinary[1] = 0;
//   return;

//   }
//   else if (spreadValue > 7)
//   {
//     SpreadBinary[0] = 15;
//     SpreadBinary[1] = 0;
//   return;

//   }
//   else if (spreadValue > 3)
//   {
//     SpreadBinary[0] = 7;
//     SpreadBinary[1] = 0;
//   return;

//   }
//   else if (spreadValue > 1)
//   {
//     SpreadBinary[0] = 3;
//     SpreadBinary[1] = 0;
//   return;

//   }
//   else if (spreadValue > 0)
//   {
//     SpreadBinary[0] = 1;
//     SpreadBinary[1] = 0;
//   return;

//   }
//   else
//   {
//     SpreadBinary[0] = 0;
//     SpreadBinary[1] = 0;
//   return;

//   }

//   // Serial.println(SpreadBinary[0]+SpreadBinary[1]);
// }

