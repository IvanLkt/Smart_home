/*Версия 2.1
 * 1 реле на pin D3, тактовая кновка для управления
   2 поддержка датчиков температуры/влажности D5, освещенности А0, движения D6
   3 тактовые кновки для управления реле D2 и запоминания параметров датчиков D4
   4 дисплей А4,А5
   5 RFID D6, D7, D11-D13 отображение показаний на дисплее
   6 инициализована сенсорная клавиатура D0,D1
 */

#include <dht11.h>
#include <SPI.h>
#include <RFID.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>  
#include <TTP229.h>    

//Реле
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

//Датчик температуры-влажности
dht11 DHT;               // Объявление переменной класса dht11
#define DHTPin 5         // Датчик DHT11 подключен к цифровому пину номер 5

//Датчик освещенности
const int lightPin = A0; //один вывод на vcc, второй на А0 и через 10КОм на gnd
int light = 0;

//Датчик движения
#define pirPin 2
int pirAct = 0;

/* RFID:
* MOSI: Pin 11 / ICSP-4
* MISO: Pin 12 / ICSP-1
* SCK: Pin 13 / ISCP-3
* SS: Pin 7
* RST: Pin 6
*/
#define SSPin 17
#define RSTPin 6
RFID rfid(SSPin, RSTPin);
unsigned char reading_card[5]; //for reading card
unsigned char master[5] = {123,178,168,133,228}; // allowed card
unsigned char i;

/* LCD:
 * SCL: pin A5
 * SDA: pin A4 
 */
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

//Клавиатура
const int SCLPin = 2;  // The pin number of the clock pin.
const int SDOPin = 3;  // The pin number of the data pin.
TTP229 ttp229(SCLPin, SDOPin); // TTP229(sclPin, sdoPin)
int tmpKey = 0;

  
void setup()
{
  lcd.init();                      // initialize the lcd 
  lcd.backlight();                 // подсветка
  Serial.begin(115200);
  lcd.clear();
  lcd.setCursor(0,0);

  SPI.begin(); 
  rfid.init();
  
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


void rfidCheck()
{
  if (rfid.isCard()) 
    {
        if (rfid.readCardSerial()) 
        {
                /* Reading card */
                Serial.println(" ");
                Serial.println("Card found");
                Serial.println("Cardnumber:"); 
                for (i = 0; i < 5; i++)
                {     
      Serial.print(rfid.serNum[i]);
                  Serial.print(" ");
                  reading_card[i] = rfid.serNum[i];
                }
                Serial.println();
                //verification
                for (i = 0; i < 5; i++)
                {
                  if (reading_card[i]!=master[i])
                  {
                    break;
                  }
                }
                if (i == 5)
                {
                  rfid_allow();
                }
                else
                {
                  rfid_denied();
                }
         } 
    }
    rfid.halt();  
}


void rfid_allow()
{
  lcd.clear();
  lcd.print("Access allow!");
  delay(1000);
  lcd.clear();
  lcd.print("TEMP:  ");
  lcd.print(DHT.temperature);
  lcd.setCursor(0,1);
  lcd.print("HUMI:  ");
  lcd.print(DHT.humidity);
  delay(3000);
}


void rfid_denied()
{
  lcd.clear();
  lcd.print("Access denied!");
  delay(1500);
}


void Rele()
{
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
}


void loop()
{
  Rele();
  
  //Чтение данных с датчиков  
  //Датчик температуры-влажности
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

  rfidCheck(); 

  uint8_t key = ttp229.ReadKey16(); // Blocking
  if (key) tmpKey = key;
}
