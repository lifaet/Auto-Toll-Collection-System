// connect buzzer pins to GND and D6
// connect ir-sensor-1 pins to GND, 5v and A2
// connect ir-sensor-2 pins to GND, 5v and A3
// connect servo pins to GND, 5v and D8
// Connect by following for RFID pins: 
// SDA to D10
// SCK to D13
// MOSI to D11
// MISO to D12
// GND to GND
// RST to D9
// 3.3V to 3.3V

#include <SPI.h>
#include <Servo.h>
#include <MFRC522.h>
Servo servo;
int servoPos = 0;
int senVal1 = 0;
int senVal2 = 0;
int state = 0;
#define buzzerPin 6
#define sensorPin1 A2
#define sensorPin2 A3
#define RST_PIN 9
#define SS_PIN 10
MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(9600);
  servo.attach(8);
  servo.write(180);
  pinMode(buzzerPin, OUTPUT);
  pinMode(sensorPin1, INPUT);
  pinMode(sensorPin2, INPUT);
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Automatic toll Collection System");
  delay(1000);
}

void loop() {
  sensorRead();
  rfid();
  if (senVal1 == 0) {
    servoDown();
    Serial.println("Vehicle Detected. Swipe you Card..");
    delay(1000);
  } else if (senVal2 == 0 && state == 1) {
    servoUp();
    Serial.println("Have a safe Journey");
    delay(1000);
    state = 0;
  } else if (senVal2 == 0 && state == 0) {
    accessDenied();
  }
}

void servoDown() {
  servo.attach(8);
  servo.write(90);
}

void servoUp() {
  servo.attach(8);
  servo.write(180);
}

void sensorRead() {
  senVal1 = digitalRead(sensorPin1);
  senVal2 = digitalRead(sensorPin2);
}

void rfid() {
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  String content = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  if (content.substring(1) == "B0 54 95 32") {
    state = 1;
  } else if (content.substring(1) == "A9 9C 09 BA") {
    state = 1;
  } else {
    accessDenied();
  }
}

void accessDenied() {
  servo.attach(8);
  servo.write(90);
  Serial.println("Access denied");
  digitalWrite(buzzerPin, HIGH);
  delay(1000);
  digitalWrite(buzzerPin, LOW);
}
