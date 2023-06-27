Problem:
Taking the audio output at the right time.
and int overflow issue while RMS
number of samples
- int int muliplication nd assign to float causes some error.
- Many math operations are not automatically type converted, needed to be careful with data types and size of data type.

Compenents

KYX-B10BGYR LED color array 10 segment
20 pins

Always series with resistors
https://th.bing.com/th/id/R.16f6b2396e20a6ba81c690457f7975c2?rik=FhFPofoCoqN1tw&riu=http%3a%2f%2fwebshop.domoticx.nl%2fimage%2fcatalog%2fmodule%2fLED+Bar+module+10+segmenten+BGYR%2fLED+Bar+module+10+segmenten+BGYR+maten.jpg&ehk=iWKSZz5LrSle8icCM4POQ52SIBH%2f4cuO61SfdiuRoSI%3d&risl=&pid=ImgRaw&r=0
https://youtu.be/jIKyPMU5kwk


U74HC595AG- 8 bit serial-in shift register(16 pin)
https://youtu.be/NykONuPWycc
bottom down to anti clock-wise pin numbering 
https://www.alldatasheet.com/datasheet-pdf/pdf/742505/UTC/U74HC595AG-D16-T.html
http://www.unisonic.com.tw/datasheet/U74HC595A.pdf#
int clearPin = 5;   // Arduino pin 5 connected to Pin 10, SRCLR(Clear/Reset) of 74HC595
int serialData = 6; // Arduino pin 6 connected to Pin 14, SER(serial input) of 74HC595
int shiftClock = 7; // Arduino pin 7 connected to Pin 11, SRCLK(shift clock) of 74HC595
int latchClock = 8; // Arduino pin 8 connected to Pin 12, RCLK(storage/latch clock) of 74HC595 ]
Pin 13 to GND

Sound sensor(4 pin)
Pin diagram and sensor @ https://circuitdigest.com/microcontroller-projects/interfacing-sound-sensor-with-arduino
When sensor potentiometer adjuster is lessthan 512, LED2 will blink, A0 gives range between 0-1023, sensitivity can be adjusted to give 1023 at normal level.
https://datasheetspdf.com/pdf-file/1402048/Joy-IT/KY-038/1


=========================================

High Pass Filter:
To implement a high pass filter for a sound sensor and calculate the RMS value of the sound using Arduino, you can follow these steps:

1. Set up your Arduino and connect the sound sensor to an analog input pin.
2. Define variables for storing the sound sensor readings and the filtered output.
3. Initialize the filter by taking multiple readings from the sound sensor and calculating their average as the initial value for the filter output.
4. In your main loop, read the raw sound sensor value using `analogRead()`.
5. Apply the high pass filter equation to calculate the filtered output:
   `filteredValue = alpha * (filteredValue + rawValue - previousRawValue);`
   Adjust the value of `alpha` to control the cutoff frequency of the filter.
6. Square the filtered value to obtain the squared sound signal.
7. Accumulate the squared values over a certain period to calculate the sum.
8. Divide the sum by the number of samples to obtain the average squared value.
9. Take the square root of the average squared value to get the RMS value of the sound.
10. Use this RMS value for further processing or display.

Here's a sample code snippet to help you get started:

```cpp
const int soundSensorPin = A0;
const int numSamples = 100;
const float alpha = 0.1;

int rawValue = 0;
int previousRawValue = 0;
float filteredValue = 0.0;
float sumSquared = 0.0;
float rmsValue = 0.0;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Initialize the sound sensor pin
  pinMode(soundSensorPin, INPUT);

  // Calculate initial filtered value
  for (int i = 0; i < numSamples; i++) {
    rawValue = analogRead(soundSensorPin);
    filteredValue += rawValue;
    delay(1);
  }
  filteredValue /= numSamples;
}

void loop() {
  // Read the raw sound sensor value
  rawValue = analogRead(soundSensorPin);

  // Apply high pass filter
  filteredValue = alpha * (filteredValue + rawValue - previousRawValue);

  // Calculate squared value
  float squaredValue = filteredValue * filteredValue;

  // Accumulate squared values
  sumSquared += squaredValue;

  // Update previous raw value
  previousRawValue = rawValue;

  // Calculate RMS value every numSamples iterations
  static int sampleCount = 0;
  sampleCount++;
  if (sampleCount >= numSamples) {
    // Calculate average squared value
    float averageSquared = sumSquared / numSamples;

    // Calculate RMS value
    rmsValue = sqrt(averageSquared);

    // Reset sample count and sum
    sampleCount = 0;
    sumSquared = 0.0;

    // Display RMS value
    Serial.print("RMS value: ");
    Serial.println(rmsValue);
  }
}
```

