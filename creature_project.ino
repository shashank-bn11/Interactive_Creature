#include <Wire.h> 
#include <Adafruit_GFX.h> 
#include <Adafruit_SSD1306.h> 
#include <Servo.h> 
/* ---------- OLED ---------- */ 
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); 
/* ---------- PIN DEFINITIONS ---------- */ 
#define TRIG_PIN    8 
#define ECHO_PIN    7 
#define LED_PIN     4 
#define BUZZER_PIN  5 
#define TOUCH_PIN   2 
#define SERVO_PIN   9 
#define LDR_PIN     A0 
/* ---------- LDR THRESHOLDS (HYSTERESIS) ---------- */ 
#define DARK_THRESHOLD   100 
#define LIGHT_THRESHOLD  450 
/* ---------- OBJECTS ---------- */ 
Servo myServo; 
/* ---------- VARIABLES ---------- */ 
long duration; 
int distance; 
bool waved = false; 
bool isSleeping = false; 
unsigned long prevLEDToggle = 0; 
unsigned long prevOLEDUpdate = 0; 
const unsigned long ledInterval  = 200;  // LED + buzzer blink speed 
const unsigned long oledInterval = 300;  // OLED refresh rate 
bool ledState = false; 
void setup() { 
pinMode(TRIG_PIN, OUTPUT); 
pinMode(ECHO_PIN, INPUT); 
pinMode(LED_PIN, OUTPUT); 
pinMode(BUZZER_PIN, OUTPUT); 
pinMode(TOUCH_PIN, INPUT); 
myServo.attach(SERVO_PIN); 
myServo.write(90);  // Center position 
Serial.begin(9600); 
if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
while (1); // OLED not found 
} 
display.clearDisplay(); 
display.display(); 
} 
void loop() { 
unsigned long currentMillis = millis(); 
/* ---------- ULTRASONIC SENSOR ---------- */ 
digitalWrite(TRIG_PIN, LOW); 
delayMicroseconds(2); 
digitalWrite(TRIG_PIN, HIGH); 
delayMicroseconds(10); 
digitalWrite(TRIG_PIN, LOW); 
duration = pulseIn(ECHO_PIN, HIGH, 50000); 
distance = duration * 0.034 / 2; 
Serial.print("Distance: "); 
Serial.println(distance); 
/* ---------- LED + BUZZER (SCARED REACTION) ---------- */ 
if (distance > 0 && distance < 20) { 
if (currentMillis - prevLEDToggle >= ledInterval) { 
prevLEDToggle = currentMillis; 
ledState = !ledState; 
digitalWrite(LED_PIN, ledState); 
digitalWrite(BUZZER_PIN, ledState); 
} 
} else { 
digitalWrite(LED_PIN, LOW); 
digitalWrite(BUZZER_PIN, LOW); 
ledState = false; 
} 
  /* ---------- TOUCH SENSOR + SERVO ---------- */ 
  int touchState = digitalRead(TOUCH_PIN); 
 
  if (touchState == HIGH) { 
    myServo.write(30); 
    delay(300); 
    myServo.write(150); 
    delay(300); 
    myServo.write(90); 
    waved = true; 
  } 
 
  if (touchState == LOW && !waved) { 
    waved = false; 
  } 
 
  /* ---------- LDR + OLED (SLEEP / AWAKE) ---------- */ 
  if (currentMillis - prevOLEDUpdate >= oledInterval) { 
    prevOLEDUpdate = currentMillis; 
 
    int ldrValue = analogRead(LDR_PIN); 
    Serial.print("LDR: "); 
    Serial.println(ldrValue); 
 
    // Hysteresis logic 
    if (ldrValue < DARK_THRESHOLD) { 
      isSleeping = true; 
    } 
    else if (ldrValue > LIGHT_THRESHOLD) { 
      isSleeping = false; 
    } 
 
    display.clearDisplay(); 
    display.setTextColor(SSD1306_WHITE); 
 
    if (isSleeping) { 
      display.setTextSize(2); 
      display.setCursor(10, 20); 
      display.println("Sleeping"); 
      display.setTextSize(1); 
      display.setCursor(40, 45); 
      display.println("Zzz..."); 
    } else { 
      display.setTextSize(2); 
      display.setCursor(20, 20); 
      display.println("Awake!"); 
      display.setTextSize(1); 
      display.setCursor(15, 45); 
      display.println("Hello Human"); 
    } 
 
    display.display(); 
  } 
} 
 
 
