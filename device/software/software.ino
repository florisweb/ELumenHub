#include "connectionManager.h"


const int motorEnablePins[4] = {14, 27, 13, 12}; // Plants, left to right

const int moistureSensorEnablePins[4] = {21, 5, 18, 19}; // 5 todo
const int moistureSensorReadPin = 33;


const int moistureCheckFrequency = 5 * 60 * 1000; // Every 15 minutes
int targetMoisturePercs[4] = {50, 0, 50, 50};

// Calibrations
const int motorMLOffsets[4] = {718, 548, 1526, 1692}; // ms until water starts to flow out at the plant
const int motorMLSlopes[4] = {37, 44, 61, 67}; // ms/mL: amount of ms one mL of water takes after the offset above
const int moistureSensorMinima[4] = {0, 0, 0, 0}; // Defined as the value when in air
const int moistureSensorMaxima[4] = {995, 995, 900, 1024}; // Defined as the value when in pure water


connectionManager ConnectionManager;


const int stockVolume = 5000; // mL
int waterVolumeInStock = stockVolume; // mL



void onWiFiConnStateChange(bool conned) {
  Serial.print("Connected?: ");
  Serial.println(conned);
}

void onMessage(DynamicJsonDocument message) {
  String packetType = message["type"];
  String error = message["error"];

  Serial.print("[OnMessage] Error: ");
  Serial.println(error);
  Serial.print("[OnMessage] type: ");
  Serial.println(packetType);
}

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


  ConnectionManager.defineEventDocs("[]");
  ConnectionManager.defineAccessPointDocs("["
                                          "{"
                                          "\"type\": \"setText\","
                                          "\"description\": \"Sets text\""
                                          "}"
                                          "]");
  ConnectionManager.setup(&onMessage, &onWiFiConnStateChange);
}

String command;
int state = 0;
int curMotor = -1;
int duration = 0;
unsigned long prevUpdate = 0;
void loop() {
  ConnectionManager.loop();

  if (prevUpdate + moistureCheckFrequency < millis())
  {
    checkPlantMoisture();
    prevUpdate = millis();
  }

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
      giveXMLWaterToPlantY(duration, curMotor);

      //      Serial.print("Running for: ");
      //      Serial.println(duration);
      //      digitalWrite(motorEnablePins[curMotor], HIGH);
      //      delay(duration);
      //      digitalWrite(motorEnablePins[curMotor], LOW);
    }
  }
}

int prevMoisturePercs[4];
void checkPlantMoisture() {
  int moisturePercs[4];
  for (int i = 0; i < sizeof(moistureSensorEnablePins) / sizeof(int); i++)
  {
    moisturePercs[i] = readAveragedMoisture(i);
    Serial.print("moisture ");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(moisturePercs[i]);
    if (moisturePercs[i] >= targetMoisturePercs[i])
    {
      Serial.print(" >= ");
      Serial.print(targetMoisturePercs[i]);
      Serial.println(": not giving water!");
    } else {
      Serial.print(" < ");
      Serial.print(targetMoisturePercs[i]);
      Serial.println(": giving water!");
      giveXMLWaterToPlantY(25, i);
    }
  }

  bool changed = false;
  for (int i = 0; i < sizeof(moistureSensorEnablePins) / sizeof(int); i++)
  {
    if (prevMoisturePercs[i] == moisturePercs[i]) continue;
    changed = true;
  }
  if (!changed) 
  {
    Serial.println("Values have not changed.");
    return;
  }
  
  

  String dataString = "{";
  dataString.concat("\"type\": \"sensorState\", \"data\": [");
  for (int i = 0; i < sizeof(moistureSensorEnablePins) / sizeof(int); i++)
  {
    if (i != 0) dataString.concat(",");
    dataString.concat(String(moisturePercs[i]));
  }
  dataString.concat("]}");
  ConnectionManager.send(dataString);
  
  for (int i = 0; i < sizeof(moistureSensorEnablePins) / sizeof(int); i++) prevMoisturePercs[i] = moisturePercs[i];
}

void giveXMLWaterToPlantY(int mL, int motorIndex) {
  int ms = mL * motorMLSlopes[motorIndex] + motorMLOffsets[motorIndex];
  Serial.print("Giving ");
  Serial.print(mL);
  Serial.print(" mL water to motor ");
  Serial.print(motorIndex);
  Serial.print(" takes ");
  Serial.print(ms);
  Serial.println(" ms");

  digitalWrite(motorEnablePins[motorIndex], HIGH);
  delay(ms);
  digitalWrite(motorEnablePins[motorIndex], LOW);

  waterVolumeInStock -= mL;

  // Update Server
  int stockPercentage = waterVolumeInStock * 100 / stockVolume;

  String addedWaterValString = "[";
  for (int i = 0; i < sizeof(moistureSensorEnablePins) / sizeof(int); i++)
  {
    if (i != 0) addedWaterValString += ",";
    if (i == motorIndex) {
      addedWaterValString += String(mL);
    } else addedWaterValString += "0";
  }
  addedWaterValString += "]";

  String dataString = "{";
  dataString.concat("\"type\": \"waterState\",");
  dataString.concat( "\"data\": {\"stockPerc\": ");
  dataString.concat(String(stockPercentage));
  dataString.concat(",");
  dataString.concat("\"addedWaterVolumes\":");
  dataString.concat(addedWaterValString);
  dataString.concat("}}");

  ConnectionManager.send(dataString);
}


int readAveragedMoisture(int index) {
  const int averageCount = 20;
  digitalWrite(moistureSensorEnablePins[index], HIGH);
  delay(10);
  int sum = 0;
  for (int i = 0; i < averageCount; i++)
  {
    int curVal = analogRead(moistureSensorReadPin);
    int perc = min(max(curVal - moistureSensorMinima[index], 0) * 100 / (moistureSensorMaxima[index] - moistureSensorMinima[index]), 100);
    sum += perc;
    delay(50);
  }
  digitalWrite(moistureSensorEnablePins[index], LOW);
  return sum / averageCount;
}

int readMoisture(int index) {
  digitalWrite(moistureSensorEnablePins[index], HIGH);
  delay(10);
  int curVal = analogRead(moistureSensorReadPin);
  int perc = min(max(curVal - moistureSensorMinima[index], 0) * 100 / (moistureSensorMaxima[index] - moistureSensorMinima[index]), 100);

  delay(10);
  digitalWrite(moistureSensorEnablePins[index], LOW);
  return perc;
}
