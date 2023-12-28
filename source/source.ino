/*
#Circuit Connection
connect buzzer pins to GND and D6
connect ir-sensor-1 pins to GND, 5v and A2
connect ir-sensor-2 pins to GND, 5v and A3
connect servo pins to GND, 5v and D8
Connect by following for RFID pins: 
SDA to D10
SCK to D13
MOSI to D11
MISO to D12
GND to GND
RST to D9
3.3V to 3.3V
*/

#include <SPI.h>
#include <Servo.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#define sensorPin1 A2
#define sensorPin2 A3
#define buzzerPin 6
#define servoPin 8
#define RST_PIN 9
#define SS_PIN 10
int senVal1 = 0;
int senVal2 = 0;
int state = 0;
String tollPaidIDs[] = { "B0 54 95 32", "A9 9C 09 BA", "92 CA BA 1D", "7C D0 8F 81" };
Servo servo;
MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);
  servo.attach(servoPin);
  servo.write(180);
  pinMode(buzzerPin, OUTPUT);
  pinMode(sensorPin1, INPUT);
  pinMode(sensorPin2, INPUT);
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Automatic toll Collection System");
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("    Welcome    ");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   Automatic   ");
  lcd.setCursor(0, 1);
  lcd.print("Toll Collection");
  delay(1000);
}
void loop() {
  senVal1 = digitalRead(sensorPin1);
  senVal2 = digitalRead(sensorPin2);
  readRfid();
  if (senVal1 == 0) {
    VehicleDetected();
  } else if (senVal2 == 0 && state == 1) {
    accessGranted();
  } else if (senVal2 == 0 && state == 0) {
    accessDenied();
  }
}

void servoDown() {
  servo.attach(servoPin);
  servo.write(90);
}
void servoUp() {
  servo.attach(servoPin);
  servo.write(180);
}
void VehicleDetected() {
  servoDown();
  Serial.println("Vehicle Detected. Swipe your card..");
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Vehicle Detected");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Swipe Your Card");
  delay(1000);
}
void accessGranted() {
  servoUp();
  Serial.println("Have a safe Journey *_*");
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("  Safe Journery  ");
  delay(2000);
  lcd.clear();
  state = 0;
}
void accessDenied() {
  servoDown();
  Serial.println("Access denied");
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print(" Access denied ");
  digitalWrite(buzzerPin, HIGH);
  delay(1000);
  digitalWrite(buzzerPin, LOW);
}
void readRfid() {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) return;
  String content = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    content += String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ") + String(mfrc522.uid.uidByte[i], HEX);
  }
  content.toUpperCase();

  int numIDs = sizeof(tollPaidIDs) / sizeof(tollPaidIDs[0]);
  for (int i = 0; i < numIDs; i++) {
    if (content.substring(1) == tollPaidIDs[i]) {
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("   Toll Paid   ");
      state = 1;
      return;
    }
  }
  accessDenied();
}
