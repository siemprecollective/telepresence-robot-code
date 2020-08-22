#include <SoftwareSerial.h>
#include <SerialCommand.h>
#include <Servo.h>

SerialCommand SCmd;   // The demo SerialCommand object

//1 is the most counter-clockwise
//175 is the most clockwise

Servo s1; //base tilts forward (-) and backward (+)
Servo s2; //base twists left (-) and right (+)
Servo s3; //left shoulder up (-) and down (+)
Servo s4; //left shoulder forward (+) and backward (-)
Servo s5; //right shoulder up (+) and down (-)
Servo s6; //right shoulder forward (-) and backward (+)
Servo s7; //neck twist left (-) and right (+)
Servo s8; //head up (-) and down (+)
Servo s9; //left elbow up (-) and down (+)
Servo s10; //right elbow up (-) and down (+)

const int numServos = 10;

int globalServoMin = 1;
int globalServoMax = 174;
float smootheningRatio = 0.9;

//                                 1    2    3    4    5    6    7    8    9   10
int lower[numServos]         = {   0,  80, 106,  30,   1,  50,  20, 120,  10,   0};
int upper[numServos]         = {  30, 120, 174, 110,  70, 140, 130, 160, 170, 170};
int currentPos[numServos]    = {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0};
int targetPos[numServos]     = {  30, 100, 160,  30,  15, 140,  75, 120,  90,  90};
float smoothening[numServos] = { 0.9, 0.9, 0.9, 0.8, 0.9, 0.8, 0.9, 0.6, 0.3, 0.3};

void setup() {
  Serial.begin(9600);
  delay(500);

  initializeServos();

  // Setup callbacks for SerialCommand commands
  SCmd.addCommand("S", receiveServoCommand);
  SCmd.addCommand("D", receiveDirectServoCommand);
  //  SCmd.addCommand("Mode",switchMode);
  SCmd.addDefaultHandler(unrecognized);
  Serial.println("Ready");
}

void loop()
{
  SCmd.readSerial();
  updateServoPositions();
}

void receiveServoCommand()
{
  int servoNumber;
  int servoCommand;
  char *arg;
  bool valid = true;

  arg = SCmd.next();
  if (arg != NULL) servoNumber = atoi(arg);  // Converts a char string to an integer
  else valid = false;

  arg = SCmd.next();
  if (arg != NULL) servoCommand = atol(arg);
  else valid = false;

  if (servoCommand < 0) valid = false;
  if (servoCommand > 180) valid = false;

  Serial.print("S ");
  Serial.print(servoNumber);
  Serial.print(" ");
  Serial.println(servoCommand);
  if (valid) {
    setTargetServoPosition(servoNumber, servoCommand);
  }
}

void receiveDirectServoCommand()
{
  int servoNumber;
  int servoCommand;
  char *arg;
  bool valid = true;

  arg = SCmd.next();
  if (arg != NULL) servoNumber = atoi(arg);  // Converts a char string to an integer
  else valid = false;

  arg = SCmd.next();
  if (arg != NULL) servoCommand = atol(arg);
  else valid = false;

  Serial.print("D ");
  Serial.print(servoNumber);
  Serial.print(" ");
  Serial.println(servoCommand);
  writeToServo(servoNumber, servoCommand);
}

//servoCommand is in [0, 180]
void writeToServo(int servoNumber, int servoPosition) {
  if ((servoNumber > 0) && (servoNumber <= numServos)) {
    servoPosition = max(servoPosition, lower[servoNumber - 1]);
    servoPosition = min(servoPosition, upper[servoNumber - 1]);
    currentPos[servoNumber - 1] = servoPosition;

//    Serial.print("W ");
//    Serial.print(servoNumber);
//    Serial.print(" ");
//    Serial.println(servoPosition);

    switch (servoNumber) {
      case 1:
        s1.write(servoPosition);
        break;
      case 2:
        s2.write(servoPosition);
        break;
      case 3:
        s3.write(servoPosition);
        break;
      case 4:
        s4.write(servoPosition);
        break;
      case 5:
        s5.write(servoPosition);
        break;
      case 6:
        s6.write(servoPosition);
        break;
      case 7:
        s7.write(servoPosition);
        break;
      case 8:
        s8.write(servoPosition);
        break;
      case 9:
        s9.write(servoPosition);
        break;
      case 10:
        s10.write(servoPosition);
        break;
    }
  }
  else {
    Serial.print("ERROR: Servo ");
    Serial.print(servoNumber);
    Serial.println(" does not exist");
  }
}

void setTargetServoPosition(int servoNumber, int servoPosition) {
  if ((servoNumber > 0) && (servoNumber <= numServos)) {
//    Serial.print("T ");
//    Serial.print(servoNumber);
//    Serial.print(" ");
//    Serial.println(servoPosition);

    servoPosition = max(servoPosition, lower[servoNumber - 1]);
    servoPosition = min(servoPosition, upper[servoNumber - 1]);
    targetPos[servoNumber - 1] = servoPosition;
  }
}

void updateServoPositions() {
  for (int servoNumber = 1; servoNumber <= numServos ; servoNumber++) {
    if (abs(currentPos[servoNumber - 1] - targetPos[servoNumber - 1]) > 2) {
      
      int smoothedPosition = (smoothening[servoNumber - 1] * currentPos[servoNumber - 1]);
      smoothedPosition    += ((1.0 - smoothening[servoNumber - 1]) * targetPos[servoNumber - 1]);

      if (smoothedPosition == currentPos[servoNumber - 1]) {
        targetPos[servoNumber - 1] = currentPos[servoNumber - 1];
      }
      else {
//        Serial.print("U ");
//        Serial.print(servoNumber);
//        Serial.print(" ");
//        Serial.println(smoothedPosition);
        writeToServo(servoNumber, smoothedPosition);
        delay(25);
      }
    }
  }
}

void initializeServos() {

  //attach servos
  s1.attach(9);
  s2.attach(11);
  s3.attach(7);
  s4.attach(8 );
  s5.attach(12);
  s6.attach(13);
  s7.attach(6);
  s8.attach(5);
  s9.attach(4);
  s10.attach(10);

  //initialize array and go to safe position
  for (int servoNumber = 1; servoNumber <= numServos ; servoNumber++) {
    currentPos[servoNumber - 1] = targetPos[servoNumber - 1];
    writeToServo(servoNumber, targetPos[servoNumber - 1]);
  }
}

void unrecognized()
{
}

