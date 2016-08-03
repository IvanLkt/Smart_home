
/*Версия 2.3
 * 1 реле на pin D2,
   2 поддержка датчиков температуры/влажности D5, освещенности А0, движения D1
   3 сенсорные кновки для управления реле 13 и запоминания параметров датчиков 14 и вывода данных 5
   4 дисплей А4,А5
   5 RFID D6, D7, D11-D13 отображение показаний на дисплее
   6 инициализована сенсорная клавиатура D4,D8
   7 звуковой сигнал D9 при правильной карте
   8 прерывание по тактовой кнопке для ввода с сенсорной клавиатуры
   UPDATE v 2.2.1 програмная отладка тактовой кнопки 

    */

#include <dht11.h>
#include <SPI.h>
#include <RFID.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>  
#include <TTP229.h>
#include <pitches.h> 

//Реле
int relePin = 2;
boolean lastRele = LOW;


//Параметры кнопки  запоминания
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
#define pirPin 0
int pirAct = 0;

/* RFID:
* MOSI: Pin 11 / ICSP-4
* MISO: Pin 12 / ICSP-1
* SCK: Pin 13 / ISCP-3
* SS: Pin 7
* RST: Pin 6
*/
#define SSPin 7
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
const int SCLPin = 4;  // The pin number of the clock pin.
const int SDOPin = 8;  // The pin number of the data pin.
TTP229 ttp229(SCLPin, SDOPin); // TTP229(sclPin, sdoPin)
int tmpKey = 0;
#define attachPin 3  //вход для кнопки (Int1) pin3
#define t_Dreb 50  //интервал для устраниения дребезга (мс)
int t_Tek = 0;    //текущий отсчет времени при определении нажатия
int t_Pred = 0;   //предыдущий отсчет
int t_T1 = 0;     //длительность отсутствия реакции на кнопку
int pin_Tek = 1;  //состояние кнопки (0 - замкнута)


int speakerPin = 9;
// задаем ноты для мелодии:
int melody[] = {
NOTE_C4, NOTE_G3,NOTE_G3, NOTE_A3, NOTE_G3,0, NOTE_B3, NOTE_C4};
// задаем продолжительность нот: 
// 4 – это четвертная нота, 8 – это восьмая нота и т.д.
int noteDurations[] = {4, 8, 8, 4,4,4,4,4};


void speaker()
{
    // выполняем перебор нот в мелодии:
    for (int thisNote = 0; thisNote < 8; thisNote++) {
 
    // чтобы рассчитать время продолжительности ноты, 
    // берем одну секунду и делим ее цифру, соответствующую нужному типу ноты:
    // например, четвертная нота – это 1000/4,
    // а восьмая нота – это 1000/8 и т.д.
    int noteDuration = 1000/noteDurations[thisNote];
    tone(speakerPin, melody[thisNote],noteDuration);
 
    // чтобы выделить ноты, делаем между ними небольшую задержку.
    // в данном примере сделаем ее равной продолжительности ноты
    // плюс еще 30% от продолжительности ноты:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // останавливаем проигрывание мелодии:
    noTone(speakerPin);
  }      
}


void setup()
{
  lcd.init();                      // initialize the lcd 
  lcd.backlight();                 // подсветка
  Serial.begin(115200);
  lcd.clear();
  lcd.setCursor(0,0);

  SPI.begin(); 
  rfid.init();

  pinMode(relePin, OUTPUT);
  pinMode(lightPin, INPUT);
  pinMode(pirPin, INPUT);

  pinMode(attachPin, INPUT_PULLUP); //вход с внутр.резистором на VCC
  attachInterrupt(1, keyboard, CHANGE);
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
                  speaker();
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
  tmpKey = 0;
}


void rfid_denied()
{
  lcd.clear();
  lcd.print("Access denied!");
  delay(1500);
  tmpKey = 0;
}


void Rele()
{
  if (tmpKey == 13 && lastRele == LOW)
  {
    digitalWrite(relePin, HIGH);
    lastRele = HIGH;
  }
  else if (tmpKey == 13 && lastRele == HIGH)
  {
    digitalWrite(relePin, LOW);
    lastRele = LOW;
  }
}


void keyboard()
{
   pin_Tek = digitalRead(attachPin); //читаем текущее состояние pin2 (Int0)
   t_Tek = millis(); //текущий отсчет времени
   if (t_Tek > t_T1) {//если время подавления дребезга не прошло - игнорируем
        if (pin_Tek == 0) { //замкнуто
            uint8_t key = ttp229.ReadKey16(); 
            tmpKey = key; 
            Serial.println(tmpKey);
       }
   }
   t_T1 = t_Tek + t_Dreb;
}

void loop()
{ 
  Serial.println("OK");
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
  if (tmpKey == 14)
  {
    int oldTemperature = DHT.temperature;
    int oldHumidity = DHT.humidity;
    int oldLight = light;
    int oldPirAct = pirAct;
  }
  if (tmpKey == 5)
  {
    rfid_allow();
  }
  Serial.println(tmpKey); 
  rfidCheck(); 
  lcd.clear();
}
