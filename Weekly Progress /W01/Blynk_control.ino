#define BLYNK_TEMPLATE_ID   "TMPL2k6VKVGql"
#define BLYNK_TEMPLATE_NAME "Connection_of_boxing_bots_from_Harish_to_the_bot"
#define BLYNK_AUTH_TOKEN    "B13_JGf0JzTasIhX2yM_tTiRY2sTFi7p"

#include <Bluepad32.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

const char* ssid     = "Jov-Fi";
const char* password = "Jovsecuregate2014";

const int motor1_enable_pin   = 13;
const int motor2_enable_pin   = 25;
const int motor1_forward_pin  = 12;
const int motor1_backward_pin = 14;
const int motor2_forward_pin  = 27;
const int motor2_backward_pin = 26;
//const int buzzer               = 15;

const int motor_range_max =  200;
const int motor_range_min = -200;

const int servo1_pin = 2;
const int servo2_pin = 4;
const int servo3_pin = 5;
const int servo4_pin = 18;
const int servo5_pin = 19;

Servo servo1, servo2, servo3, servo4, servo5;

ControllerPtr myControllers[BP32_MAX_GAMEPADS] = { nullptr };
bool controllerConnected = false;

int  blynk_j1_x = 127, blynk_j1_y = 127;
int  blynk_j2_x = 127, blynk_j2_y = 127;
bool blynk_fwd   = false;
bool blynk_bwd   = false;
bool blynk_left  = false;
bool blynk_right = false;

bool hookInProgress      = false;
const bool blynkLocked   = true;   // Blynk-only mode, gamepad permanently blocked

void performL1Action() {
  if (hookInProgress) return;
  hookInProgress = true;
  servo1.write(90);  servo2.write(90);
  servo3.write(90);  servo4.write(90);
  servo5.write(45);  servo2.write(40);  servo1.write(180);
  delay(400);
  servo1.write(90);  servo2.write(90);
  servo3.write(90);  servo4.write(90);  servo5.write(90);
  delay(100);
  hookInProgress = false;
}

void performR1Action() {
  if (hookInProgress) return;
  hookInProgress = true;
  servo1.write(90);  servo2.write(90);
  servo3.write(90);  servo4.write(90);  servo5.write(90);
  delay(100);
  servo5.write(135); servo3.write(0);   servo4.write(130);
  delay(400);
  servo1.write(90);  servo2.write(90);
  servo3.write(90);  servo4.write(90);  servo5.write(90);
  delay(100);
  hookInProgress = false;
}

BLYNK_WRITE(V0)  { blynk_j1_x  = param.asInt(); }
BLYNK_WRITE(V1)  { blynk_j1_y  = param.asInt(); }
BLYNK_WRITE(V2)  { blynk_j2_x  = param.asInt(); }
BLYNK_WRITE(V3)  { blynk_j2_y  = param.asInt(); }
BLYNK_WRITE(V4)  { blynk_fwd   = param.asInt(); }
BLYNK_WRITE(V5)  { blynk_bwd   = param.asInt(); }
BLYNK_WRITE(V6)  { blynk_left  = param.asInt(); }
BLYNK_WRITE(V7)  { blynk_right = param.asInt(); }
BLYNK_WRITE(V8)  { if (param.asInt()) performL1Action(); }
BLYNK_WRITE(V9)  { if (param.asInt()) performR1Action(); }

void controlMotor(int forwardPin, int backwardPin, int speed) {
  if (speed > 0) {
    analogWrite(forwardPin,  speed);
    analogWrite(backwardPin, 0);
  } else if (speed < 0) {
    analogWrite(forwardPin,  0);
    analogWrite(backwardPin, -speed);
  } else {
    analogWrite(forwardPin,  0);
    analogWrite(backwardPin, 0);
  }
}

void stopAllMotorsAndServos() {
  controlMotor(motor1_forward_pin, motor1_backward_pin, 0);
  controlMotor(motor2_forward_pin, motor2_backward_pin, 0);
  digitalWrite(motor1_enable_pin, LOW);
  digitalWrite(motor2_enable_pin, LOW);
  servo1.write(90); servo2.write(90); servo3.write(90);
  servo4.write(90); servo5.write(90);
}

