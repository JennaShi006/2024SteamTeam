#include <Stepper.h>

// # steps for full 360-degree rotation, change to fit your motor
int stepsPerRevolution = 2048;

// set a speed for the stepper motor
int rpm = 10;

// initialize stepper library on pins 8 - 11
// pin order IN1, IN3, IN2, IN4
Stepper myStepper (stepsPerRevolution, 8, 10, 9, 11);

// Button pin
const int buttonPin = 2;
bool motorRunning = false;  // Tracks motor state
bool lastButtonState = HIGH; // Previous button state

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);  // Enable internal pull-up resistor
  myStepper.setSpeed(rpm);
  Serial.begin(9600);
}

void loop() {
  bool buttonState = digitalRead(buttonPin);
  if (buttonState == LOW && lastButtonState == HIGH) {  
    delay(50);  // Simple debounce
    motorRunning = !motorRunning;  // Toggle motor state
    Serial.println(motorRunning ? "Motor Started" : "Motor Stopped");
    
    while (digitalRead(buttonPin) == LOW); // Wait for button release
  }
  lastButtonState = buttonState;

  // Run motor continuously if toggled on
  if (motorRunning) {
    myStepper.step(10);  // Small steps for smooth motion
  }
}