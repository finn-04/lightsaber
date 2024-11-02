# include <Adafruit_NeoPixel.h>
# include <ArduinoBLE.h>

# define LED_PIN    6
# define LED_COUNT  20
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

BLEService myService ("180F");
BLECharacteristic myCharacteristic ("12345678-1234-5678-1234-56789abcdef0", BLERead | BLEWrite, 20);

Adafruit_NeoPixel strip (LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
String value;
uint32_t color = strip.Color (255, 0, 0);
// track if color is controlled by Bluetooth or potentiometer
bool control_ble = false;

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

  BLE.setLocalName ("NanoBLE");
  BLE.setAdvertisedService (myService);
  myService.addCharacteristic (myCharacteristic);
  BLE.addService (myService);
  myCharacteristic.setValue ("0");
  BLE.advertise ();
  Serial.println ("BLE device active, waiting for connections...");
  strip.begin ();
  strip.show ();
  strip.setBrightness (255);

  for (int i = 0; i < LED_COUNT; i++)
  {
    strip.setPixelColor (i, color);
  }
  
  strip.show ();
}

void loop ()
{
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
        }
        else
        {
          Serial.println ("off");
        }
      }
    }
  }

  last_button_state = reading;

  if (is_on)
  {
    BLEDevice central = BLE.central ();

    if (central)
    {
      Serial.print ("connected to: ");
      Serial.println (central.address ());

      while (central.connected ())
      {
        if (myCharacteristic.written ())
        {
          char buffer [20];
          size_t length = myCharacteristic.valueLength ();
          myCharacteristic.readValue (buffer, sizeof (buffer));

          if (length < sizeof (buffer))
          {
            buffer [length] = '\0';
          }

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
      int pot_val = analogRead (POT_PIN);
      Serial.print ("potientiometer value: ");
      Serial.println (pot_val);
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
    }
    for (int i = 0; i < LED_COUNT; i++)
    {
      strip.setPixelColor (i, color);
    }
    strip.show ();
  }
}
