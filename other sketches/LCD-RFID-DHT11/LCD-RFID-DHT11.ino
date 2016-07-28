/*Получение данных с датчика температуры/влажности и вывод на дисплей по RFID карте
 * 
 * LCD:
 * SCL: pin A5
 * SDA: pin A4
 * 
 * HC11: pin 4
 *  
 * RFID:
* MOSI: Pin 11 / ICSP-4
* MISO: Pin 12 / ICSP-1
* SCK: Pin 13 / ISCP-3
* SS: Pin 10
* RST: Pin 9
*/

#include <SPI.h>
#include <RFID.h>

#define SS_PIN 10
#define RST_PIN 9

RFID rfid(SS_PIN, RST_PIN); 

unsigned char reading_card[5]; //for reading card
unsigned char master[5] = {123,178,168,133,228}; // allowed card
unsigned char i;

void allow();
void denied();

#include <dht11.h>      // Добавляем библиотеку DHT11
dht11 DHT;               // Объявление переменной класса dht11
#define DHT11_PIN 4     // Датчик DHT11 подключен к цифровому пину номер 4

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup(){
  lcd.init();                      // initialize the lcd 
  lcd.backlight();                 // подсветка
  Serial.begin(9600);
  lcd.clear();
  lcd.setCursor(0,0);

  SPI.begin(); 
  rfid.init();
  
  delay(100);
  
}

void loop () {
  
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
                  allow();
                }
                else
                {
                  denied();
                }
         } 
    }
    rfid.halt();
  
  int chk;   
  // Мониторинг ошибок
  chk = DHT.read(DHT11_PIN);    // Чтение данных
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
  delay(1000);
  
  lcd.clear();
  lcd.print("Check card");
}

void allow()
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

void denied()
{
  lcd.clear();
  lcd.print("Access denied!");
  delay(1500);
}


