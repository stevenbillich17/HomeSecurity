#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#define SS_PIN 53
#define RST_PIN 49
#define GREEN_LED 2
#define RED_LED 3
#define BUZZER 8

// Interrupt wifi
const int INTERRUPT_WIFI = 12;
char checkInterruptSending = 0;

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Instance of the class

uint8_t byte_array[] = {0xF3, 0xBD, 0xD0, 0x04};
float reading;

void setup() {
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(A0, INPUT);
  digitalWrite(BUZZER, HIGH);
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("RFID");  // Init SPI bus // Init MFRC522 reading UID");
  // communication init
  digitalWrite(INTERRUPT_WIFI, LOW);
  pinMode(INTERRUPT_WIFI, OUTPUT);
  Wire.begin(9);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

}

void loop() {
  read_nfc();
}

void callInterruptOnWIFI() {
  digitalWrite(INTERRUPT_WIFI, HIGH);
}

// Sends data about the event that occured when requested
void requestEvent() {
  char r = '1';
  Wire.write(r);
  digitalWrite(INTERRUPT_WIFI, LOW);
}

// Event from wifi
void receiveEvent(int howMany) {
  byte x = Wire.read();
}

void read_nfc() {
  if (mfrc522.PICC_IsNewCardPresent()) {
    if (mfrc522.PICC_ReadCardSerial()) {
      Serial.print("Tag UID: ");

      // Compare UID bytes with byte_array
      bool match = true;
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? "0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);

        // Compare byte_array with UID byte
        if (mfrc522.uid.uidByte[i] != byte_array[i]) {
          match = false;
          //break;
        }
      }

      Serial.println();
      if (match) {
        Serial.println("UID matches byte_array!");
        good_password();
        callInterruptOnWIFI();
      } else {
        Serial.println("UID does not match byte_array.");
        bad_password();
      }

      mfrc522.PICC_HaltA();
    }
  }
}

void good_password() {
  tone(BUZZER, 4000);
  digitalWrite(GREEN_LED, HIGH);
  delay(500);
  noTone(BUZZER);
  digitalWrite(BUZZER, HIGH);
  delay(500);
  digitalWrite(GREEN_LED, LOW);
}

void bad_password() {
  tone(BUZZER, 1000);
  digitalWrite(RED_LED, HIGH);
  delay(250);
  digitalWrite(RED_LED, LOW);
  delay(250);
  digitalWrite(RED_LED, HIGH);
  delay(250);
  digitalWrite(RED_LED, LOW);
  delay(250);
  noTone(BUZZER);
  digitalWrite(BUZZER, HIGH);
}
