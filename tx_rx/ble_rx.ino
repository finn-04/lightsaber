# include <ArduinoBLE.h>

// custom service that Arduino will provide over BLE
// service = collection of characteristics that define specific functionlity
// 180F = UUID that identifies service
BLEService myService ("180F");
// characteristic within service
// 2A19 = UUID that identifies characteristic
// BLERead | BLEWrite = permissions for characteristic
// 20 = maximum length of characteristic value in bytes
BLECharacteristic myCharacteristic ("12345678-1234-5678-1234-56789abcdef0", BLERead | BLEWrite, 20);

void setup() {
  // put your setup code here, to run once:
  Serial.begin (9600);
  while (!Serial);

  if (!BLE.begin ())
  {
    Serial.println ("starting BLE failed");
    while (1);
  }
  // sets name that BLE device will advertise
  // when we scan for Bluetooth devices, this name shows up
  BLE.setLocalName ("NanoBLE");
  // specifies service that is advertised to devices scanning for BLE services
  BLE.setAdvertisedService (myService);
  // adds characteristic to service
  myService.addCharacteristic (myCharacteristic);
  // registers service with BLE stack --> Arduino can respond to requests related to service
  BLE.addService (myService);
  // initalize
  myCharacteristic.setValue ("0");
  BLE.advertise ();
  Serial.println ("BLE device active, waiting for connections...");
}

void loop() {
  // put your main code here, to run repeatedly:
  // central = variable of type BLEDevice that holds information about connected client device
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
        String value((char *) buffer);
        Serial.print ("received: ");
        Serial.println (value);
        if (value == "0")
        {
          Serial.println ("zerooo");
        }
      }
    }
    Serial.print ("disconnected from central: ");
    Serial.println (central.address ());
  }
}

