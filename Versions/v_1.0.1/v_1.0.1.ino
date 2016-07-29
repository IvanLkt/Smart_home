/*Версия 1.0.1
 * 1 реле на pin D3, тактовая кновка для управления
   2 поддержка датчиков температуры/влажности D5, освещенности А0, движения D6
   3 тактовые кновки для управления реле D2 и запоминания параметров датчиков D4
 */

#include <dht11.h>      

int relePin = 3;
int memoryPin = 4;
int switchRelePin = 2;

//Параметры кнопки реле
boolean lastButtonRele = LOW;
boolean currentButtonRele = LOW;

//Параметры кнопки  запоминания
boolean lastButtonMemory = LOW;
boolean currentButtonMemory = LOW;
int oldTemperature = 0;
int oldHumidity = 0;
int oldLight = 0;
int oldPirAct = 0;

dht11 DHT;               // Объявление переменной класса dht11
#define DHTPin 5         // Датчик DHT11 подключен к цифровому пину номер 5

const int lightPin = A0; //один вывод на vcc, второй на А0 и через 10КОм на gnd
int light = 0;

#define pirPin 2
int pirAct = 0;

void setup()
{
  pinMode(switchRelePin, INPUT);
  pinMode(relePin, OUTPUT);
  pinMode(memoryPin, INPUT); 
  pinMode(lightPin, INPUT);
  pinMode(pirPin, INPUT);
  Serial.begin(9600);
}

boolean debounce(boolean last, int Pin)
{
  boolean current = digitalRead(Pin);
  if (last != current)
  {
    delay(5);
    current = digitalRead(Pin);
  }
  return current;
}

void loop()
{
  //Реле
  currentButtonRele = debounce(lastButtonRele, relePin); 
  if (lastButtonRele == LOW && currentButtonRele == HIGH)
  {
    digitalWrite(relePin, HIGH);
  }
  else if (lastButtonRele == HIGH && currentButtonRele == LOW)
  {
    digitalWrite(relePin, LOW);
  }
  lastButtonRele = currentButtonRele;

  //Чтение данных с датчиков
  
  //датчик температуры и влажности
  int chk;   
  // Мониторинг ошибок
  chk = DHT.read(DHTPin);    // Чтение данных
  switch (chk){
  case DHTLIB_OK:  
    break;
  case DHTLIB_ERROR_CHECKSUM:
    Serial.println("Checksum error, \t");
    break;
  case DHTLIB_ERROR_TIMEOUT:
    Serial.println("Time out error, \t");
    break;
  default:
    Serial.println("Unknown error, \t");
    break;
  } 
  // Выводим показания влажности и температуры
  Serial.print("Humidity = ");
  Serial.print(DHT.humidity, 1);
  Serial.print(", Temp = ");
  Serial.println(DHT.temperature,1);

  //датчик освещенности
  light = analogRead(lightPin);
  Serial.println(light);

  //датчик движения
  int pirVal = digitalRead(pirPin);
  if (pirVal == HIGH)
  {
    pirAct = 1;
    Serial.print("Motion detected");
  }
  else
  {
    pirAct = 0;
  }
  
  //кнопка сохранения
  currentButtonMemory = debounce(lastButtonMemory, memoryPin); 
  if (lastButtonMemory == LOW && currentButtonMemory == HIGH)
  {
    int oldTemperature = DHT.temperature;
    int oldHumidity = DHT.humidity;
    int oldLight = light;
    int oldPirAct = pirAct;
  }
  lastButtonMemory = currentButtonMemory;

  delay(1000);
}
