#include <LiquidCrystal_I2C.h>
//Author :Uday kaipa

const int trigPin = 5;
const int echoPin = 6;
const int ledGreen =12;
const int ledYellow = 11;
const int ledRed =10;
const int buzzPin = 3;
int distance;

String flag="NAN";

LiquidCrystal_I2C lcd(0x27,20,4);


void led(String color){
  // function to glow one of the Red, Green or Yellow LED.
  // Arguments : R | Y | G

  // Serial.println(color); 
  if (color=="G"){
    digitalWrite(ledGreen, HIGH);
    digitalWrite(ledYellow, LOW);
    digitalWrite(ledRed, LOW);
  }
  else if(color=="Y"){
    digitalWrite(ledGreen, LOW);
    digitalWrite(ledYellow, HIGH);
    digitalWrite(ledRed, LOW);
  }
  else if (color=="R"){
    digitalWrite(ledGreen, LOW);
    digitalWrite(ledYellow, LOW);
    digitalWrite(ledRed, HIGH);
  }
  return 0;

}

void setup() {
  // put your setup code here, to run once:
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT); 
  pinMode(ledGreen, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(buzzPin, OUTPUT);
  Serial.begin(9600);

  lcd.init();	// initialize the lcd
  lcd.backlight();
}


void loop() {
  
  // put your main code here, to run repeatedly:

  //  ultrasonic readings as per docs
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  int duration = pulseIn(echoPin, HIGH);
  distance = duration/58;  //cms as per guide


  if((distance >= 250 && distance < 400)&&(flag!="G")){
    flag="G";
    led(flag);
    lcd.clear();
    lcd.print("Distance Ok");
  }
  else if((distance >=50 && distance < 250)&&(flag!="Y")){
    flag="Y";
    led(flag);
    lcd.clear();
    lcd.print("Distance Caution");
  }
  else if(distance < 50 && flag!="R"){
    flag="R";
    led(flag);
    lcd.clear();
    lcd.print("Distance Danger");
  }
  tone(buzzPin,3000,(400-distance));
  delay(500);
}
