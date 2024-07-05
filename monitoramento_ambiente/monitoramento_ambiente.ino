// Libraries
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// Constants
#define DHTPIN 2       // Pin where the DHT22 is connected
#define HL69PIN A0     // Pin where the soil moisture sensor is connected
#define DHTTYPE DHT22  // DHT 22 (AM2302)

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE);

// Initialize LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Set the LCD address to 0x27 for a 16 chars and 2 line display, please check your LCD

// Variables
float h;  // Stores humidity value
float t;  // Stores temperature value
// int soil_moisture;  // Stores soil moisture value
uint32_t timer = 0;  // Timer for readings

// Calibration values for the soil moisture sensor
// int soilMoistureMin = 1005;  // Replace with your dry soil value -> next 1023
// int soilMoistureMax = 238;   // Replace with your wet soil value -> next 0

// Linearization coefficients
const float slope = -0.1303;
const float intercept = 130.7865;

// States
enum State {
  MONITOR_TEMP_UMID,
  SHOW_TEMP,
  // MONITOR_SOIL,
  // UPDATE_SOIL
};

State currentState = MONITOR_TEMP_UMID;

void setup() {
  Serial.begin(9600);
  Serial.println("Temperature, Humidity Sensor Test");

  pinMode(HL69PIN, INPUT);

  dht.begin();
  lcd.init();       // Initialize the LCD
  lcd.backlight();  // Turn on the backlight

  // // Calibration instructions
  // Serial.println("Calibration Instructions:");
  // Serial.println("1. Place the sensor in dry soil and note the raw value.");
  // Serial.println("2. Place the sensor in wet soil and note the raw value.");
  // Serial.println("Update the soilMoistureMin and soilMoistureMax constants in the code.");
}

void loop() {
  switch (currentState) {
    case MONITOR_TEMP_UMID:
      monitorTemperature();
      break;
    case SHOW_TEMP:
      showTemperature();
      break;
      // case MONITOR_SOIL:
      //   monitorSoilMoisture();
      //   break;
      // case UPDATE_SOIL:
      //   updateSoilMoisture();
      //   break;
  }
}

void monitorTemperature() {
  if (millis() - timer >= 2000) {
    h = dht.readHumidity();
    t = dht.readTemperature();
    currentState = SHOW_TEMP;
    timer = millis();
  }
}

void showTemperature() {
  Serial.print("Humidity:");
  Serial.print(h);
  Serial.print(" %, Temp:");
  Serial.print(t);
  Serial.println(" °C");

  currentState = MONITOR_TEMP_UMID;
}

// void monitorSoilMoisture() {
//   if (millis() - timer >= 2000) {
//     soil_moisture = analogRead(HL69PIN);
//     currentState = UPDATE_SOIL;
//     timer = millis();
//   }
// }

// void updateSoilMoisture() {

//   // int soilMoisturePercent = round(slope * soil_moisture + intercept);
//   // soilMoisturePercent = constrain(soilMoisturePercent, 0, 100);
//   // float soil_moisture_percent_invert = (1.00 - soil_moisture/1023)*100.00;

//   // Serial.print("Soil Moisture: ");
//   //Serial.print(soil_moisture);
//   // Serial.print(soil_moisture);
//   // Serial.print(soilMoisturePercent);
//   // Serial.print(soil_moisture_percent_invert);
//   // Serial.println("%)");

//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print("T:");
//   lcd.print(t, 1); // One decimal place
//   lcd.print("C");

//   lcd.setCursor(8, 0);
//   lcd.print(" H:");
//   lcd.print(h, 1); // One decimal place
//   lcd.print("%");

//   // lcd.setCursor(0, 1);
//   // lcd.print("Solo: ");
//   // lcd.print(soil_moisture_percent_invert);
//   // lcd.print("% (");
//   // lcd.print(soil_moisture);
//   // lcd.print(")");

//   currentState = MONITOR_TEMP_UMID;
// }
