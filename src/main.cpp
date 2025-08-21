/*

Moving bench
Hardware:
esp32-c3 supermini
stepper motor (2x)
End switches 2x2 (magnet reed switch)
potmeter (speed control)
lipo + charger

// NOTE: pin 0-4 can be used as ADC but pin 5 cannot be used as ADC (bug in ESP32-C3)
// build in led: pin 8

What is does:

Move stepper(s) according to potmeter value speed in forward direction
When end switch is triggered, reverse motor direction


*/
#include <Arduino.h>
#include <AccelStepper.h>

// Motor A: 5, 6, 7, 8
// Juiste volgorde voor 28BYJ-48: IN1, IN3, IN2, IN4
const int motorA_pins[4] = {5, 6, 7, 8};
// Motor B: 9, 10, 20, 21
const int motorB_pins[4] = {9, 10, 20, 21};

// Limit switches: 0-3 (A: 0,1; B: 2,3)
const int limitA_left = 0;
const int limitA_right = 1;
const int limitB_left = 2;
const int limitB_right = 3;

// Potmeter op pin 4 (A0)
const int potPin = 4;

// AccelStepper objecten (FULL4WIRE = 4-wire mode)
AccelStepper stepperA(AccelStepper::FULL4WIRE, motorA_pins[0], motorA_pins[2], motorA_pins[1], motorA_pins[3]);
AccelStepper stepperB(AccelStepper::FULL4WIRE, motorB_pins[0], motorB_pins[2], motorB_pins[1], motorB_pins[3]);

void setup()
{
  pinMode(limitA_left, INPUT_PULLUP);
  pinMode(limitA_right, INPUT_PULLUP);
  pinMode(limitB_left, INPUT_PULLUP);
  pinMode(limitB_right, INPUT_PULLUP);
  pinMode(potPin, INPUT);

  // Snelheid en acceleratie instellen
  stepperA.setMaxSpeed(800);
  stepperB.setMaxSpeed(800);

  // Start richting vooruit
  stepperA.setSpeed(100); // beginwaarde
  stepperB.setSpeed(100);

  // Serial.begin(115200);

  int prevPot = 0;
  int threshold = 10;

  // Test: Zet alle motorA en motorB pinnen om de beurt hoog om leds te testen
  for (int i = 0; i < 4; i++)
  {
    digitalWrite(motorA_pins[i], HIGH);
    digitalWrite(motorB_pins[i], HIGH);
    delay(200);
    digitalWrite(motorA_pins[i], LOW);
    digitalWrite(motorB_pins[i], LOW);
  }
}

void loop()
{
  // Snelheid instellen via potmeter (tussen 50 en 1000 steps/sec)
  int potValue = analogRead(potPin);
  float speed = map(potValue, 0, 4095, 20, 1000);

  // // Debug alleen bij significante verandering
  // static int prevPot = 0;
  // const int threshold = 50;
  // if (abs(potValue - prevPot) > threshold)
  // {
  //   Serial.print("Pot value: ");
  //   Serial.print(potValue);
  //   Serial.print(", Speed: ");
  //   Serial.println(speed);
  //   prevPot = potValue;
  // }

  // Richting bepalen op basis van limit switches
  static int dirA = 1;
  static int dirB = 1;

  if (digitalRead(limitA_left) == LOW)
    dirA = 1;
  if (digitalRead(limitA_right) == LOW)
    dirA = -1;
  if (digitalRead(limitB_left) == LOW)
    dirB = 1;
  if (digitalRead(limitB_right) == LOW)
    dirB = -1;

  stepperA.setSpeed(dirA * speed);
  stepperB.setSpeed(dirB * speed);

  stepperA.runSpeed();
  stepperB.runSpeed();

  delay(1); // Houd deze delay minimaal, voorkomt CPU-hogging en stalling
}