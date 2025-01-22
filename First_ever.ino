#include <Arduino.h> // Include Arduino library (optional but good practice)
int switchstate; // Declare the switchstate variable
void setup() {
  // Initialize Serial communication at 9600 baud rate
  Serial.begin(9600);
  // Declare the LED pins as Outputs
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  // Declare the switch pin as input
  pinMode(2, INPUT);
}
void loop() {
  // Read value of the switch
  // digitalRead() checks to see if there is voltage on the pin or not
  switchstate = digitalRead(2);
  // Print the switch state to the Serial Monitor
  Serial.println(switchstate);
  // If the button is not pressed, turn on the green LED and off the red LEDs
  if (switchstate == 0) {
    digitalWrite(3, HIGH); // Green LED on
    digitalWrite(4, LOW);  // Red LED off
    digitalWrite(5, LOW);  // Red LED off
  }
  else {
    digitalWrite(3, LOW);  // Green LED off
    digitalWrite(4, HIGH); // Red LED on
    digitalWrite(5, LOW);  // Red LED off
    delay(250);
    digitalWrite(4, LOW);  // Red LED off
    digitalWrite(5, HIGH); // Red LED on
    delay(250);
  }
}