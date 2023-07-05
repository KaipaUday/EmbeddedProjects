
#include <ShiftRegister74HC595.h>

const int clearPin = 5;   // Arduino pin 5 connected to Pin 10, SRCLR(Clear/Reset) of 74HC595
const int serialData = 6; // Arduino pin 6 connected to Pin 14, SER(serial input) of 74HC595
const int shiftClock = 7; // Arduino pin 7 connected to Pin 11, SRCLK(shift clock) of 74HC595
const int latchClock = 8; // Arduino pin 8 connected to Pin 12, RCLK(storage/latch clock) of 74HC595 ]
const int audioPin = A0;

#define CntSiftRegisters 10 //number of register pins used= leds
#define HIGH_PASS_FIR_FILTER_ORDER 2  // Order of the FIR filter.
#define BUFFER_SIZE (HIGH_PASS_FIR_FILTER_ORDER + 1)   //  more buffersize delyed dying down 

float inputBuffer[BUFFER_SIZE] = {0}; //Buffer to store previous input samples used in filtering
float filterCoefficients[HIGH_PASS_FIR_FILTER_ORDER] = {0.999, -0.999 }; // attained from digital filter designer tool- matlab 

const int numSamples = 128; //  256- works. should be power of 2, maybe need to check for overflow of var for higher num.
int samples[numSamples]; // variable to batch of analog read values from sensor
double highPassedVal;  


ShiftRegister74HC595<CntSiftRegisters> shiftRegister(serialData, shiftClock, latchClock);
float updateIntervel; //milli sec, for leds 
int prevDisplayLevel=0;
double maxRms=0.0;
unsigned long newTime;

void setup()
{
  Serial.begin(115200);
  
  //define pin types
  pinMode(clearPin, OUTPUT);
  pinMode(shiftClock, OUTPUT);
  pinMode(latchClock, OUTPUT);
  pinMode(serialData, OUTPUT);

  //clear/shift previous data if any
  digitalWrite(clearPin, LOW);  // Pin is active-low, this clears the shift register
  digitalWrite(clearPin, HIGH); // Clear pin is inactive
  Serial.println("Which sensor would you like to read? ex:15,30 ");
  while (Serial.available() == 0)   updateIntervel = 1000000.0/(float) Serial.parseInt();
  newTime=micros();
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

void updateLEDS(int displayLevel){
  /*
  simple function to control shift register pins individually , 
  */
  if(displayLevel > prevDisplayLevel && prevDisplayLevel < 10) displayLevel+=1;
  else if(displayLevel < prevDisplayLevel && prevDisplayLevel > 0) displayLevel-=1;

  for (int i = 0; i < CntSiftRegisters; i++) {
    if (i < displayLevel) {
      shiftRegister.set(i, HIGH);  // Turn on the segment
    } else {
      shiftRegister.set(i, LOW);   // Turn off the segment
    }
  }

  prevDisplayLevel= displayLevel; //update prev val

}

void loop(){
  
  for (int i = 0; i < numSamples; i++)  samples[i] = analogRead(audioPin); //record samples and then process to capture high freq, 120us time elapsed=> 4khz is max frequency can be picked, 

  double rms=0.0;
  for (int i = 0; i < numSamples; i++)
  { 
    highPassedVal = processFIRFilter(samples[i]);  // pass the value through filter
    highPassedVal*=8; //amplify filtered value
    rms += highPassedVal * highPassedVal;
  }
  rms /= numSamples;
  rms = sqrt(rms);
  Serial.println(rms);
  if(rms>maxRms)  maxRms=rms;

  if ((micros() - newTime) > updateIntervel) {
    return; // do not update if time elapsed is smaller
  }
  else  {
    int displayLevel= round(log10(maxRms)/log10(2)); //log2(x)= log10(x)/log10(2)
    updateLEDS(displayLevel);
    newTime=micros();
    maxRms = 0.0;
  }

}

  // Light up the LED bar based on the scaled value-
  //to do : display average instead of the samples until 33.3 ms. we can also have 20 or 15 fps
  // smooth curve between the samples, if curr>pre : preval+1, else <:prev-1 - so it decreses step by step