Remember to adjust the `alpha` value based on your desired cutoff frequency for the high pass filter. Also, make sure to connect the sound sensor to the correct pin on your Arduino board.

This code implements a basic high pass filter using a single-pole recursive filter equation. You can further refine the filter parameters and adjust the code according to your specific requirements.


part 1: read sensor input.

done : Analog pin poutputs a value of 590 and above. Max around 680.

Part 2: interface two 8-bit shift registers in series check output on LED.
Done: 1023 is passed to serial input. sent all the 10 leds on strip glowing, 10 1's =1023

2. 10 SEGMENT LED
To implement the blinking of a 10-segment LED based on the RMS value calculated using the previous code, you can follow these steps:

1. Connect the 10-segment LED to the appropriate digital output pins of your Arduino.
2. Define the pin numbers for each LED segment.
3. Inside the `loop()` function, after calculating the RMS value, determine the appropriate threshold values for each LED segment. These thresholds will determine when each segment should be turned on or off.
4. Use conditional statements to check the RMS value against the threshold values and control the LED segments accordingly.
5. If the RMS value is greater than or equal to a particular threshold, turn on the corresponding LED segment; otherwise, turn it off.
6. Repeat this process for each LED segment.

Here's an example of how you can modify the code to include the blinking of the 10-segment LED based on the RMS value:

```cpp
// Define the pin numbers for each LED segment
const int segmentPins[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

// Define the threshold values for each LED segment
const float segmentThresholds[] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};

// Rest of the code remains the same as the previous example

void loop() {
  // Read the raw sound sensor value
  rawValue = analogRead(soundSensorPin);

  // Apply high pass filter
  filteredValue = alpha * (filteredValue + rawValue - previousRawValue);

  // Calculate squared value
  float squaredValue = filteredValue * filteredValue;

  // Accumulate squared values
  sumSquared += squaredValue;

  // Update previous raw value
  previousRawValue = rawValue;

  // Calculate RMS value every numSamples iterations
  static int sampleCount = 0;
  sampleCount++;
  if (sampleCount >= numSamples) {
    // Calculate average squared value
    float averageSquared = sumSquared / numSamples;

    // Calculate RMS value
    rmsValue = sqrt(averageSquared);

    // Reset sample count and sum
    sampleCount = 0;
    sumSquared = 0.0;

    // Display RMS value
    Serial.print("RMS value: ");
    Serial.println(rmsValue);

    // Blink the LED segments based on the RMS value
    for (int i = 0; i < 10; i++) {
      // Check if the RMS value is greater than or equal to the threshold for the LED segment
      if (rmsValue >= segmentThresholds[i]) {
        // Turn on the LED segment
        digitalWrite(segmentPins[i], HIGH);
      } else {
        // Turn off the LED segment
        digitalWrite(segmentPins[i], LOW);
      }
    }
  }
}
```

Make sure to connect each LED segment to the correct digital output pin on your Arduino board. Adjust the `segmentPins` array and `segmentThresholds` array according to your specific setup and desired threshold values for each LED segment.

With this modification, each LED segment will blink on or off based on the RMS value. If the RMS value exceeds the threshold for a particular LED segment, that segment will be turned on; otherwise, it will be turned off.

Remember to include the necessary `pinMode()` statements to set the appropriate pins as output in your `setup()` function.

Feel free to adjust the threshold values and add additional logic or functionality as needed for your specific application.

