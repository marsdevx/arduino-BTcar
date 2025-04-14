#include <SoftwareSerial.h>

#define IN1 4
#define IN2 2
#define ENA 3

#define IN3 8
#define IN4 7
#define ENB 5

#define BT_RX 12
#define BT_TX 13
SoftwareSerial BTSerial(BT_RX, BT_TX);

bool startParsing = false;
bool doneParsing = false;
String stringBuffer = "";
int dataX = 0;
int dataY = 0;

void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  BTSerial.begin(9600);
}

void loop() {
  parseJoystickData();

  if (doneParsing) {
    doneParsing = false;

    int joystickX = map(dataX, -40, 40, -127, 127);
    int joystickY = map(dataY, -40, 40, -255, 255);

    int dutyR = constrain(joystickY - joystickX, -255, 255);
    int dutyL = constrain(joystickY + joystickX, -255, 255);

    moveMotors(dutyL, dutyR);
  }
}

void moveMotors(int leftSpeed, int rightSpeed) {
  // Left motor
  if (leftSpeed > 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else if (leftSpeed < 0) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    leftSpeed = -leftSpeed;
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
  }
  analogWrite(ENA, constrain(leftSpeed, 0, 255));

  // Right motor
  if (rightSpeed > 0) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else if (rightSpeed < 0) {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    rightSpeed = -rightSpeed;
  } else {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
  }
  analogWrite(ENB, constrain(rightSpeed, 0, 255));
}

void parseJoystickData() {
  while (BTSerial.available()) {
    char c = BTSerial.read();

    if (startParsing) {
      if (c == ' ') {
        dataX = stringBuffer.toInt();
        stringBuffer = "";
      } else if (c == ';') {
        dataY = stringBuffer.toInt();
        stringBuffer = "";
        doneParsing = true;
        startParsing = false;
      } else {
        stringBuffer += c;
      }
    }

    if (c == '$') {
      startParsing = true;
      stringBuffer = "";
    }
  }
}