void applyMovement(bool fwd, bool bwd, bool left, bool right) {
  if (fwd) {
    digitalWrite(motor1_enable_pin, HIGH);
    digitalWrite(motor2_enable_pin, HIGH);
    controlMotor(motor1_forward_pin, motor1_backward_pin,  motor_range_max);
    controlMotor(motor2_forward_pin, motor2_backward_pin,  motor_range_max);
  } else if (bwd) {
    digitalWrite(motor1_enable_pin, HIGH);
    digitalWrite(motor2_enable_pin, HIGH);
    controlMotor(motor1_forward_pin, motor1_backward_pin,  motor_range_min);
    controlMotor(motor2_forward_pin, motor2_backward_pin,  motor_range_min);
  } else if (left) {
    digitalWrite(motor1_enable_pin, HIGH);
    digitalWrite(motor2_enable_pin, HIGH);
    controlMotor(motor1_forward_pin, motor1_backward_pin,  motor_range_min);
    controlMotor(motor2_forward_pin, motor2_backward_pin,  motor_range_max);
  } else if (right) {
    digitalWrite(motor1_enable_pin, HIGH);
    digitalWrite(motor2_enable_pin, HIGH);
    controlMotor(motor1_forward_pin, motor1_backward_pin,  motor_range_max);
    controlMotor(motor2_forward_pin, motor2_backward_pin,  motor_range_min);
  } else {
    digitalWrite(motor1_enable_pin, LOW);
    digitalWrite(motor2_enable_pin, LOW);
    controlMotor(motor1_forward_pin, motor1_backward_pin,  0);
    controlMotor(motor2_forward_pin, motor2_backward_pin,  0);
  }
}

void onConnectedController(ControllerPtr ctl) {
  if (blynkLocked) return;  // Reject all BT controller connections
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == nullptr) {
      myControllers[i] = ctl;
      controllerConnected = true;
      break;
    }
  }
  for (int i = 0; i < 2; i++) {
    digitalWrite(buzzer, HIGH); delay(500);
    digitalWrite(buzzer, LOW);  delay(500);
  }
}

void onDisconnectedController(ControllerPtr ctl) {
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == ctl) {
      myControllers[i] = nullptr;
      controllerConnected = false;
      stopAllMotorsAndServos();
      break;
    }
  }
  digitalWrite(buzzer, HIGH); delay(500);
  digitalWrite(buzzer, LOW);
}

void setup() {
  Serial.begin(115200);

  pinMode(motor1_forward_pin,  OUTPUT);
  pinMode(motor1_backward_pin, OUTPUT);
  pinMode(motor2_forward_pin,  OUTPUT);
  pinMode(motor2_backward_pin, OUTPUT);
  pinMode(motor1_enable_pin,   OUTPUT);
  pinMode(motor2_enable_pin,   OUTPUT);
  pinMode(buzzer,              OUTPUT);

  servo1.attach(servo1_pin);
  servo2.attach(servo2_pin);
  servo3.attach(servo3_pin);
  servo4.attach(servo4_pin);
  servo5.attach(servo5_pin);

  stopAllMotorsAndServos();

  BP32.setup(&onConnectedController, &onDisconnectedController);
  BP32.forgetBluetoothKeys();
  BP32.enableVirtualDevice(false);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); }

  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect();
}

void loop() {
  Blynk.run();

  BP32.update();  // Still call to keep BT stack alive, but data is never used

  if (!hookInProgress) {
    applyMovement(blynk_fwd, blynk_bwd, blynk_left, blynk_right);
    servo1.write(map(blynk_j1_y, 0,   255, 0, 180));
    servo2.write(map(blynk_j1_x, 255,   0, 0, 180));
    servo3.write(map(blynk_j2_y, 255,   0, 0, 180));
    servo4.write(map(blynk_j2_x, 255,   0, 0, 180));
  }

  delay(2);
}  if (targetPos == currentPos) return;
  if (currentPos < targetPos) {
    for (int p = currentPos; p <= targetPos; p++) {
      servo.write(p);
      delay(8);
    }
  } else {
    for (int p = currentPos; p >= targetPos; p--) {
      servo.write(p);
      delay(8);
    }
  }
  currentPos = targetPos;
}

// Arm sliders
// Left shoulder (normal)
BLYNK_WRITE(V7) { servo1.write(param.asInt()); }           // Vertical
BLYNK_WRITE(V6) { servo2.write(180 - param.asInt()); }     // Horizontal inverted for inward/outward

// Right shoulder (inverted)
BLYNK_WRITE(V5) { servo3.write(180 - param.asInt()); }     // Vertical inverted
BLYNK_WRITE(V4) { servo4.write(180 - param.asInt()); }     // Horizontal inverted for inward/outward

// Hip slider (V12)
BLYNK_WRITE(V12) {
  int targetPos = param.asInt();
  Serial.print("HIP -> ");
  if (targetPos == 0) {
    Serial.println("LEFT (0°)");
    smoothMove(hipServo, leftPos);
  } else if (targetPos == 180) {
    Serial.println("RIGHT (120°)");
    smoothMove(hipServo, rightPos);
  } else {
    Serial.println("CENTER (90°)");
    smoothMove(hipServo, centerPos);
  }
}

