# include <ArduinoBLE.h>
# include <Adafruit_NeoPixel.h>
// pin connected to our lights
# define LED_PIN 6
// number of lights attached
# define LED_COUNT 31
Adafruit_NeoPixel strip (LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  // put your setup code here, to run once:
  // initialize strip
  Serial.begin (9600);
  while (!Serial);
  Serial.println ("hello");
  delay(100);
  strip.begin ();
  // turn off to start
  // brightness max = 255
  strip.setBrightness (100);


}

void loop() {
  for (int i = 0; i < LED_COUNT; i++)
  {
    strip.setPixelColor (i, strip.Color (255, 0, 0));
  }
  strip.show ();
  delay (250);
}
