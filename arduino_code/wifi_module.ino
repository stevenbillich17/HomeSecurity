#include <Keypad.h>
#include <Adafruit_LiquidCrystal.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>

// todo: must add your own credentials here
char wifiSSID[] = "";       //  your network SSID (name) between the " "
char wifiPassword[] = "";     // your network password between the " "
int connectionStatus = WL_IDLE_STATUS;  //connection status
WiFiServer wifiServer(80);              //server socket

WiFiClient wifiClient = wifiServer.available();

const short alarmLedPin = 2;

const byte keypadRows = 4;
const byte keypadCols = 3;

char keypadKeys[keypadRows][keypadCols] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};
char inputKey;

String inputPassword;

String keySequence = "";
int PASSWORD_LENGTH = 4;
String validPassword = String("4567");

String inputHistory[25];
int historyCount = 0;

int alarmState = 0;

byte rowPinArray[keypadRows] = { 9, 8, 7, 6 };
byte colPinArray[keypadCols] = { 5, 4, 3 };

Keypad userKeypad = Keypad(makeKeymap(keypadKeys), rowPinArray, colPinArray, keypadRows, keypadCols);

bool roomOneStatus = false;
bool roomTwoStatus = false;
Adafruit_LiquidCrystal lcd_1(0);

bool shouldAsk = true;
bool shouldAsk2 = false;
char temperature;
const int shift = 3;
int x = 0;

void setup() {
  Serial.begin(9600);

  pinMode(alarmLedPin, OUTPUT);

  pinMode(10, INPUT);
  Wire.begin();
  attachInterrupt(digitalPinToInterrupt(10), personDetected, RISING);
  attachInterrupt(digitalPinToInterrupt(11), personDetected2, RISING);

  lcd_1.begin(16, 2);
  lcd_1.clear();

  checkWiFiModule();
  connectToWiFi();

  printWifiStatus();
  wifiServer.begin();
}

void loop() {
  wifiClient = wifiServer.available();

  captureKeypadData();

  if (wifiClient) {
    handleClientRequests();
  }

  if (shouldAsk == true) {
    Wire.requestFrom(8, 2);
    char status = 0;
    while (!Wire.available()) {
      Serial.println("in while");
    }

    status = Wire.read();

    temperature = Wire.read();

    if (temperature == 5) {
      if (temperature < 0)
        temperature *= -1;
    }
    if (status == 1) {
      Serial.println(1);
      roomOneStatus = true;
    }
    if (status == 2) {
      roomTwoStatus = true;
    }
    if (status == 3) {
      roomOneStatus = true;
      roomTwoStatus = true;
    }
    if (status != 0) {
    }
    // shouldAsk = false;
  }

  if (shouldAsk2 == true) {
    Wire.requestFrom(9, 1);
    char status = 0;
    while (!Wire.available()) {
      Serial.println("in while");
    }

    status = Wire.read();
    shouldAsk2 = false;
    alarmState = false;
    roomOneStatus = false;
    roomTwoStatus = false;
    turnOffAlarm();
  }
}

void printWifiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI): ");
  Serial.print(rssi);
  Serial.println(" dBm");

  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

void checkWiFiModule() {
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true)
      ;
  }

  if (WiFi.firmwareVersion() < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }
}

void connectToWiFi() {
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(wifiSSID);

  while (connectionStatus != WL_CONNECTED) {
    connectionStatus = WiFi.begin(wifiSSID, wifiPassword);
    delay(10000);
  }
}

void handleClientRequests() {
  if (wifiClient) {
    Serial.println("new client");
    String currentLine = "";
    while (wifiClient.connected()) {
      if (wifiClient.available()) {
        char c = wifiClient.read();
        Serial.write(c);
        if (c == '\n') {
          if (currentLine.length() == 0) {
            wifiClient.println("HTTP/1.1 200 OK");
            wifiClient.println("Content-type:text/html");
            wifiClient.println();

            // Generate the response in one print() call
            wifiClient.print("Click <a href=\"/H\">here</a> turn the LED on<br>"
                             "Click <a href=\"/L\">here</a> turn the LED off<br><br>"
                             "Random reading from analog pin: ");
            wifiClient.print(analogRead(A1));

            wifiClient.print("room1status:");
            wifiClient.println(roomOneStatus == true ? "1" : "0");
            wifiClient.print("room2status:");
            wifiClient.println(roomTwoStatus == true ? "1" : "0");
            wifiClient.print("temperature:");
            wifiClient.println((int)temperature);
            wifiClient.print("ENDTEMPERATURE");
            wifiClient.print("<br><br>GETALARM:");

            wifiClient.println(alarmState);

            wifiClient.print("Button pressed: ");
            wifiClient.println(inputKey);

            if (keySequence.length() != 0) {
              wifiClient.print("Keypad Input: ");
              wifiClient.print(keySequence);
              wifiClient.print("<br><br>");
            }

            wifiClient.println("STARTHISTORY<br>");
            for (int index = 0; index < historyCount; index++) {
              wifiClient.print(inputHistory[index]);
              wifiClient.println("<br>");
            }
            wifiClient.println("ENDHISTORY");

            wifiClient.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }

        if (currentLine.endsWith("GET /switch-alarm")) {
          String dateTime = fetchCurrentDateTime();
          delay(200);
          if (alarmState == 0) {
            inputHistory[historyCount++] = "(app) alarm turned on at " + dateTime;
            alarmState = 1;
            turnOnAlarm();
            digitalWrite(alarmLedPin, HIGH);
            delay(1000);
            digitalWrite(alarmLedPin, LOW);
          } else {
            inputHistory[historyCount++] = "(app) alarm turned off at " + dateTime;
            alarmState = 0;
            turnOffAlarm();
            digitalWrite(alarmLedPin, HIGH);
            delay(1000);
            digitalWrite(alarmLedPin, LOW);
          }
        }

        if (currentLine.lastIndexOf("/change-password-") != -1) {
          if (currentLine.substring(currentLine.lastIndexOf("-") + 1).length() == 4)
            validPassword = encrypt(currentLine.substring(currentLine.lastIndexOf("-") + 1));
          Serial.println("valid password: " + validPassword);
        }

        if (currentLine.endsWith("GET /L")) {
          digitalWrite(alarmLedPin, LOW);
        }
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(alarmLedPin, HIGH);
        }
      }
    }
    wifiClient.stop();
    Serial.println("client disconnected");
  }
}

