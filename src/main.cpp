/*

Good starting point for new dev with ESP32-C3
Hardware: esp32-c3 supermini, neopixel leds x2, button

// NOTE: pin 0-4 can be used as ADC but pin 5 cannot be used as ADC (bug in ESP32-C3)

Working:

*/

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Button2.h>

#define NEO_PIN 2
#define Button_PIN 3

#define NEO_NUMPIXELS 2
#define NEO_BRIGHTNESS 140
Adafruit_NeoPixel pixels(NEO_NUMPIXELS, NEO_PIN, NEO_GRBW);

int topMode = 0;
int bottomMode = 0;
bool topShow = true;
bool bottomShow = true;

static unsigned long previousMillis = 0;
static unsigned long previousMillisBottom = 0;
unsigned long currentMillis = millis();

//////////////////////////
Button2 button;

void pressed(Button2 &btn)
{
  Serial.println("pressed");
}
void released(Button2 &btn)
{
  Serial.print("released: ");
  Serial.println(btn.wasPressedFor());
}
void changed(Button2 &btn)
{
  Serial.println("changed");
}
void click(Button2 &btn)
{
  Serial.println("click\n");
  // loop through bottommode
  bottomMode = (bottomMode + 1) % 4;
  bottomShow = true;
  previousMillisBottom = millis();
}
void longClickDetected(Button2 &btn)
{
  Serial.println("long click detected");
}
void longClick(Button2 &btn)
{
  Serial.println("long click\n");
}
void doubleClick(Button2 &btn)
{
  Serial.println("double click\n");
  // loop through topmode
  topMode = (topMode + 1) % 4;
  topShow = true;
  previousMillis = millis();
}
void tripleClick(Button2 &btn)
{
  Serial.println("triple click\n");
  Serial.println(btn.getNumberOfClicks());
}
void tap(Button2 &btn)
{
  Serial.println("tap");
}
//////////////////////

// struct with sets of rgbw colors
struct colorObject
{
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t w;
};

const colorObject brightWhite = {255, 255, 255, 255};
const colorObject lime = {130, 210, 0, 0};
const colorObject amber = {255, 100, 0, 0};
const colorObject white = {200, 200, 200, 0};
const colorObject pureWhite = {0, 0, 0, 255};
const colorObject black = {0, 0, 0, 0};

colorObject topColor = lime;
colorObject bottomColor = white;

// function to breathe the top led
void breatheTop(const uint32_t increment, const uint8_t step)
{
  static uint32_t lastTimeChange = 0;
  static uint8_t direction = 1;
  const static uint8_t lowLimit = 55;
  static uint8_t value = lowLimit;
  if (millis() - lastTimeChange > increment)
  {
    value += (direction * step);
    value = constrain(value, lowLimit, 255);
    if (value <= lowLimit || value >= 255)
    {
      direction *= -1;
    }
    printf("value: %d\n", value);
    pixels.setPixelColor(1, pixels.Color(map(value, 0, 255, 0, topColor.r), map(value, 0, 255, 0, topColor.g), map(value, 0, 255, 0, topColor.b), map(value, 0, 255, 0, topColor.w)));
    pixels.show();
    lastTimeChange += increment;
  }
}

// function to breathe the bottom led
void breatheBottom(const uint32_t increment, const uint8_t step)
{
  static uint32_t lastTimeChange = 0;
  static uint8_t direction = 1;
  const static uint8_t lowLimit = 55;
  static uint8_t value = lowLimit;
  if (millis() - lastTimeChange > increment)
  {
    value += (direction * step);
    value = constrain(value, lowLimit, 255);
    if (value <= lowLimit || value >= 255)
    {
      direction *= -1;
    }

    pixels.setPixelColor(0, pixels.Color(map(value, 0, 255, 0, bottomColor.r), map(value, 0, 255, 0, bottomColor.g), map(value, 0, 255, 0, bottomColor.b), map(value, 0, 255, 0, bottomColor.w)));
    pixels.show();
    lastTimeChange += increment;
  }
}

