//http://www.esp8266learning.com/wemos-mini-hc-sr04-ultrasonic-sensor.php

#define vccPin D5 // Vcc Pin
#define echoPin D7 // Echo Pin
#define trigPin D6 // Trigger Pin

long duration, distance; // Duration used to calculate distance
 
void setup() {
  Serial.begin (115200);
  pinMode(vccPin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}
 
void loop() {
  /*
  The following trigPin/echoPin cycle is used to determine the
  distance of the nearest object by bouncing soundwaves off of it.
  */
  digitalWrite(vccPin, HIGH);
  delay(100);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  digitalWrite(vccPin, LOW);
  //Calculate the distance (in cm) based on the speed of sound.
  distance = duration/58.2;
  Serial.println("Distance: " + String(distance) + " cm");
  
  //Delay 1s before next reading.
  delay(10000);
}
