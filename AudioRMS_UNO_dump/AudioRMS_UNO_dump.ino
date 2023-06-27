
#include <arduinoFFT.h>

// Constants for filter coefficients and buffer size
#define BUFFER_SIZE 10  // Buffer size
// Buffer to store previous input samples
float inputBuffer[BUFFER_SIZE] = {0};

int clearPin = 5;   // Arduino pin 5 connected to Pin 10, SRCLR(Clear/Reset) of 74HC595
int serialData = 6; // Arduino pin 6 connected to Pin 14, SER(serial input) of 74HC595
int shiftClock = 7; // Arduino pin 7 connected to Pin 11, SRCLK(shift clock) of 74HC595
int latchClock = 8; // Arduino pin 8 connected to Pin 12, RCLK(storage/latch clock) of 74HC595 ]
const int audioPin = A0;

const int numSamples = 255; // change to get smooth value, Overflow occurs for higher values >400, recommended 255.
float filteredValue=0.0;
float calibratedValue;
int SpreadBinary[2];


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
  // Calculate initial filtered value
  float samples[numSamples];
  for (int i = 0; i < numSamples; i++)
  {
    samples[i] = analogRead(audioPin);
  }
  for (int i = 0; i < numSamples; i++) {
    filteredValue += samples[i];
  }
  filteredValue /= numSamples;
  Serial.print("Calibrated: ");
  Serial.println(filteredValue);
  calibratedValue= filteredValue;
}

// Function to remove DC offset from the audio signal
float removeDCOffset(float inputSample) {
  // Shift samples in the buffer
  for (int i = BUFFER_SIZE - 1; i >= 1; i--) {
    inputBuffer[i] = inputBuffer[i - 1];
  }
  
  // Add the current input sample to the buffer
  inputBuffer[0] = inputSample;
  
  // Calculate the average of the buffer
  float sum = 0;
  for (int i = 0; i < BUFFER_SIZE; i++) {
    sum += inputBuffer[i];
  }
  float average = sum / BUFFER_SIZE;
  
  // Subtract the average from the current input sample
  float outputSample = inputSample - average;
  
  return outputSample;
}

void showRMS(int rms, int rms2)
{
  digitalWrite(latchClock, LOW);                    // take the latchClock low so the LEDs don't change while you're sending in bits:
  shiftOut(serialData, shiftClock, MSBFIRST, rms2); // shift out the bits
  shiftOut(serialData, shiftClock, MSBFIRST, rms);  // shift out the bits
  digitalWrite(latchClock, HIGH);                   // take the latch pin high so the LEDs will light up
  // delay(100);
  // digitalWrite(latchClock, LOW);                    // take the latchClock low so the LEDs don't change while you're sending in bits:
  // shiftOut(serialData, shiftClock, MSBFIRST, 0); // shift out the bits
  // shiftOut(serialData, shiftClock, MSBFIRST, 0);  // shift out the bits
  // digitalWrite(latchClock, HIGH);                   // take the latch pin high so the LEDs will light up
}
void spreadValue(float value, int SpreadBinary[])
{

  // Map the value from the input range to the spread range
  // float mappedValue = ((float)(value - minRange) / (float)(maxRange - minRange)) * (float)(spreadMax - spreadMin);
  // int spreadValue = int(mappedValue);
  int spreadValue= int(value);

  // to have lower continous incremental instead of discrete.
  if (spreadValue > 511)
  {
    SpreadBinary[0] = 255;
    SpreadBinary[1] = 3;
    return;
  }
  else if (spreadValue > 255)
  {
    SpreadBinary[0] = 255;
    SpreadBinary[1] = 1;
    return;
  }
  else if (spreadValue > 127)
  {
    SpreadBinary[0] = 255;
    SpreadBinary[1] = 0;
    return;

  }
  else if (spreadValue > 63)
  {
    SpreadBinary[0] = 127;
    SpreadBinary[1] = 0;
  return;

  }
  else if (spreadValue > 31)
  {
    SpreadBinary[0] = 63;
    SpreadBinary[1] = 0;
  return;

  }
  else if (spreadValue > 15)
  {
    SpreadBinary[0] = 31;
    SpreadBinary[1] = 0;
  return;

  }
  else if (spreadValue > 7)
  {
    SpreadBinary[0] = 15;
    SpreadBinary[1] = 0;
  return;

  }
  else if (spreadValue > 3)
  {
    SpreadBinary[0] = 7;
    SpreadBinary[1] = 0;
  return;

  }
  else if (spreadValue > 1)
  {
    SpreadBinary[0] = 3;
    SpreadBinary[1] = 0;
  return;

  }
  else if (spreadValue > 0)
  {
    SpreadBinary[0] = 1;
    SpreadBinary[1] = 0;
  return;

  }
  else
  {
    SpreadBinary[0] = 0;
    SpreadBinary[1] = 0;
  return;

  }

  // Serial.println(SpreadBinary[0]+SpreadBinary[1]);
}


void loop()
{
  //read samples 
  float samples[numSamples];
  for (int i = 0; i < numSamples; i++)
  {
    samples[i] = analogRead(audioPin);
  }

  //compute RMS
  filteredValue=0;
  for (int i = 0; i < numSamples; i++)
  { 
    filteredValue += samples[i] * samples[i];
  }
  filteredValue /= numSamples;
  float rms = sqrt(filteredValue);
  
  //display only if there is some sound, 
  // if(rms>calibratedValue){
  //Display the RMS on LED using Shift registers
  // rms=removeDCOffset(rms);
  // rms=rms+10.00;
  // FFT.DCRemoval();
  Serial.println(rms);

  spreadValue(rms, SpreadBinary);
  showRMS(0,0);
  // showRMS(SpreadBinary[0], SpreadBinary[1]);
  // }

  
}
