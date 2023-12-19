#include <Wire.h>

// Module 1
const int PIR_SENSOR_1 = 2;
const int BUZZER_1 = 3;
const int LED_SENSOR_1 = 4;
// Module 2
const int PIR_SENSOR_2 = 5;
const int BUZZER_2 = 6;
const int LED_SENSOR_2 = 7;
// Active
bool sensor_1_active = false;
bool sensor_2_active = false;
// System armed variable
bool system_armed = false;
// Stepper motor
const int STEPPER_PIN_1 = 8;
const int STEPPER_PIN_2 = 9;
const int STEPPER_PIN_3 = 10;
const int STEPPER_PIN_4 = 11;
int step_number = 0;
// Interrupt wifi
const int INTERRUPT_WIFI = 12;
char checkInterruptSending = 0;
// Door locked
bool doorLocked = false;

void setup() {
  // set pins status
  pinMode(PIR_SENSOR_1, INPUT);
  pinMode(PIR_SENSOR_2, INPUT);
  pinMode(LED_SENSOR_1, OUTPUT);
  pinMode(LED_SENSOR_2, OUTPUT);
  pinMode(STEPPER_PIN_1, OUTPUT);
  pinMode(STEPPER_PIN_2, OUTPUT);
  pinMode(STEPPER_PIN_3, OUTPUT);
  pinMode(STEPPER_PIN_4, OUTPUT);
  pinMode(BUZZER_1, OUTPUT);
  pinMode(BUZZER_2, OUTPUT);

  // initial state of buzzer is off (obvious)
  digitalWrite(BUZZER_1, HIGH);
  digitalWrite(BUZZER_2, HIGH);

  // communication init
  digitalWrite(INTERRUPT_WIFI, LOW);
  pinMode(INTERRUPT_WIFI, OUTPUT);
  Wire.begin(8);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);


  // Debugging (for now)
  Serial.begin(9600);
}

/*
  Method used for locking/unlocking the room
  param: shouldLock (if true means that the door will lock
  and if it is false the door will unlock)
*/
void lockRoom(bool shouldLock) {
  for (int i = 0; i < 1000; i++) {
    OneStep(shouldLock);
    delay(3);
  }
}

void loop() {
  if (system_armed == true) {
    if (doorLocked == false) {
      lockRoom(true);
      doorLocked = true;
    }
    bool result_1 = checkSensor(PIR_SENSOR_1);
    bool result_2 = checkSensor(PIR_SENSOR_2);
    computeResults(result_1, result_2);
  } else {
    if (doorLocked == true) {
      lockRoom(false);
      doorLocked = false;
    }
  }
}

/** Function use to check if the given sensor is active
 Makes conversion from HIGH/LOW to true/false */
bool checkSensor(int sensor) {
  int result = digitalRead(sensor);
  if (result == HIGH) {
    return true;
  }
  return false;
}

/** Function that performs the logic for the sensors*/
void computeResults(bool res1, bool res2) {
  if (res1 == true) {
    sensor_1_active = true;
  }

  if (res2 == true) {
    sensor_2_active = true;
  }

  if (sensor_1_active == false && sensor_2_active == false) {
    // nobody was in the house
    return;
  } else if (sensor_1_active == true && sensor_2_active == false) {
    // detected person just in room 1
    if ((checkInterruptSending & 1) == 0) {
      callInterruptOnWIFI();
      checkInterruptSending = checkInterruptSending | 1;
    }
    turnOnAlarm_1(2);
    return;
  } else if (sensor_1_active == false && sensor_2_active == true) {
    // detected person just in room 2
    if (((checkInterruptSending >> 1) & 1) == 0) {
      callInterruptOnWIFI();
      checkInterruptSending = checkInterruptSending | 2;
    }
    turnOnAlarm_2(2);
    return;
  } else if (sensor_1_active == true && sensor_2_active == true) {
    // detected person just in both rooms
    if (((checkInterruptSending >> 2) & 1) == 0) {
      callInterruptOnWIFI();
      checkInterruptSending = checkInterruptSending | 4;
    }
    turnOnSpecialAlarm(3);
    return;
  }
}


// Turns on first alarm for a given number of times
// it also performs checks for the other sensor to catch
// a possible intruder
void turnOnAlarm_1(int numberOfTimes) {
  bool interReadS2 = checkSensor(PIR_SENSOR_2);
  digitalWrite(LED_SENSOR_1, HIGH);
  for (int i = 0; i < numberOfTimes; i++) {
    tone(BUZZER_1, 2000);
    delay(500);
    interReadS2 = checkSensor(PIR_SENSOR_2) ? true : interReadS2;
    delay(500);
    interReadS2 = checkSensor(PIR_SENSOR_2) ? true : interReadS2;

    tone(BUZZER_1, 1000);
    delay(500);
    interReadS2 = checkSensor(PIR_SENSOR_2) ? true : interReadS2;
    delay(500);
    interReadS2 = checkSensor(PIR_SENSOR_2) ? true : interReadS2;
  }
  digitalWrite(LED_SENSOR_1, LOW);
  noTone(BUZZER_1);
  digitalWrite(BUZZER_1, HIGH);
  if (interReadS2 == true) {
    sensor_2_active = true;
  }
}

