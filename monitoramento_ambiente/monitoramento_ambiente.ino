// Libraries
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// Constants
#define DHTPIN 5      // Pin where the DHT22 is connected
#define DHTTYPE DHT22 // DHT 22 (AM2302)
#define BTN_ESQ 19
#define BTN_DIR 4
#define BTN_OK 15
#define LDRPIN 27
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

float setPointLuminosity = 4095/2;
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

State currentState = STAND_BY;

boolean enterPressionado = false;  // Variável para rastrear o estado do botão ENTER
boolean leftPressionado = false;   // Variável para rastrear o estado do botão LEFT
boolean rightPressionado = false;  // Variável para rastrear o estado do botão RIGHT

boolean debounce(int pinoBotao, boolean &estadoDoBotao) {
  boolean estadoAtual = digitalRead(pinoBotao);
  if (estadoAtual != estadoDoBotao) {
    delay(5);  // Delay de debounce para estabilidade
    estadoAtual = digitalRead(pinoBotao);
    if (estadoAtual != estadoDoBotao) {
      estadoDoBotao = estadoAtual;
      return estadoAtual == LOW;
    }
  }
  return false;
}


void setup()
{
  Serial.begin(9600);
  Serial.println("Temperature, Humidity Sensor Test");

  pinMode(BTN_ESQ, INPUT_PULLUP);
  pinMode(BTN_DIR, INPUT_PULLUP);
  pinMode(BTN_OK, INPUT_PULLUP);
  pinMode(LDRPIN, INPUT);
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
  // if (digitalRead(BTN_OK) == LOW)
  // {
  //   currentState = MONITORAR_AMBIENTE;
  // }
  if ((debounce(BTN_OK, enterPressionado))) {
    currentState = MONITORAR_AMBIENTE;
  }
}


unsigned long previousMillis = 0;
const long interval = 3000; // intervalo de 3 segundos
int step = 0;

void monitorar_ambiente()
{
  unsigned long currentMillis = millis();

  if (step == 0)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Para setar temp");
    lcd.setCursor(0, 1);
    lcd.print("Press > e OK");
    
    if (currentMillis - previousMillis >= interval)
    {
      previousMillis = currentMillis;
      step++;
    }
  }
  else if (step == 1)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Para sair");
    lcd.setCursor(0, 1);
    lcd.print("Press < e OK");
    
    if (currentMillis - previousMillis >= interval)
    {
      previousMillis = currentMillis;
      step++;
    }
  }
  else if (step == 2)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp:");
    lcd.setCursor(6, 0);
    lcd.print(monitorTemperature());
    lcd.setCursor(0, 1);  // Corrigir a linha para evitar sobrescrita
    lcd.print("Umid:");
    lcd.setCursor(6, 1); // Ajuste a coluna e linha
    lcd.print(monitorHumidity());
    step = 0; // Reinicia o ciclo
  }

  Serial.println(monitorLuminosity());
  if (monitorLuminosity() <= setPointLuminosity)
  {
    digitalWrite(LEDPIN, HIGH);
  }
  else
  {
    digitalWrite(LEDPIN, LOW);
  }

  if (debounce(BTN_ESQ, leftPressionado))
  {
    currentState = SAIR;
  }
  if (debounce(BTN_DIR, rightPressionado))
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

  if ((debounce(BTN_OK, enterPressionado))) {
    currentState = STAND_BY;
  }

  if ((debounce(BTN_DIR, rightPressionado))) {
    currentState = MONITORAR_AMBIENTE;
  }

  if ((debounce(BTN_ESQ, leftPressionado))) {
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

  if ((debounce(BTN_OK, enterPressionado))) {
    currentState = DEFINE_TEMP;
  }

  if ((debounce(BTN_DIR, rightPressionado))) {
    currentState = SAIR;
  }

  if ((debounce(BTN_ESQ, leftPressionado))) {
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

  if ((debounce(BTN_DIR, rightPressionado)))
  {
    setPointTemp++;
    if (setPointTemp >= 26)
    {
      setPointTemp = 26;
    }
  }

  if (debounce(BTN_ESQ, leftPressionado))
  {
    setPointTemp--;
    if (setPointTemp <= 18)
    {
      setPointTemp = 18;
    }
  }
  if (debounce(BTN_OK, enterPressionado))
  {
    currentState = MONITORAR_AMBIENTE;
  }
}

void loop()
{
  if (millis() - timer >= 1000)
  {
    timer = millis();
    switch (currentState)
    {
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
