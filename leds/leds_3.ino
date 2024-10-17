# include <Adafruit_NeoPixel.h>
// pin connected to our lights
# define LED_PIN 6
// number of lights attached
# define LED_COUNT 20
// pin connected to potentiometer output
int pot_pin = A0;
// variable to store input from potentiometer
int pot_val = 0;
const int minimum = 0;
const int maximum = 255;
// declare NeoPixel object: strip (number of pixels, pin number, flags)
// NEO_KHZ800 = 800 KHz bitstream for our strip
Adafruit_NeoPixel strip (LED_COUNT, LED_PIN, NEO_RGB + NEO_KHZ800);

void setup() {
  // put your setup code here, to run once:
  // initialize strip
  strip.begin ();
  // turn off to start
  strip.show ();
  // brightness max = 255
  strip.setBrightness (100);
  Serial.begin (9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  // read potentiometer value at input pin
  pot_val = analogRead (A0);
//  Serial.println ("pot val");
  Serial.println (pot_val);
  int range = map (pot_val, 0, 1023, minimum, maximum);
  int rgb_range = map (range, minimum, maximum, 0, 6);

  int g_up = map (range, 0, 42, minimum, maximum);
  int r_down = map (range, 43, 84, minimum, maximum);
  int b_up = map (range, 85, 127, minimum, maximum);
  int g_down = map (range, 128, 169, maximum, minimum);
  int r_up = map (range, 170, 212, minimum, maximum);
  int b_down = map (range, 213, 255, maximum, minimum);

  switch (rgb_range)
  {
    case 0:
      for (int i = 0; i < LED_COUNT; i++)
      {
        strip.setPixelColor (i, strip.Color (255, g_up, 0));
      }
      break;
    case 1:
      for (int i = 0; i < LED_COUNT; i++)
      {
        strip.setPixelColor (i, strip.Color (r_down, 255, 0));
      }
      break;
    case 2:
      for (int i = 0; i < LED_COUNT; i++)
      {
        strip.setPixelColor (i, strip.Color (0, 255, b_up));
      }
      break;
    case 3:
      for (int i = 0; i < LED_COUNT; i++)
      {
        strip.setPixelColor (i, strip.Color (0, g_down, 255));
      }
      break;
    case 4:
      for (int i = 0; i < LED_COUNT; i++)
      {
        strip.setPixelColor (i, strip.Color (r_up, 0 , 255));
      }
      break;
    case 5:
      for (int i = 0; i < LED_COUNT; i++)
      {
        strip.setPixelColor (i, strip.Color (255, 0, b_down));
      }
      break;
  }
  strip.show ();
  delay (250);
}