// Turns on second alarm for a given number of times
// it also performs checks for the other sensor to catch
// a possible intruder
void turnOnAlarm_2(int numberOfTimes) {
  bool interReadS1 = checkSensor(PIR_SENSOR_1);
  digitalWrite(LED_SENSOR_2, HIGH);
  for (int i = 0; i < numberOfTimes; i++) {
    tone(BUZZER_2, 4000);
    delay(500);
    interReadS1 = checkSensor(PIR_SENSOR_1) ? true : interReadS1;
    delay(500);
    interReadS1 = checkSensor(PIR_SENSOR_1) ? true : interReadS1;

    tone(BUZZER_2, 3000);
    delay(500);
    interReadS1 = checkSensor(PIR_SENSOR_1) ? true : interReadS1;
    delay(500);
    interReadS1 = checkSensor(PIR_SENSOR_1) ? true : interReadS1;
  }
  digitalWrite(LED_SENSOR_2, LOW);
  noTone(BUZZER_2);
  digitalWrite(BUZZER_2, HIGH);
  if (interReadS1 == true) {
    sensor_1_active = true;
  }
}

// Special sound, person detected in both rooms
// no more intermediate checkings
void turnOnSpecialAlarm(int numberOfTimes) {
  Serial.println("Special alarm called!");
  for (int i = 0; i < numberOfTimes; i++) {
    specialPart(LED_SENSOR_1, BUZZER_1, 1500);
    specialPart(LED_SENSOR_2, BUZZER_2, 3000);
  }
  stopAllAlarmsSoft();
}

// Turns on a led and a buzzer with a specific frequency
// for a period of 1 second 
void specialPart(int led_sensor, int buzzer, int freq) {
  digitalWrite(led_sensor, HIGH);
  tone(buzzer, freq);
  delay(1000);
  noTone(buzzer);
  digitalWrite(buzzer, HIGH);
  digitalWrite(led_sensor, LOW);
}

/* This will not have long term effect, just for current
iteration of the main loop */
void stopAllAlarmsSoft() {
  digitalWrite(LED_SENSOR_1, LOW);
  digitalWrite(LED_SENSOR_2, LOW);
  noTone(BUZZER_1);
  noTone(BUZZER_2);
  digitalWrite(BUZZER_1, HIGH);
  digitalWrite(BUZZER_2, HIGH);
}

// Method that signals the arduino wifi that an alarming
// event occured
void callInterruptOnWIFI() {
  digitalWrite(INTERRUPT_WIFI, HIGH);
}

// Method that moves the stepper one single step in
// the given position
void OneStep(bool dir) {
  if (dir) {
    switch (step_number) {
      case 0:
        digitalWrite(STEPPER_PIN_1, HIGH);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, LOW);
        break;
      case 1:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, HIGH);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, LOW);
        break;
      case 2:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, HIGH);
        digitalWrite(STEPPER_PIN_4, LOW);
        break;
      case 3:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, HIGH);
        break;
    }
  } else {
    switch (step_number) {
      case 0:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, HIGH);
        break;
      case 1:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, HIGH);
        digitalWrite(STEPPER_PIN_4, LOW);
        break;
      case 2:
        digitalWrite(STEPPER_PIN_1, LOW);
        digitalWrite(STEPPER_PIN_2, HIGH);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, LOW);
        break;
      case 3:
        digitalWrite(STEPPER_PIN_1, HIGH);
        digitalWrite(STEPPER_PIN_2, LOW);
        digitalWrite(STEPPER_PIN_3, LOW);
        digitalWrite(STEPPER_PIN_4, LOW);
    }
  }
  step_number++;
  if (step_number > 3) {
    step_number = 0;
  }
}

// Event called to arm or disarm the alarm
void receiveEvent(int howMany) {
  byte x = Wire.read();
  if (x == 1) {
    system_armed = true;
    Serial.println("Received command, system armed");
  } else {
    system_armed = false;
    deactivateAlarm();
  }
}

// Fully deactivating the alarm system
void deactivateAlarm() {
  stopAllAlarmsSoft();
  sensor_1_active = false;
  sensor_2_active = false;
  checkInterruptSending = 0;

  digitalWrite(INTERRUPT_WIFI, false);
}

// Sends data about the event that occured when requested
void requestEvent() {
  char r = buildResponseCode();
  Wire.write(r);
  digitalWrite(INTERRUPT_WIFI, LOW);
}

// Method that encodes the status of alarm systems
char buildResponseCode() {
  char response = 0;
  if (sensor_1_active == true && sensor_2_active == false) {
    response = 1;
  } else if (sensor_1_active == false && sensor_2_active == true) {
    response = 2;
  } else if (sensor_1_active == true && sensor_2_active == true) {
    response = 3;
  }
  return response;
}

// Method use for testing/debugging
void scream(int numberOfTimes, int buzzerPin) {
  for (int i = 0; i < numberOfTimes; i++) {
    tone(buzzerPin, 2000);
    delay(1000);
    tone(buzzerPin, 1000);
    delay(1000);
  }
  noTone(buzzerPin);
}


