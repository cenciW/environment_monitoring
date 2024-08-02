// Libraries
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// Constants
#define DHTPIN 5      // Pin where the DHT22 is connected
#define DHTTYPE DHT22 // DHT 22 (AM2302)
#define BTN_ESQ 17
#define BTN_DIR 16
#define BTN_OK 4
#define LDRPIN 36
#define LEDPIN 2

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE);

// Initialize LCD
LiquidCrystal_I2C lcd(0x27, 16, 2); // Set the LCD address to 0x27 for a 16 chars and 2 line display, please check your LCD

// Variables
float h; // Stores humidity value
float t; // Stores temperature value
// int soil_moisture;  // Stores soil moisture value
uint32_t timer = 0; // Timer for readings

float setPointLuminosity = 0;
float setPointTemp = 22;

// States
enum State
{
  STAND_BY,
  MONITORAR_AMBIENTE,
  SAIR,
  ESCOLHE_TEMP,
  DEFINE_TEMP
};

State currentState = MONITOR_TEMP_UMID;

void setup()
{
  Serial.begin(9600);
  Serial.println("Temperature, Humidity Sensor Test");

  pinMode(BTN_ESQ, INPUT_PULLUP);
  pinMode(BTN_DIR, INPUT_PULLUP);
  pinMode(BTN_OK, INPUT_PULLUP);
  pinMode(LDRPIN, INPUT_PULLUP);
  pinMode(LEDPIN, OUTPUT);

  dht.begin();
  lcd.init();      // Initialize the LCD
  lcd.backlight(); // Turn on the backlight
}

void stand_by()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Monitoramento");
  lcd.setCursor(0, 1);
  lcd.print("Ambiente");
  if (BTN_OK == HIGH)
  {
    currentState = MONITORAR_AMBIENTE;
  }
}

void monitorar_ambiente()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Para setar temp");
  lcd.setCursor(0, 1);
  lcd.print("Press > e OK");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Para sair");
  lcd.setCursor(0, 1);
  lcd.print("Press < e OK");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp Atual:");
  lcd.setCursor(12, 0);
  lcd.print(monitorTemperature());
  lcd.setCursor(0, 0);
  lcd.print("Humid Atual:");
  lcd.setCursor(13, 0);
  lcd.print(monitorHumidity());

  if (monitorLuminosity() <= setPointLuminosity)
  {
    digitalWrite(LEDPIN, HIGH);
  }
  else
  {
    digitalWrite(LEDPIN, LOW);
  }

  if (BTN_ESQ == HIGH)
  {
    currentState = SAIR;
  }
  if (BTN_DIR == HIGH)
  {
    currentState = ESCOLHE_TEMP;
  }
}

void sair()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Press OK ");
  lcd.setCursor(0, 1);
  lcd.print("Para sair!");
  if (BTN_OK == HIGH)
  {
    currentState = STAND_BY;
  }
  if (BTN_DIR == HIGH)
  {
    currentState = MONITORAR_AMBIENTE;
  }
  if (BTN_ESQ == HIGH)
  {
    currentState = ESCOLHE_TEMP;
  }
}

void escolhe_temp()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Setar Temp:");
  lcd.setCursor(0, 1);
  lcd.print("Press OK");
  if (BTN_OK == HIGH)
  {
    currentState = DEFINE_TEMP;
  }
  if (BTN_DIR == HIGH)
  {
    currentState = SAIR;
  }
  if (BTN_ESQ == HIGH)
  {
    currentState = MONITORAR_AMBIENTE;
  }
}

void define_temp()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("< - | ");
  lcd.setCursor(6, 0);
  lcd.print(setPointTemp);
  lcd.setCursor(8, 0);
  lcd.print(" | + >");
  lcd.setCursor(0, 1);
  lcd.print("Press OK");

  if (BTN_DIR == HIGH)
  {
    setPointTemp++;
    if (setPointTemp >= 26)
    {
      setPointTemp = 26;
    }
  }
  if (BTN_ESQ == HIGH)
  {
    setPointTemp--;
    if (setPointTemp <= 18)
    {
      setPointTemp = 18;
    }
  }
  if (BTN_OK == HIGH)
  {
    currentState = MONITORAR_AMBIENTE;
  }
}

void loop()
{
  if (millis() - timer >= 1000)
  {
    switch (currentState)
    {
      time = millis();
    case STAND_BY:
      stand_by();
      break;
    case MONITORAR_AMBIENTE:
      monitorar_ambiente();
      break;
    case SAIR:
      sair();
      break;
    case ESCOLHE_TEMP:
      escolhe_temp();
      break;
    case DEFINE_TEMP:
      define_temp();
      break;
    }
  }
}

float monitorHumidity()
{
  if (millis() - timer >= 2000)
  {
    timer = millis();
    return h = dht.readHumidity();
  }
}

float monitorTemperature()
{
  if (millis() - timer >= 2000)
  {
    timer = millis();
    return t = dht.readTemperature();
  }
}

float monitorLuminosity()
{
  return analogRead(LDRPIN);
}
