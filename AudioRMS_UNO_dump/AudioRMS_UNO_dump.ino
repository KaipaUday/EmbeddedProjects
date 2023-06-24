
int clearPin = 5;   // Arduino pin 5 connected to Pin 10, SRCLR(Clear/Reset) of 74HC595
int serialData = 6; // Arduino pin 6 connected to Pin 14, SER(serial input) of 74HC595
int shiftClock = 7; // Arduino pin 7 connected to Pin 11, SRCLK(shift clock) of 74HC595
int latchClock = 8; // Arduino pin 8 connected to Pin 12, RCLK(storage/latch clock) of 74HC595 ]
const int audioPin = A0;

const int numSamples = 50; // change to get smooth value
float min = 99999;         // used to store the min sound value recorded
float max = 0;             // used for max sound
float autocal=1000;
int flag=0;
int SpreadBinary[2];
float filteredValue = 0.0;
int rawValue = 0;
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
  int samples[numSamples];
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
  Serial.println(log(value));
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
  // long int sum = 0; // Variable to accumulate the sum of squared samples
  //int level = analogRead(audioPin);

  int samples[numSamples];
  for (int i = 0; i < numSamples; i++)
  {
    samples[i] = analogRead(audioPin);
  }

  filteredValue=0;
  for (int i = 0; i < numSamples; i++)
  {
    filteredValue += samples[i]*samples[i];
  }
  filteredValue /= numSamples;


  
  // // Collect samples
  // for (int i = 0; i < numSamples; i++)
  // {
  //   long int sample = analogRead(audioPin);
  //   sum += sample * sample; // Square and accumulate the sample
  //   // delay(1);  // Small delay between samples
  // }
  // // Calculate RMS value
  // float average = sum / numSamples;
  // float rms = sqrt(average);

  // spreadValue(rms-filteredValue, SpreadBinary);

  // showRMS(SpreadBinary[0], SpreadBinary[1]);
  // delay(10); // so lights are shown for some time.
}

  // Serial.print("val:");
  // Serial.print(SpreadBinary[0]);
  // Serial.print(":");
  // Serial.println(SpreadBinary[1]);
  /* digital read*/
  // level = analogRead(Sensor);
  // boolean val =digitalRead(audioPin);
  // Serial.println (val);

  /*analog read value*/
  // long int sample = analogRead(audioPin);
  // Serial.print("val:");
  // Serial.println(sample);
  // delay(500);

  /* to check connection*/
  // showRMS(255, 3);