// assume you have a WiFiClient named client somewhere else in your code
// and you're connected to WiFi

void captureKeypadData() {
  inputKey = userKeypad.getKey();
  if (inputKey) {
    keySequence += inputKey;
    inputPassword += inputKey;

    lcd_1.setCursor(0, 0);
    lcd_1.print("Enter Password:");
    lcd_1.setCursor(0, 1);
    lcd_1.print(inputPassword);
    digitalWrite(alarmLedPin, HIGH);
    delay(200);
    digitalWrite(alarmLedPin, LOW);
    delay(200);
    if (inputPassword.length() == PASSWORD_LENGTH) {
      Serial.println(inputPassword);
      if (encrypt(inputPassword).equals(validPassword) != 0) {
        Serial.println("correct password");
        if (alarmState == 0) {
          inputHistory[historyCount++] = "correct password: alarm turned on at " + fetchCurrentDateTime();
          alarmState = 1;
          lcd_1.clear();
          lcd_1.setCursor(0, 0);
          lcd_1.print("Alarm turned on");

          turnOnAlarm();

          digitalWrite(alarmLedPin, HIGH);
          delay(1000);
          digitalWrite(alarmLedPin, LOW);
          lcd_1.clear();
        } else {
          inputHistory[historyCount++] = "correct password: alarm turned off at " + fetchCurrentDateTime();
          alarmState = 0;
          lcd_1.clear();
          lcd_1.setCursor(0, 0);
          lcd_1.print("Alarm turned off");
          delay(1000);
          lcd_1.clear();

          turnOffAlarm();
        }
      } else {
        Serial.println("incorrect password");
        inputHistory[historyCount++] = inputPassword + " - incorrect password at " + fetchCurrentDateTime();
        lcd_1.clear();
        lcd_1.setCursor(0, 0);
        lcd_1.print("The password is");
        lcd_1.setCursor(0, 1);
        lcd_1.print("incorrect");
        for (int i = 0; i < 3; i++) {
          digitalWrite(alarmLedPin, HIGH);
          delay(200);
          digitalWrite(alarmLedPin, LOW);
          delay(200);
        }
      }
      lcd_1.clear();
      inputPassword = "";
    }
  }
}

/* 
This function retrieves the current date and time from worldtimeapi.org.
It sends an HTTP GET request to the server to get the data in the timezone of Europe/London.
The received data is in JSON format, which is parsed to extract the datetime information.
*/
String fetchCurrentDateTime() {
  // Display status
  Serial.println("Requesting date and time...");

  // Connect to the server
  if (!wifiClient.connect("worldtimeapi.org", 80)) {
    Serial.println("Failed to connect to server");
    return String("");  // If connection failed, return an empty string
  }

  // Send an HTTP GET request for the required timezone
  wifiClient.println("GET /api/timezone/Europe/London HTTP/1.1");
  wifiClient.println("Host: worldtimeapi.org");
  wifiClient.println("Connection: close");
  wifiClient.println();

  // Wait for the server to respond
  while (wifiClient.connected() && !wifiClient.available()) delay(1);

  // Parse server's response
  String datetime = "";
  while (wifiClient.connected() || wifiClient.available()) {
    String line = wifiClient.readStringUntil('\n');
    if (line == "\r") {  // Headers end here, time to parse the JSON
      DynamicJsonDocument jsonDoc(1024);
      deserializeJson(jsonDoc, wifiClient);
      datetime = jsonDoc["datetime"].as<String>();
      Serial.println(datetime);
      break;
    }
  }

  wifiClient.stop();  // Close the connection

  return datetime;
}

void personDetected() {
  shouldAsk = true;
}

void personDetected2() {
  shouldAsk2 = true;
}

void turnOnAlarm() {
  Serial.println("WIFI Sending arming request...");
  Wire.beginTransmission(8);
  char y = 1;
  Wire.write(y);
  Wire.endTransmission(8);
  Serial.println("WIFI Sent arming request");
}

void turnOffAlarm() {
  Serial.println("WIFI Sending arming request...");
  Wire.beginTransmission(8);
  char y = 2;
  Wire.write(y);
  Wire.endTransmission(8);
  Serial.println("WIFI Sent arming request");
  roomOneStatus = false;
  roomTwoStatus = false;
}

String encrypt(String str) {
  int len = str.length();

  for (int i = 0; i < len; i++) {
    str[i] += shift;
  }
  return str;
}

String decrypt(String str) {
  int len = str.length();

  for (int i = 0; i < len; i++) {
    str[i] -= shift;
  }

  return str;
}
