#include <Wire.h>
#include <MPU6050.h>
#include <SD.h>
#include <TMRpcm.h>

MPU6050 mpu;
TMRpcm audio;

const int speakerPin = 9;
const int buttonPin = 3;
// movement must be above this
const int threshold = 3000;
// SD card pin
const int chipSelect = 10;
int16_t lastAx = 0;
bool isPlaying = false;
bool ishum = false;
bool deviceOn = false;
int buttonpress = 0;
// 2 seconds before checking to play swing sound again
unsigned long last_motion_time = 0;
const unsigned long motion_cooldown = 2000;
unsigned long start_time = millis ();


void setup() {
  Serial.begin(9600);
  Serial.println("Starting setup...");

  Wire.begin();
  
  // initialize MPU6050
  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed!");
    while (1);
  }
  Serial.println("MPU6050 initialized successfully.");

  // initialize SD card
  if (!SD.begin(chipSelect)) {
    Serial.println("SD card initialization failed!");
    while (1);
  }
  Serial.println("SD card initialized.");
  audio.setVolume(7);

  // initialize audio library
  audio.speakerPin = speakerPin;

  // set pin modes
  pinMode(speakerPin, OUTPUT);
  // button as input with pull-up
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.println("Setup complete.");
  if (deviceOn == false)
  {
    Serial.println ("device is off");
  }

  audio.quality(0);
}

void loop() {
  int16_t ax, ay, az;
  // audio.play("on.wav");

  // check if button is pressed (active LOW with pull-up)
  bool buttonPressed = digitalRead (buttonPin) == LOW;

  // toggle device state on button press
  if (buttonPressed) 
  {
  	// debounce
    delay(50);
    buttonpress++;
    start_time = millis ();
    deviceOn = !(buttonpress % 2 == 0);

    if (deviceOn) 
	{
      Serial.println ("Device is ON. Playing 'on' sound.");
      audio.play ("on.wav");
      while (audio.isPlaying ()) delay (100);
    } 
	else 
	{
      Serial.println ("Device is OFF. Playing 'off' sound.");
      stopHum ();
      audio.play ("off.wav");
      while (audio.isPlaying ()) delay (100);
    }
	// debounce
    delay (300);
  }

  // if device is ON, allow hum and movement sounds
  if (deviceOn) 
  {
    // get accelerometer values
    mpu.getAcceleration (&ax, &ay, &az);

    // play hum sound if no other audio is playing
    if (!isPlaying && !audio.isPlaying()) {
      Serial.println ("Playing background audio (hum).");
      audio.play ("hum.wav");
      ishum = true;
    }
    if (millis () - last_motion_time > motion_cooldown)
    {
      // int16_t deltaX = ax - lastAx;
      // int16_t deltaY = ay - lastAy;
      // int16_t deltaZ = az - lastAz;
      // long movement_magnitude = sqrt (sq (deltaX) + sq (deltaY) + sq (deltaZ));
      // play movement sound if significant movement is detected
      if (!isPlaying && abs (ax - lastAx) > threshold && millis () - start_time > 2000) 
	  {
        Serial.println ("Significant movement detected. Playing movement sound.");
        stopHum ();
        audio.play("swing8.wav");
        isPlaying = true;
        while (audio.isPlaying ()) 
		{
          delay (100);
        }
        isPlaying = false;
        Serial.println ("Movement sound finished.");
        last_motion_time = millis ();
        lastAx = ax;
        // lastAy = ay;
        // lastAz = az;
      }
    }
  } 
  else 
  {
    // if device is OFF, ensure no audio is playing
    stopHum ();
  }

  delay (100); 
  }

void stopHum () {
  if (ishum) {
    audio.stopPlayback ();
    ishum = false;
    Serial.println ("Stopping background audio.");
  }
}