void setup()
{
  pixels.begin();
  pixels.setBrightness(NEO_BRIGHTNESS);

  // set top led to lime
  pixels.setPixelColor(1, pixels.Color(topColor.r, topColor.g, topColor.b, topColor.w));
  // set bottom led to white
  pixels.setPixelColor(0, pixels.Color(bottomColor.r, bottomColor.g, bottomColor.b, bottomColor.w));
  pixels.show();

  button.begin(Button_PIN);
  button.setDoubleClickTime(500);
  button.setLongClickTime(1000);

  // button.setPressedHandler(pressed);
  // button.setReleasedHandler(released);
  button.setClickHandler(click);
  // button.setLongClickDetectedHandler(longClickDetected);
  // button.setLongClickHandler(longClick);
  // button.setLongClickDetectedRetriggerable(false);
  button.setDoubleClickHandler(doubleClick);
  // button.setTripleClickHandler(tripleClick);

  Serial.begin(115200);
}

void loop()
{
  // put your main code here, to run repeatedly:
  button.loop();

  // top led
  if (topMode == 0)
  {
    topColor = lime;
    pixels.setPixelColor(1, pixels.Color(topColor.r, topColor.g, topColor.b, topColor.w));
    pixels.show();
  }
  else if (topMode == 1)
  {
    topColor = amber;
    pixels.setPixelColor(1, pixels.Color(topColor.r, topColor.g, topColor.b, topColor.w));
    pixels.show();
  }
  else if (topMode == 2)
  {
    topColor = amber;
    // breatheTop(50, 10);
    // breatheTop(100, 20);
    // breatheTop(200, 40);

    // blink
    currentMillis = millis();
    if (currentMillis - previousMillis >= 800)
    { // 1 hz
      previousMillis = currentMillis;
      topShow = !topShow;
      if (topShow)
      {
        pixels.setPixelColor(1, pixels.Color(topColor.r, topColor.g, topColor.b, topColor.w));
      }
      else
      {
        pixels.setPixelColor(1, pixels.Color(0, 0, 0, 0));
      }
      pixels.show();
    }
  }
  else if (topMode == 3)
  { // off state
    topColor = black;
    pixels.setPixelColor(1, pixels.Color(topColor.r, topColor.g, topColor.b, topColor.w));
    pixels.show();
  }

  // bottom led
  if (bottomMode == 0)
  {
    bottomColor = white;
    pixels.setPixelColor(0, pixels.Color(bottomColor.r, bottomColor.g, bottomColor.b, bottomColor.w));
    pixels.show();
  }
  else if (bottomMode == 1)
  {
    bottomColor = amber;
    currentMillis = millis();
    if (currentMillis - previousMillisBottom >= 800)
    { // 1 hz
      previousMillisBottom = currentMillis;
      bottomShow = !bottomShow;
      if (bottomShow)
      {
        pixels.setPixelColor(0, pixels.Color(bottomColor.r, bottomColor.g, bottomColor.b, bottomColor.w));
      }
      else
      {
        pixels.setPixelColor(0, pixels.Color(0, 0, 0, 0));
      }
      pixels.show();
    }
  }
  else if (bottomMode == 2)
  {
    bottomColor = amber;
    currentMillis = millis();
    if (currentMillis - previousMillisBottom >= 400)
    { // 2 hz
      previousMillisBottom = currentMillis;
      bottomShow = !bottomShow;
      if (bottomShow)
      {
        pixels.setPixelColor(0, pixels.Color(bottomColor.r, bottomColor.g, bottomColor.b, bottomColor.w));
      }
      else
      {
        pixels.setPixelColor(0, pixels.Color(0, 0, 0, 0));
      }
      pixels.show();
    }
  }
  else if (bottomMode == 3)
  {
    bottomColor = black;
    pixels.setPixelColor(0, pixels.Color(bottomColor.r, bottomColor.g, bottomColor.b, bottomColor.w));
    pixels.show();
  }
}

// put function definitions here:
int myFunction(int x, int y)
{
  return x + y;
}