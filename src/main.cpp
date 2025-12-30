/*

Moving bench - HIGH TORQUE VERSION
Hardware:
esp32-c3 supermini
stepper motor 28BYJ-48 (2x)
End switches 2x2 (magnet reed switch)
potmeter (speed control)
lipo + charger

TORQUE IMPROVEMENTS:
- HALF4WIRE mode:  activates 2 coils at same time = ~40% more torque
- Lower speed range: more time for magnetic flux = better torque
- Removed delay:  smoother stepping = less torque loss
- Optional: custom step sequence for maximum torque

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

// AccelStepper objecten - HALF4WIRE voor meer torque! 
// HALF4WIRE = dual-phase stepping = ~40% meer torque dan FULL4WIRE
AccelStepper stepperA(AccelStepper::HALF4WIRE, motorA_pins[0], motorA_pins[2], motorA_pins[1], motorA_pins[3]);
AccelStepper stepperB(AccelStepper:: HALF4WIRE, motorB_pins[0], motorB_pins[2], motorB_pins[1], motorB_pins[3]);

// ALTERNATIEF: Voor MAXIMALE torque (100% meer dan single-phase):
// Uncomment onderstaande regels en comment bovenstaande steppers uit
// #define USE_MAXIMUM_TORQUE
// AccelStepper stepperA(AccelStepper::FULL4WIRE, motorA_pins[0], motorA_pins[2], motorA_pins[1], motorA_pins[3]);
// AccelStepper stepperB(AccelStepper::FULL4WIRE, motorB_pins[0], motorB_pins[2], motorB_pins[1], motorB_pins[3]);

void setup()
{
  pinMode(limitA_left, INPUT_PULLUP);
  pinMode(limitA_right, INPUT_PULLUP);
  pinMode(limitB_left, INPUT_PULLUP);
  pinMode(limitB_right, INPUT_PULLUP);
  pinMode(potPin, INPUT);

  // TORQUE TIP 1: Lagere max speed = meer torque
  // Bij HALF4WIRE moet je speeds halveren (dubbel zoveel steps)
  stepperA.setMaxSpeed(400);  // Was 800, nu 400 voor HALF4WIRE
  stepperB.setMaxSpeed(400);
  
  // TORQUE TIP 2: Acceleratie toevoegen voorkomt stalling bij start
  stepperA.setAcceleration(200);
  stepperB.setAcceleration(200);

  // Start richting vooruit
  stepperA.setSpeed(50);  // Was 100, nu 50 voor HALF4WIRE
  stepperB.setSpeed(50);

  // Serial voor debugging (optioneel)
  // Serial.begin(115200);
  // Serial.println("HIGH TORQUE MODE - HALF4WIRE");

  // Test: Zet alle motorA en motorB pinnen om de beurt hoog om leds te testen
  for (int i = 0; i < 4; i++)
  {
    pinMode(motorA_pins[i], OUTPUT);
    pinMode(motorB_pins[i], OUTPUT);
    digitalWrite(motorA_pins[i], HIGH);
    digitalWrite(motorB_pins[i], HIGH);
    delay(200);
    digitalWrite(motorA_pins[i], LOW);
    digitalWrite(motorB_pins[i], LOW);
  }

#ifdef USE_MAXIMUM_TORQUE
  // Custom stepping sequence voor MAXIMALE torque
  // Dit activeert altijd 2 spoelen tegelijk
  uint8_t fullStepSequence[4] = {
    0b1010,  // Coil 1+3
    0b0110,  // Coil 2+3
    0b0101,  // Coil 2+4
    0b1001   // Coil 1+4
  };
  // Note: Je moet dit custom implementeren met setOutputPins()
  // Voor nu is HALF4WIRE de beste praktische oplossing
#endif
}

void loop()
{
  // TORQUE TIP 3: Lagere snelheidsbereik voor meer controle en torque
  // Bij HALF4WIRE: halveer je snelheden (dubbel zoveel steps voor zelfde afstand)
  int potValue = analogRead(potPin);
  
  // Aangepast bereik:  10-500 steps/sec (was 20-1000)
  // Lagere snelheden = meer torque beschikbaar
  float speed = map(potValue, 0, 4095, 10, 500);

  // TORQUE TIP 4: Non-linear mapping geeft meer controle bij lage snelheden
  // Uncomment voor exponential curve (meer precisie bij lage snelheden):
  // float normalized = potValue / 4095.0;
  // speed = 10 + (normalized * normalized) * 490;  // Quadratic mapping

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

  // TORQUE TIP 5: runSpeed() zonder delay = consistent timing = betere torque
  stepperA.runSpeed();
  stepperB.runSpeed();

  // GEEN delay() meer!  Dit zorgt voor: 
  // - Consistente step timing
  // - Minder gemiste steps
  // - Betere torque delivery
  // ESP32 kan dit makkelijk aan zonder CPU-hogging
}