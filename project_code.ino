#include <Servo.h>

// ==================== Servos ====================
Servo hip1, knee1;
Servo hip2, knee2;
Servo hip3, knee3;
Servo hip4, knee4;

// ==================== Pins ====================
#define HIP1_PIN  4
#define KNEE1_PIN 5
#define HIP2_PIN  6
#define KNEE2_PIN 7
#define HIP3_PIN  8
#define KNEE3_PIN 9
#define HIP4_PIN  10
#define KNEE4_PIN 11

// ==================== Ultrasonic ====================
#define FRONT_TRIG  12
#define FRONT_ECHO  13
const int OBSTACLE_DIST = 20;

// ==================== LEG CALIBRATION ====================
const int L1_HIP_FORWARD = 0;
const int L1_HIP_BACK    = 120;
const int L1_HIP_NEUTRAL = 60;

const int L2_HIP_FORWARD = 0;
const int L2_HIP_BACK    = 120;
const int L2_HIP_NEUTRAL = 60;

const int L3_HIP_FORWARD = 120;
const int L3_HIP_BACK    = 60;
const int L3_HIP_NEUTRAL = 60;

const int L4_HIP_FORWARD = 120;
const int L4_HIP_BACK    = 60;
const int L4_HIP_NEUTRAL = 60;

// Knees
const int L1_KNEE_DOWN = 0;
const int L2_KNEE_DOWN = 0;
const int L3_KNEE_DOWN = 180;
const int L4_KNEE_DOWN = 180;

const int L1_KNEE_UP = 30;
const int L2_KNEE_UP = 30;
const int L3_KNEE_UP = 150;
const int L4_KNEE_UP = 150;

// ==================== Smooth ====================
int speedDelay = 10;
int stepSize = 4;

void smoothMove(Servo &servo, int target) {
  int current = servo.read();

  if (current < target) {
    for (int pos = current; pos <= target; pos += stepSize) {
      servo.write(pos);
      delay(speedDelay);
    }
  } else {
    for (int pos = current; pos >= target; pos -= stepSize) {
      servo.write(pos);
      delay(speedDelay);
    }
  }
  servo.write(target);
}

// ==================== Stand ====================
void stand() {
  knee1.write(L1_KNEE_DOWN);
  knee2.write(L2_KNEE_DOWN);
  knee3.write(L3_KNEE_DOWN);
  knee4.write(L4_KNEE_DOWN);

  hip1.write(L1_HIP_NEUTRAL);
  hip2.write(L2_HIP_NEUTRAL);
  hip3.write(L3_HIP_NEUTRAL);
  hip4.write(L4_HIP_NEUTRAL);

  delay(400);
}

// ==================== Ultrasonic ====================
long getDistance() {
  digitalWrite(FRONT_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(FRONT_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(FRONT_TRIG, LOW);

  long duration = pulseIn(FRONT_ECHO, HIGH, 30000);
  if (duration == 0) return 999;

  return duration * 0.034 / 2;
}

// ==================== WALK ====================
bool walkForwardCycle() {

  // Pair 1
  smoothMove(knee1, L1_KNEE_UP);
  smoothMove(knee3, L3_KNEE_UP);

  smoothMove(hip1, L1_HIP_FORWARD);
  smoothMove(hip3, L3_HIP_FORWARD);

  smoothMove(knee1, L1_KNEE_DOWN);
  smoothMove(knee3, L3_KNEE_DOWN);

  delay(60);

  // Pair 2
  smoothMove(knee2, L2_KNEE_UP);
  smoothMove(knee4, L4_KNEE_UP);

  smoothMove(hip2, L2_HIP_FORWARD);
  smoothMove(hip4, L4_HIP_FORWARD);

  smoothMove(knee2, L2_KNEE_DOWN);
  smoothMove(knee4, L4_KNEE_DOWN);

  delay(60);

  // 🔥 FIXED PUSH (متوازن)
  smoothMove(hip1, L1_HIP_BACK);
  smoothMove(hip3, L3_HIP_BACK);

  smoothMove(hip2, L2_HIP_BACK);
  smoothMove(hip4, L4_HIP_BACK);

  delay(80);

  return true;
}

// ==================== TURN RIGHT ====================
void turnRightCycle() {

  smoothMove(knee1, L1_KNEE_UP);
  smoothMove(knee3, L3_KNEE_UP);

  smoothMove(hip1, L1_HIP_FORWARD);
  smoothMove(hip3, L3_HIP_BACK);

  smoothMove(knee1, L1_KNEE_DOWN);
  smoothMove(knee3, L3_KNEE_DOWN);

  delay(80);

  smoothMove(knee2, L2_KNEE_UP);
  smoothMove(knee4, L4_KNEE_UP);

  // ✅ FIXED TYPO HERE
  smoothMove(hip2, L2_HIP_BACK);
  smoothMove(hip4, L4_HIP_FORWARD);

  smoothMove(knee2, L2_KNEE_DOWN);
  smoothMove(knee4, L4_KNEE_DOWN);

  delay(80);
}

// ==================== OBSTACLE ====================
void avoidObstacle() {
  stand();
  delay(150);

  for (int i = 0; i < 2; i++) {
    turnRightCycle();
  }

  stand();
  delay(150);
}

// ==================== SETUP ====================
void setup() {
  Serial.begin(9600);

  hip1.attach(HIP1_PIN); knee1.attach(KNEE1_PIN);
  hip2.attach(HIP2_PIN); knee2.attach(KNEE2_PIN);
  hip3.attach(HIP3_PIN); knee3.attach(KNEE3_PIN);
  hip4.attach(HIP4_PIN); knee4.attach(KNEE4_PIN);
  pinMode(FRONT_TRIG, OUTPUT);
  pinMode(FRONT_ECHO, INPUT);

  stand();
  delay(800);

  Serial.println("Robot Ready");
}

// ==================== LOOP ====================
void loop() {

  long dist = getDistance();

  if (dist <= OBSTACLE_DIST) {
    avoidObstacle();
  } else {
    walkForwardCycle();
  }
}