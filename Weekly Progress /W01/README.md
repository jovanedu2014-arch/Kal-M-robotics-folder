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
#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "YOUR_TEMPLATE_NAME"
#define BLYNK_AUTH_TOKEN "YOUR_AUTH_TOKEN"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>

char ssid[] = "YOUR_WIFI_NAME";
char pass[] = "YOUR_WIFI_PASSWORD";
char auth[] = BLYNK_AUTH_TOKEN;

// ====== Servo Setup ======
Servo servo1;  // Left shoulder vertical
Servo servo2;  // Left shoulder horizontal
Servo servo3;  // Right shoulder vertical
Servo servo4;  // Right shoulder horizontal
Servo hipServo;

// Pins
int servo1Pin = 2;   // V7
int servo2Pin = 4;   // V6
int servo3Pin = 5;   // V5
int servo4Pin = 18;  // V4
int hipPin    = 19;  // hip (V10/V11 control)

// Hip angles
int centerPos  = 90;
int leftPos    = 0;
int rightPos   = 180;
int currentPos = centerPos;

// Smooth motion routine for hip
void smoothMove(Servo &servo, int targetPos) {
  if (targetPos == currentPos) return;
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

// ====== Arm sliders ======
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

// ====== Motor Setup ======
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
