# include <FastLED.h>

# define NUM_LEDS 4
# define LED_PIN 2

CRGB leds [NUM_LEDS];

void setup() {
  // put your setup code here, to run once:
  FastLED.addLeds <WS2811, LED_PIN, GRB> (leds, NUM_LEDS);
  // 0 - 255
  FastLED.setBrightness (50);

}

void loop() {
  // put your main code here, to run repeatedly:
  // on event
  for (int i = 0; i < NUM_LEDS, i++)
  {
    leds [i] = CRGB::Red;
    // leds [i].red = 255;
    // leds [i] = CRGB (0, 255, 2);
    // leds [i].setRGB (255, 0, 50);
    FastLED.show ();
    delay (50);
  }
  // off event
  for (int i = NUM_LEDS - 1; i >-= 0; i--)
  {
    leds [i] = CRGB::Black;
    FastLED.show ();
    delay (50);
  }
  // for delaying
  // delay (ms);
  // off
  // CRGB::Black; 
}

