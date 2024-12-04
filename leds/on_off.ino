# include <Adafruit_NeoPixel.h>
# include <ArduinoBLE.h>
// watchdog
# include <avr/wdt.h>

// pin connected to our lights
# define LED_PIN    6
// number of lights attached
# define LED_COUNT  20
// pin conected to our potentiometer
# define POT_PIN    A0

const int minimum = 0;
const int maximum = 255;
// where our button is connected
const int button_pin = 2;
// current state of button
bool button_state = false;
// last state of button (to detect change)
bool last_button_state = false;
// last time button was checked
unsigned long last_debounce_time = 0;
// debounce time (ms)
unsigned long debounce_delay = 50;
// tracking button
bool is_on = false;
// custom service that Arduino will provide over BLE
// service = collection of characteristics that defines specific functionality
// 180F = UUID that identifies service
BLEService myService ("180F");
// characteristic within service
// BLERead | BLEWrite = permissions for characteristic
// 20 = maximum length of characteristic value in bytes
BLECharacteristic myCharacteristic ("12345678-1234-5678-1234-56789abcdef0", BLERead | BLEWrite, 20);
// declare NeoPixel object: strip (number of pixels, pin number, flags)
// NEO_KHZ800 = 800 KHz bitstream for our strip
Adafruit_NeoPixel strip (LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
String value;
uint32_t color = strip.Color (255, 0, 0);
// track if color is controlled by Bluetooth or potentiometer
bool control_ble = false;
// potentiomter control
int last_pot_val = -1;
const int pot_threshold = 20;

void setup ()
{
  Serial.begin (9600);
  while (!Serial);
  // set button pin as input with internal pull-up
  pinMode (button_pin, INPUT_PULLUP);
  Serial.println ("circuit is off");
  if (!BLE.begin ())
  {
    Serial.println ("starting BLE failed");
    while (1);
  }
  // sets name that BLE device will advertise
  // when we scan for Bluetooh devices, this name shows up
  BLE.setLocalName ("NanoBLE");
  // specifies service that is advertised to devices scanning for BLE services
  BLE.setAdvertisedService (myService);
  // adds characteristic to service
  myService.addCharacteristic (myCharacteristic);
  // registers service with BLE stack --> Arduino can respond to requests related to service
  BLE.addService (myService);
  // initialize
  myCharacteristic.setValue ("0");
  BLE.advertise ();
  Serial.println ("BLE device active, waiting for connections...");
  // initialize strip
  strip.begin ();
  // turn off to start
  strip.clear ();
  strip.show ();
  // brightness maximum = 255
  strip.setBrightness (255);
}

void light_up ()
{
  for (int i = 0; i < LED_COUNT; i++)
  {
    // set color
    strip.setPixelColor (i, color);
    // update strip
    strip.show ();
    delay (15);
  }
}

void turn_off ()
{
  for (int i = LED_COUNT - 1; i >= 0; i--)
  {
    // turn off
    strip.setPixelColor (i, strip.Color (0, 0, 0));
    strip.show ();
    delay (15);
  }
}

void loop ()
{
  // reset watchdog timer to prevent reset
  wdt_reset ()
  // read button state
  bool reading = digitalRead (button_pin);
  // check if button state has changed
  if (reading != last_button_state)
  {
    last_debounce_time = millis ();
  }
  if ((millis () - last_debounce_time) > debounce_delay)
  {
    if (reading != button_state)
    {
      button_state = reading;
      
      if (button_state == LOW)
      {
        is_on = !is_on;
        if (is_on)
        {
          Serial.println ("on");
          light_up ();
        }
        else
        {
          Serial.println ("off");
          turn_off ();
        }
      }
    }
  }

  last_button_state = reading;

  if (is_on)
  {
    // central = valriable of type BLEDevice that holds information about connected client device
    BLEDevice central = BLE.central ();

    if (central)
    {
      Serial.print ("connected to: ");
      Serial.println (central.address ());

      while (central.connected ())
      {
        if (myCharacteristic.written ())
        {
          // buffer to hold value
          char buffer [20];
          size_t length = myCharacteristic.valueLength ();
          myCharacteristic.readValue (buffer, sizeof (buffer));

          if (length < sizeof (buffer))
          {
            buffer [length] = '\0';
          }
          // convert buffer to string
          value = String ((char *) buffer);
          Serial.print ("received: ");
          Serial.println (value);

          control_ble = true;

          if (value == "0")
          {
            color = strip.Color (255, 0, 0);
            Serial.println ("zero");
          }
          if (value == "1")
          {
            color = strip.Color (0, 0, 255);
            Serial.println ("one");
          }
          if (value == "2")
          {
            color = strip.Color (255, 255, 0);
            Serial.println ("two");
          }
          if (value == "3")
          {
            color = strip.Color (0, 255, 0);
            Serial.println ("three");
          }
          if (value == "4")
          {
            color = strip.Color (255, 0, 255);
            Serial.println ("four");
          }
          if (value == "5")
          {
            color = strip.Color (255, 255, 255);
            Serial.println ("five");
          }

          Serial.println ("color set by Bluetooth");
        }
      }
      Serial.print ("disconnected from central: ");
      Serial.println (central.address ());
      control_ble = false;
    }
    else if (!control_ble)
    {
      // read potentiometer value as input pin
      int pot_val = analogRead (POT_PIN);
      Serial.print ("potientiometer value: ");
      Serial.println (pot_val);
      // check if difference from last value is greater than threshold
      if (last_pot_val == -1 || abs (pot_val - last_pot_val) > pot_threshold)
      {
        int range = map (pot_val, 0, 1023, 0, 255);
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
          {
            color = strip.Color (255, g_up, 0);
            break;
          }
          case 1:
          {
            color = strip.Color (r_down, 255, 0);
            break;
          }
          case 2:
          {
            color = strip.Color (0, 255, b_up);
            break;
          }
          case 3:
          {
            color = strip.Color (0, g_down, 255);
            break;
          }
          case 4:
          {
            color = strip.Color (r_up, 0, 255);
            break;
          }
          case 5:
          {
            color = strip.Color (255, 0, b_down);
            break;
          }
        }
        last_pot_val = pot_val;
      }
    }
    for (int i = 0; i < LED_COUNT; i++)
    {
      strip.setPixelColor (i, color);
    }
    strip.show ();
  }
}
