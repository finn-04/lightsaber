#include <Wire.h>
#include <MPU6050.h>
#include <SD.h>
#include <TMRpcm.h>

MPU6050 mpu;
TMRpcm audio;

const int speakerPin = 9;
const int buttonPin = 2;
const int threshold = 3000;   // Movement threshold
const int chipSelect = 10;    // SD card CS pin
int16_t lastAx = 0;
bool isPlaying = false;       // Tracks if button or movement audio is currently playing
bool ishum = false;           // Tracks if hum is currently playing
bool deviceOn = false;        // Tracks if the device is "on"
int buttonpress = 0;
// 2 seconds before checking to play swing sound again
unsigned long last_motion_time = 0;
const unsigned long motion_cooldown = 3000;
unsigned long start_time = millis ();


void setup() {
  Serial.begin(9600);
  Serial.println("Starting setup...");

  Wire.begin();
  
  // Initialize MPU6050
  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed!");
    while (1);
  }
  Serial.println("MPU6050 initialized successfully.");

  // Initialize SD card
  if (!SD.begin(chipSelect)) {
    Serial.println("SD card initialization failed!");
    while (1);
  }
  Serial.println("SD card initialized.");
  audio.setVolume(7);

  // Initialize audio library
  audio.speakerPin = speakerPin;

  // Set pin modes
  pinMode(speakerPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);  // Configure button as input with pull-up
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

  // Check if button is pressed (active LOW with pull-up)
  bool buttonPressed = digitalRead(buttonPin) == LOW;

  // Toggle device state on button press
  if (buttonPressed) {
    delay(50); // Debounce delay
    buttonpress++;
    start_time = millis ();
    deviceOn = !(buttonpress % 2 == 0);

    if (deviceOn) {
      Serial.println("Device is ON. Playing 'on' sound.");
      audio.play("on.wav");  // Play "on" sound
      while (audio.isPlaying()) delay(100);  // Wait for "on" sound to finish
    } else {
      Serial.println("Device is OFF. Playing 'off' sound.");
      stopHum();              // Stop background audio
      audio.play("off.wav");  // Play "off" sound
      while (audio.isPlaying()) delay(100);  // Wait for "off" sound to finish
    }
    delay(300); // Additional debounce delay to avoid repeated toggling
  }

  // If device is ON, allow hum and movement sounds
  if (deviceOn) 
  {
    // Get accelerometer values
    mpu.getAcceleration(&ax, &ay, &az);

    // Play hum sound if no other audio is playing
    if (!isPlaying && !audio.isPlaying()) {
      Serial.println("Playing background audio (hum).");
      audio.play("hum.wav");
      ishum = true;
    }


	if ((abs (ax - lastAx) > threshold) && abs (millis () - last_motion_time) > motion_cooldown)
	{
		Serial.println ("playing movement sound");
		stopHum ();
		audio.play ("swing8.wav");
		isPlaying = true;

		while (audio.isPlaying ());

		isPlaying = false;

		Serial.println ("movement sound finished");
		last_motion_time = millis ();
	}

	lastAx = ax;
  }
	


    // if (millis () - last_motion_time > motion_cooldown)
    // {
      // int16_t deltaX = ax - lastAx;
      // int16_t deltaY = ay - lastAy;
      // int16_t deltaZ = az - lastAz;
      // long movement_magnitude = sqrt (sq (deltaX) + sq (deltaY) + sq (deltaZ));
      // Play movement sound if significant movement is detected
      // if (!isPlaying && abs(ax - lastAx) > threshold && millis () - start_time > 2000) {
      // if (!isPlaying && abs(ax - lastAx) > threshold) {
        // Serial.println("Significant movement detected. Playing movement sound.");
        // stopHum();  // Stop background hum
        // audio.play("swing8.wav");  // Play movement sound
        // isPlaying = true;

        // Wait for movement sound to finish
        // while (audio.isPlaying()) {
          // delay(100);
        // }
        // isPlaying = false;
        // Serial.println("Movement sound finished.");
        // last_motion_time = millis ();
        // lastAx = ax;
        // lastAy = ay;
        // lastAz = az;
      // }
    // }

  //} 
  else {
    // If device is OFF, ensure no audio is playing
    stopHum();
  }

  delay(100);  // Stability delay
}

void stopHum() {
  if (ishum) {
    audio.stopPlayback();
    ishum = false;
    Serial.println("Stopping background audio.");
  }
}
