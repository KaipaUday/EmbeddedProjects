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


Sound sensor(4 pin)
Pin diagram and sensor @ https://circuitdigest.com/microcontroller-projects/interfacing-sound-sensor-with-arduino
When sensor potentiometer adjuster is lessthan 512, LED2 will blink, A0 gives range between 0-1023, sensitivity can be adjusted to give 1023 at normal level.
https://datasheetspdf.com/pdf-file/1402048/Joy-IT/KY-038/1


part 1: read sensor input.

done : Analog pin poutputs a value of 590 and above. Max around 680.

Part 2: interface two 8-bit shift registers in series check output on LED.
Done: 1023 is passed to serial input. sent all the 10 leds on strip glowing, 10 1's =1023

