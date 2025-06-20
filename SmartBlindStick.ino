#include <LiquidCrystal.h>

// LCD Pins
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

// Sensors Pins
#define WET_SENSOR_PIN A0
#define IR_SENSOR_PIN 8
#define VIBRATION_SENSOR_PIN A1
#define TRIG_PIN 9
#define ECHO_PIN 10
#define GSM_TX 11 // GSM Module TX pin
#define GSM_RX 12 // GSM Module RX pin

// Thresholds
#define WATER_DETECTED_THRESHOLD 500
#define VIBRATION_THRESHOLD 300 // Adjust this based on sensor sensitivity
#define DISTANCE_THRESHOLD 15 // Distance threshold for SMS alert (in cm)

// Initialize GSM Module
#include <SoftwareSerial.h>
SoftwareSerial Serial1(GSM_RX, GSM_TX); // Software serial for GSM module

void setup() {
Serial.begin(9600);
Serial1.begin(9600); // Initialize GSM Module

lcd.begin(16, 2);
lcd.print("Initializing...");

// Set Pins
pinMode(WET_SENSOR_PIN, INPUT);
pinMode(VIBRATION_SENSOR_PIN, INPUT);
pinMode(IR_SENSOR_PIN, INPUT);
pinMode(TRIG_PIN, OUTPUT);
pinMode(ECHO_PIN, INPUT);

delay(2000);
lcd.clear();
}

void loop() {
lcd.setCursor(0, 0);

// Water Detection
int wetValue = analogRead(WET_SENSOR_PIN);
lcd.setCursor(0, 0);
if (wetValue < WATER_DETECTED_THRESHOLD) {
lcd.print("Water Detected ");
Serial.println("Water Detected");
} else {
lcd.print("No Water ");
//Serial.println("No Water");
}

delay(1000);

// Vibration Sensor for Fall Detection
int vibrationValue = analogRead(VIBRATION_SENSOR_PIN);
lcd.clear();
lcd.setCursor(0, 0);
if (vibrationValue > VIBRATION_THRESHOLD) {
lcd.print("Fall Detected ");
//Serial.println("Fall Detected");

// Send SMS for fall detection
sendSMS("+91832*******", "Alert! Fall detected at https://maps.app.goo.gl/3nsACvLFz9ePT5oJA?g_st=iw");
} else {
lcd.print("No Fall ");
// Serial.println("No Fall");
}

delay(1000);

// Staircase Detection
lcd.clear();
lcd.setCursor(0, 0);
if (digitalRead(IR_SENSOR_PIN) == LOW) {
lcd.print("Staircase Ahead");
Serial.println("Staircase Ahead");
} else {
lcd.print("Clear Path ");
// Serial.println("Clear Path");
}

// Distance Detection
lcd.setCursor(0, 1);
long distance = getDistance();
if (distance < DISTANCE_THRESHOLD) {
lcd.print("Obj: ");
lcd.print(distance);
lcd.print("cm ");
Serial.print("Object Detected at Distance: ");
Serial.print(distance);
Serial.println(" cm");

// Send SMS when distance < threshold
//sendSMS("+91832*******", "Alert! Object detected within 10cm distance.");
} else {
lcd.print("Dist OK ");
// Serial.println("Distance OK");
}

delay(1000);
}

long getDistance() {
digitalWrite(TRIG_PIN, LOW);
delayMicroseconds(2);
digitalWrite(TRIG_PIN, HIGH);
delayMicroseconds(10);
digitalWrite(TRIG_PIN, LOW);

long duration = pulseIn(ECHO_PIN, HIGH);
long distance = duration * 0.034 / 2;
return distance;
}

void sendSMS(String phoneNumber, String message) {
// Send AT Command to check GSM Module connectivity
Serial1.println("AT");
delay(1000);
if (Serial1.available()) {
String response = Serial1.readString();
if (response.indexOf("OK") == -1) {
Serial.println("GSM Module not responding");
return;
}
} else {
Serial.println("No response from GSM Module");
return;
}

// Set SMS to Text Mode
Serial1.println("AT+CMGF=1"); // Set the GSM Module to Text Mode
delay(1000);
if (Serial1.available()) {
String response = Serial1.readString();
if (response.indexOf("OK") == -1) {
Serial.println("Failed to set SMS text mode");
return;
}
}

// Specify the Recipient's Phone Number
Serial1.print("AT+CMGS=\"");
Serial1.print(phoneNumber); // Replace with the recipient's phone number
Serial1.println("\"");
delay(1000);

// Send the SMS Message
Serial1.print(message);
delay(1000);

// Send CTRL+Z (ASCII 26) to indicate the end of the message
Serial1.write(26);
delay(3000);

// Check for GSM Module response
if (Serial1.available()) {
String response = Serial1.readString();
if (response.indexOf("OK") != -1 || response.indexOf("+CMGS") != -1) {
Serial.println("SMS sent successfully");
} else {
Serial.println("Failed to send SMS");
}
} else {
Serial.println("No response after sending SMS");
}
}
