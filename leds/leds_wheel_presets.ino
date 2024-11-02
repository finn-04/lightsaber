# include <Adafruit_NeoPixel.h>
# include <ArduinoBLE.h>

# define LED_PIN    6
# define LED_COUNT 20

BLEService myService ("180F");
BLECharacteristic myCharacteristic ("12345678-1234-5678-1234-56789abcdef0", BLERead | BLEWrite, 20);

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// storing received preset
String value;
uint32_t color = strip.Color (255, 0, 0);

void setup() {
  Serial.begin (9600);
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
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // turn OFF all pixels ASAP
  strip.setBrightness(255);

  for (int i = 0; i < LED_COUNT; i++)
  {
    Serial.println (value);
    strip.setPixelColor (i, color);
  }
  strip.show ();

}


// loop() function -- runs repeatedly as long as board is on ---------------

void loop() 
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
        // update with received
        value = String ((char *) buffer);
        Serial.print ("received: ");
        Serial.println (value);

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
        // update strip
        for (int i = 0; i < LED_COUNT; i++)
        {
          Serial.print ("VALUE: ");
          Serial.println (value);
          strip.setPixelColor (i, color);
        }
      strip.show ();
      }
    }
    Serial.print ("disconected from central: ");
    Serial.println (central.address ());
  }
}
