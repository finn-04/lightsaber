# include <Adafruit_NeoPixel.h>
# include <ArduinoBLE.h>

# define LED_PIN    6
# define LED_COUNT  20
# define POT_PIN    A0
# define BUTTON_PIN 2  // Define the button pin

const int minimum = 0;
const int maximum = 255;

BLEService myService ("180F");
BLECharacteristic myCharacteristic ("12345678-1234-5678-1234-56789abcdef0", BLERead | BLEWrite, 20);

Adafruit_NeoPixel strip (LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
String value;
uint32_t color = strip.Color (255, 0, 0);
// Track if color is controlled by Bluetooth or potentiometer
bool control_ble = false;
volatile bool circuitOn = false;  // Track the circuit state

void setup () {
  Serial.begin (9600);
  
  if (!BLE.begin ()) {
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

  pinMode(BUTTON_PIN, INPUT); // Set the button pin as input (with external pull-down)
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), toggleCircuit, RISING); // Attach interrupt for rising edge

  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor (i, color);
  }
  strip.show ();
}

void loop () {
  if (circuitOn) {  // Only execute the loop if the circuit is on
    BLEDevice central = BLE.central ();

    if (central) {
      Serial.print ("connected to: ");
      Serial.println (central.address ());

      while (central.connected ()) {
        if (myCharacteristic.written ()) {
          char buffer [20];
          size_t length = myCharacteristic.valueLength ();
          myCharacteristic.readValue (buffer, sizeof (buffer));

          if (length < sizeof (buffer)) {
            buffer [length] = '\0';
          }

          value = String ((char *) buffer);
          Serial.print ("received: ");
          Serial.println (value);

          control_ble = true;

          if (value == "0") {
            color = strip.Color (255, 0, 0);
            Serial.println ("zero");
          } else if (value == "1") {
            color = strip.Color (0, 0, 255);
            Serial.println ("one");
          } else if (value == "2") {
            color = strip.Color (255, 255, 0);
            Serial.println ("two");
          } else if (value == "3") {
            color = strip.Color (0, 255, 0);
            Serial.println ("three");
          } else if (value == "4") {
            color = strip.Color (255, 0, 255);
            Serial.println ("four");
          } else if (value == "5") {
            color = strip.Color (255, 255, 255);
            Serial.println ("five");
          }

          Serial.println ("color set by Bluetooth");
        }
      }
      Serial.print ("disconnected from central: ");
      Serial.println (central.address ());
      control_ble = false;
    } else if (!control_ble) {
      int pot_val = analogRead (POT_PIN);
      Serial.print ("potentiometer value: ");
      Serial.println (pot_val);
      int range = map (pot_val, 0, 1023, 0, 255);
      int rgb_range = map (range, minimum, maximum, 0, 6);

      int g_up = map (range, 0, 42, minimum, maximum);
      int r_down = map (range, 43, 84, minimum, maximum);
      int b_up = map (range, 85, 127, minimum, maximum);
      int g_down = map (range, 128, 169, maximum, minimum);
      int r_up = map (range, 170, 212, minimum, maximum);
      int b_down = map (range, 213, 255, maximum, minimum);

      switch (rgb_range) {
        case 0:
          color = strip.Color (255, g_up, 0);
          break;
        case 1:
          color = strip.Color (r_down, 255, 0);
          break;
        case 2:
          color = strip.Color (0, 255, b_up);
          break;
        case 3:
          color = strip.Color (0, g_down, 255);
          break;
        case 4:
          color = strip.Color (r_up, 0, 255);
          break;
        case 5:
          color = strip.Color (255, 0, b_down);
          break;
      }
    }

    for (int i = 0; i < LED_COUNT; i++) {
      strip.setPixelColor (i, color);
    }
    strip.show ();
  }
}

// Interrupt Service Routine (ISR) for the button press
void toggleCircuit() {
  circuitOn = !circuitOn; // Toggle the circuit state
  Serial.print("Circuit is now: ");
  Serial.println(circuitOn ? "ON" : "OFF");

  // If turning off, turn off all pixels
  if (!circuitOn) {
    for (int i = 0; i < LED_COUNT; i++) {
      strip.setPixelColor(i, 0); // Set to off
    }
    strip.show();
  }
}