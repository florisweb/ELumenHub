const int motorEnablePins[4] = {14, 27, 13, 12}; // Plants, left to right
const int moistureSensorEnablePins[3] = {5, 18, 19};
const int moistureSensorReadPin = 33;

void setup() {
  Serial.begin(115200);
  Serial.println("test");
  Serial.println(sizeof(motorEnablePins) / sizeof(int));

  for (int i = 0; i < sizeof(motorEnablePins) / sizeof(int); i++)
  {
    pinMode(motorEnablePins[i], OUTPUT);
  }
  for (int i = 0; i < sizeof(moistureSensorEnablePins) / sizeof(int); i++)
  {
    pinMode(moistureSensorEnablePins[i], OUTPUT);
    digitalWrite(moistureSensorEnablePins[i], LOW);
  }
  pinMode(moistureSensorReadPin, INPUT);
}

String command;
int state = 0;
int curMotor = -1;
int duration = 0;
void loop() {

  Serial.print("moisture 0: ");
  Serial.print(readAveragedMoisture(0));
  Serial.print(" 1: ");
  Serial.print(readAveragedMoisture(1));
  Serial.print(" 2: ");
  Serial.println(readAveragedMoisture(2));
  delay(1000);



  if (Serial.available()) {
    command = Serial.readStringUntil('\n');
    command.trim();

    if (curMotor == -1)
    {
      if (command.equals("0")) {
        curMotor = 0;
      } else if (command.equals("1")) {
        curMotor = 1;
      } else if (command.equals("2")) {
        curMotor = 2;
      } else if (command.equals("3")) {
        curMotor = 3;
      }
      Serial.print("Curmotor set to: ");
      Serial.println(curMotor);
    } else if (command.equals("reset")) {
      curMotor = -1;
      Serial.println("Reset.");
    } else {
      duration = command.toInt();
      Serial.print("Running for: ");
      Serial.println(duration);
      digitalWrite(motorEnablePins[curMotor], HIGH);
      delay(duration);
      digitalWrite(motorEnablePins[curMotor], LOW);
    }
  }
}


int readAveragedMoisture(int index) {
  const int averageCount = 20;
  digitalWrite(moistureSensorEnablePins[index], HIGH);
  delay(10);
  int sum = 0;
  for (int i = 0; i < averageCount; i++)
  {
    sum += analogRead(moistureSensorReadPin);
    delay(50);
  }
  digitalWrite(moistureSensorEnablePins[index], LOW);
  return sum / averageCount;
}

int readMoisture(int index) {
  digitalWrite(moistureSensorEnablePins[index], HIGH);
  delay(10);
  int curVal = analogRead(moistureSensorReadPin);
  delay(10);
  digitalWrite(moistureSensorEnablePins[index], LOW);
  return curVal;
}