// Motor Setup
const int motorForwardPins[4]  = {12, 27, 16, 21};
const int motorBackwardPins[4] = {14, 26, 17, 23};
const int motorEnablePins[4]   = {13, 25, 33, 32};

const int MOTOR_RANGE_MAX = 200;
const int MOTOR_RANGE_MIN = -200;

const int MOTOR_PWM_FREQ = 20000;
const int MOTOR_PWM_RES  = 8;  // 0–255 range

volatile uint8_t motorSpeed = 200;

int logicalToPwm(int val) {
  return map(val, MOTOR_RANGE_MIN, MOTOR_RANGE_MAX, -255, 255);
}

void controlMotorByIndex(int idx, int pwmSigned) {
  int fwd = motorForwardPins[idx];
  int back = motorBackwardPins[idx];
  int en  = motorEnablePins[idx];
  if (pwmSigned > 0) {
    digitalWrite(fwd, HIGH);
    digitalWrite(back, LOW);
    ledcWrite(en, constrain(pwmSigned, 0, 255));
  } else if (pwmSigned < 0) {
    digitalWrite(fwd, LOW);
    digitalWrite(back, HIGH);
    ledcWrite(en, constrain(-pwmSigned, 0, 255));
  } else {
    digitalWrite(fwd, LOW);
    digitalWrite(back, LOW);
    ledcWrite(en, 0);
  }
}

void stopAllMotors() {
  for (int i = 0; i < 4; i++) {
    controlMotorByIndex(i, 0);
    digitalWrite(motorEnablePins[i], LOW);
  }
}

// Motor buttons
BLYNK_WRITE(V0) {  // Forward
  if (param.asInt()) {
    for (int i = 0; i < 4; i++) digitalWrite(motorEnablePins[i], HIGH);
    for (int i = 0; i < 4; i++) controlMotorByIndex(i, logicalToPwm(MOTOR_RANGE_MAX));
  } else {
    stopAllMotors();
  }
}

BLYNK_WRITE(V3) {  // Backward
  if (param.asInt()) {
    for (int i = 0; i < 4; i++) digitalWrite(motorEnablePins[i], HIGH);
    for (int i = 0; i < 4; i++) controlMotorByIndex(i, logicalToPwm(MOTOR_RANGE_MIN));
  } else {
    stopAllMotors();
  }
}

BLYNK_WRITE(V1) {  // Turn Right
  if (param.asInt()) {
    for (int i = 0; i < 4; i++) digitalWrite(motorEnablePins[i], HIGH);
    controlMotorByIndex(0, logicalToPwm(MOTOR_RANGE_MAX));
    controlMotorByIndex(2, logicalToPwm(MOTOR_RANGE_MAX));
    controlMotorByIndex(1, logicalToPwm(MOTOR_RANGE_MIN));
    controlMotorByIndex(3, logicalToPwm(MOTOR_RANGE_MIN));
  } else {
    stopAllMotors();
  }
}

BLYNK_WRITE(V2) {  // Turn Left
  if (param.asInt()) {
    for (int i = 0; i < 4; i++) digitalWrite(motorEnablePins[i], HIGH);
    controlMotorByIndex(0, logicalToPwm(MOTOR_RANGE_MIN));
    controlMotorByIndex(2, logicalToPwm(MOTOR_RANGE_MIN));
    controlMotorByIndex(1, logicalToPwm(MOTOR_RANGE_MAX));
    controlMotorByIndex(3, logicalToPwm(MOTOR_RANGE_MAX));
  } else {
    stopAllMotors();
  }
}

// ====== Setup ======
void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);

  // Servo attach
  servo1.attach(servo1Pin);
  servo2.attach(servo2Pin);
  servo3.attach(servo3Pin);
  servo4.attach(servo4Pin);
  hipServo.attach(hipPin);

  servo1.write(90);
  servo2.write(90);
  servo3.write(90);
  servo4.write(90);
  hipServo.write(centerPos);
  currentPos = centerPos;

  // Motor pin setup + PWM
  for (int i = 0; i < 4; i++) {
    pinMode(motorForwardPins[i], OUTPUT);
    pinMode(motorBackwardPins[i], OUTPUT);
    pinMode(motorEnablePins[i], OUTPUT);
    digitalWrite(motorForwardPins[i], LOW);
    digitalWrite(motorBackwardPins[i], LOW);
    digitalWrite(motorEnablePins[i], LOW);

    ledcAttach(motorEnablePins[i], MOTOR_PWM_FREQ, MOTOR_PWM_RES);
    ledcWrite(motorEnablePins[i], 0);
  }

  Serial.println("All servos and motors ready!");
}

// ====== Loop ======
void loop() {
  Blynk.run();
}
