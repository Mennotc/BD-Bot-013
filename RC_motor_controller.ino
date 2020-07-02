// Controller pins
const int CH_1_PIN = 6; // Speed and forward-reverse
const int CH_2_PIN = 5; // Steering
const int DIR_LM = 8; // Direction control left motor
const int PWM_LM = 9; // speed control left motor
const int DIR_RM = 12; // Direction control right motor
const int PWM_RM = 10; // speed control right motor

// variables to store the values of each channel
int ch1;
float ch2;
int pwmLM;
int pwmRM;
bool dir;

// Parameters
const int deadzone = 20;
float f;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(CH_1_PIN, INPUT);
  pinMode(CH_2_PIN, INPUT);
  pinMode(DIR_LM, OUTPUT);
  pinMode(PWM_LM, OUTPUT);
  pinMode(DIR_RM, OUTPUT);
  pinMode(PWM_RM, OUTPUT);

  int deadzone = 20;

  // Set the PWM Frequency to 3906 Hz to avoid "humming" on low speeds and improve efficiency
  setPwmFrequency(PWM_LM, 8);
  setPwmFrequency(PWM_RM, 8);
}

void loop() {
  ch1 = pulseIn (CH_1_PIN, HIGH); // read and store channel 1 input
  ch2 = pulseIn (CH_2_PIN, HIGH); // read and store channel 2 input
  ch1 = map(ch1, 1000, 2000, -300, 300); //
  ch1 = constrain(ch1, -255, 255); // make sure the value is between -255 and 255
  ch1 = ch1 * -1; //My RC transmitter sends a negative value for forward. It is more intuitive to change the sign of the value here.
  ch2 = map(ch2, 1000, 2000, -300, 300);
  ch2 = constrain(ch2, -255, 255);

  // The transmitter doesn't have a stable zero point. To prevent flutter, all values between -20 and 20 will be set to 0
  if (abs(ch1) <= deadzone) {
    ch1 = 0;
  }
  if (abs(ch2) <= deadzone) {
    ch2 = 0;
  }

  // Determine power to motors to be able to change direction
  // Steering is done by decreasing the power on 1 motor. This is done by applying a factor, calculated in this section
  f = 1 - (abs(ch2) / 255);
  if (f < 0.1) {
    f = 0.1;
  }

  if (ch2 < 0) {
    pwmLM = f * abs(ch1); // Go right when value <0
    pwmRM = abs(ch1);
  }
  else if (ch2 > 0) {
    pwmRM = f * abs(ch1); // Go left when value >0
    pwmLM = abs(ch1);
  }
  else {
    pwmRM = abs(ch1);
    pwmLM = abs(ch1);
  }

  // The direction is set in this part. Reverse for values < 0
  if (ch1 < 0) {
    dir = LOW;
  }
  else {
    dir = HIGH;
  }

  // Drive motors
  digitalWrite(DIR_LM, dir);
  analogWrite(PWM_LM, pwmLM);
  digitalWrite(DIR_RM, dir);
  analogWrite(PWM_RM, pwmRM);

//  Print output to check calculations
//  Serial.print("Channel 1: ");
//  Serial.print(ch1);
//  Serial.print("  Channel 2: ");
//  Serial.print(ch2);
//  Serial.print("  factor: ");
//  Serial.print(f);
//  Serial.print("  Left motorspeed ");
//  Serial.print(pwmLM);
//  Serial.print("  Right motorspeed ");
//  Serial.print(pwmRM);
//  Serial.print("  Direction ");
//  Serial.println(dir);
}

void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if (pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch (divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if (pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if (pin == 3 || pin == 11) {
    switch (divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x07; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}
