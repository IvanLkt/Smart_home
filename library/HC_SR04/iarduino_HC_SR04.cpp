#include "iarduino_HC_SR04.h"
//		инициализация модуля
		iarduino_HC_SR04::iarduino_HC_SR04(uint8_t i, uint8_t j){	//	i: № вывода TRIG, j: № вывода ECHO)
		HC_pin_TRIG=i; pinMode(HC_pin_TRIG, OUTPUT);				//	сохраняем № вывода TRIG и устанавливаем ему режим работы на выход
		HC_pin_ECHO=j; pinMode(HC_pin_ECHO, INPUT );				//	сохраняем № вывода ECHO и устанавливаем ему режим работы на вход
		digitalWrite(HC_pin_TRIG, LOW);								//	устанавливаем уровень логического «0» на выводе TRIG
}
//		определение расстояния
long	iarduino_HC_SR04::distance(int8_t i){						//	i: текущая температура в °C
		long		j=HC_fun_duration();							//	считываем длительность импульса на выводе ECHO
		if(j<400){	j=HC_fun_duration();}							//	повторно считываем длительность импульса (для подавления "глюков" некоторых датчиков)
					j=j*sqrt(273+i)/1000;							//	определяем расстояние: L = Echo * корень(t+273) / 1000
		if(j>400){	j=400;}											//	ограничиваемся максимально допустимым расстоянием
		return j;
}
//		определение длительности
long	iarduino_HC_SR04::HC_fun_duration(){
		digitalWrite(HC_pin_TRIG, HIGH);							//	устанавливаем уровень логической «1» на выводе TRIG
		delayMicroseconds(10);										//	ждём 10 мкс
		digitalWrite(HC_pin_TRIG, LOW);								//	устанавливаем уровень логического «0» на выводе TRIG
		return pulseIn(HC_pin_ECHO, HIGH);							//	считываем длительность импульса на выводе ECHO
}
