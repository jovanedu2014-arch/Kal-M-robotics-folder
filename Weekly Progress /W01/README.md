# Project README

*This is a sample structure — feel free to customize it or add more sections based on your project.*

---

### 1. Objective
- The objective of this project is to control a robot using the blynk app on a mobile phone.

---

### 2. Components Used
To build this project, I used both the Blynk website and mobile app to set up the necessary datastreams and design a dashboard featuring buttons, switches, and sliders to control the robot's arms, shoulders, hips, and wheels. I then used the Arduino IDE to program the ESP32 microcontroller, integrating the unique Blynk credentials generated during setup.Another critical component of this project is Wi-Fi. Internet connectivity is required because the Blynk app does not communicate directly with the robot; instead, they send and receive commands through a cloud-based server. When a control is adjusted on the app, the signal travels over the internet to the Blynk Cloud, which instantly routes the instruction to the ESP32’s onboard Wi-Fi module.

---

### 3. Wiring / Circuit
- Add your wiring/circuit diagram.
- You can use **draw.io, hand drawing, circuit simulation, or any other suitable method**.
- If a wiring/circuit diagram is not applicable, explain briefly.

---

### 4. Working Principle
- Write down your understanding of how the project works.
- Explain the working principle in your own words.

---

### 5. Code
This is the code that I used for this project.

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

// ═════════════════════════════════════════════════════════════════════════════
//  HOOK SEQUENCES
// ═════════════════════════════════════════════════════════════════════════════

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

// ═════════════════════════════════════════════════════════════════════════════
//  BLYNK VIRTUAL PIN HANDLERS
// ═════════════════════════════════════════════════════════════════════════════

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

// ═════════════════════════════════════════════════════════════════════════════
//  MOTOR HELPERS
// ═════════════════════════════════════════════════════════════════════════════

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

// ═════════════════════════════════════════════════════════════════════════════
//  BLUEPAD32 CALLBACKS
// ═════════════════════════════════════════════════════════════════════════════

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

// ═════════════════════════════════════════════════════════════════════════════
//  SETUP
// ═════════════════════════════════════════════════════════════════════════════

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

// ═════════════════════════════════════════════════════════════════════════════
//  LOOP
// ═════════════════════════════════════════════════════════════════════════════

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
}
---

### 6. Result
- Share photographs / Recordings (As drive link) or screenshots of your completed project.
- Include the final output/result if possible.
  - Eg: https://drive.google.com/file/d/1t56oTDtfnhWL0OTrfkKYV429bhk_HaOQ/view?usp=sharing

---

### 7. Problems Encountered & Solutions
I faced the problem of trying to control the arm, shoulder, and hip motors. I over came this by changing the motor's PWM frequency and the motor's PWM response value. Another problem I faced was that sometimes the robot would disconnect, so I had to add a delay function so that it pauses the code, causes the ESP32 to miss its check-in with the server, and drops the connection.

---

### 8. Review Video Link
- Upload the project review/demo video to Google Drive.
- Share the **Google Drive link** here.
