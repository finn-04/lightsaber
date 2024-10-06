const int r = 3;
const int g = 5;
const int b = 6;

const int minimum = 0;
const int maximum = 255;
// pin that potentiometer output is connected to
int pot_pin = A0;
// variable to store input from potentiometer
int pot_val = 0;


void setup() {
  // put your setup code here, to run once:
  pinMode (r, OUTPUT);
  pinMode (g, OUTPUT);
  pinMode (b, OUTPUT);
  Serial.begin (9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  // read potentiometer value at input pin
  pot_val = analogRead (A0);
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
      analogWrite (g, g_up);
      digitalWrite (r, HIGH);
      digitalWrite (b, LOW);
      break;
    case 1:

      analogWrite (r, r_down);
      digitalWrite (g, HIGH);
      digitalWrite (b, LOW);
      break;
    case 2:
      analogWrite (b, b_up);
      digitalWrite (g, HIGH);
      digitalWrite (r, LOW);
      break;
    case 3:
      analogWrite (g, g_down);
      digitalWrite (b, HIGH);
      digitalWrite (r, LOW);
      break;
    case 4:
      analogWrite (r, r_up);
      digitalWrite (b, HIGH);
      digitalWrite (g, LOW);
      break;
    case 5:
      analogWrite (b, b_down);
      digitalWrite (r, HIGH);
      digitalWrite (b, LOW);
      break;
  }

  delay (1);
}

