
#include <ShiftRegister74HC595.h>

#include <arduinoFFT.h>
#define NUM_BANDS 2
// Constants for filter coefficients and buffer size
#define BUFFER_SIZE 10  // Buffer size, more buffersize delyed dying down
// Buffer to store previous input samples
float inputBuffer[BUFFER_SIZE] = {0};

int clearPin = 5;   // Arduino pin 5 connected to Pin 10, SRCLR(Clear/Reset) of 74HC595
int serialData = 6; // Arduino pin 6 connected to Pin 14, SER(serial input) of 74HC595
int shiftClock = 7; // Arduino pin 7 connected to Pin 11, SRCLK(shift clock) of 74HC595
int latchClock = 8; // Arduino pin 8 connected to Pin 12, RCLK(storage/latch clock) of 74HC595 ]
const int audioPin = A0;

const int numSamples = 128; //  256- works. should be power of 2, maybe need to check for overflow of var for higher num.
float filteredValue=0.0;
int SpreadBinary[2];
//Global Variables
int sensorPin = A0;        //pin number to use the ADC
int sensorValue = 0;      //initialization of sensor variable, equivalent to EMA Y
float EMA_a= 0.5;    //initialization of EMA alpha
int EMA_S= 0;        //initialization of EMA S
int highpass = 0;
double samples[numSamples];
double isamples[numSamples];
int bandValues[] = {0,0};
arduinoFFT FFT = arduinoFFT(samples, isamples, numSamples, 8900);

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
  
  EMA_S = analogRead(sensorPin);      //set EMA S for t=1
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
  float minValue = inputBuffer[0]; // Initialize with the first element of the buffer
  
  // Iterate through the buffer to find the minimum value
  for (int i = 1; i < BUFFER_SIZE; i++) {
    if (inputBuffer[i] < minValue) {
      minValue = inputBuffer[i];
    }
  }
  
  // Subtract the min from the current input sample
  float outputSample = inputSample - minValue;
  
  return outputSample;
}

void showRMS(int rms, int rms2)
{
  digitalWrite(latchClock, LOW);                    // take the latchClock low so the LEDs don't change while you're sending in bits:
  shiftOut(serialData, shiftClock, MSBFIRST, rms2); // shift out the bits
  shiftOut(serialData, shiftClock, MSBFIRST, rms);  // shift out the bits
  digitalWrite(latchClock, HIGH);                   // take the latch pin high so the LEDs will light up

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
void loop1(){
  //read samples 
  for (int i = 0; i < numSamples; i++) //120us time elapsed=> 4khz is max frequency can be picked
  {
    samples[i] = analogRead(audioPin);
  }
  for (int i = 0; i < numSamples; i++) //set img to zero
  {
    isamples[i] = 0;
  }
  for (int i = 0; i<NUM_BANDS; i++){
    bandValues[i] = 0;
  }
  FFT.DCRemoval();
  FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(FFT_FORWARD);
  FFT.ComplexToMagnitude();
  for (int i = 2; i < (numSamples/2); i++){
      
      if (samples[i]>30) bandValues[0]  += (int)samples[i];
  }
  Serial.println(bandValues[0]);

}
void loo9p(){
  float samples[20];
  unsigned long s[20];
  unsigned long startTime = micros();  // Record the start time
  for (int i = 0; i < 20; i++)
  {
    samples[i] = analogRead(audioPin);
    s[i]=micros();
  }

  for (int i = 0; i < 20; i++)
  {
    Serial.print("Sensor Value: ");
    Serial.print(samples[i]);
    Serial.print("\tElapsed Time between current and next(us): ");
    unsigned long elapsedTime=s[i+1]-s[i];
    Serial.println(elapsedTime);

  }

  exit(1);
}

void loop(){

  for (int i=0; i< 20; i++){
    sensorValue = analogRead(audioPin);              //read the sensor value using ADC
    EMA_S = (EMA_a*sensorValue) + ((1-EMA_a)*EMA_S);  //run the EMA
    highpass = sensorValue - EMA_S;                   //calculate the high-pass signal
    Serial.print("Sen val:");
    Serial.println(sensorValue);
    Serial.print("Highpass:");
    Serial.println(highpass);
  }
  // exit(1);
}

void iloop1()
{
  //read samples 
  float samples[numSamples];
  for (int i = 0; i < numSamples; i++) //120us time elapsed=> 4khz is max frequency can be picked
  {
    samples[i] = analogRead(audioPin);

  }
  // High-pass (EMA high pass is used)collected values and compute RMS
  filteredValue=0;
  for (int i = 0; i < numSamples; i++)
  { 
    sensorValue = samples[i];              //read the sensor value using ADC
    EMA_S = (EMA_a*sensorValue) + ((1-EMA_a)*EMA_S);  //run the EMA
    highpass = sensorValue - EMA_S;                   //calculate the high-pass signal
    filteredValue += highpass * highpass;

  }
  filteredValue /= numSamples;
  float rms = sqrt(filteredValue);
  Serial.print("rms:");
  Serial.println(rms);
  
  // //display only if there is some sound, 
  // // if(rms>calibratedValue){
  // //Display the RMS on LED using Shift registers
  // rms=removeDCOffset(rms);
  // // rms=rms+10.00;
  // // FFT.DCRemoval();
  rms=rms-2;
  rms=rms*13;
  // Serial.println(rms);

  spreadValue(rms, SpreadBinary);
  // // showRMS(0,0);
  showRMS(SpreadBinary[0], SpreadBinary[1]);
  // }

  
}
