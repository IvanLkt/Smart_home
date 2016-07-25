//	Библиотека работы arduino с датчиком HC_SR04 http://iarduino.ru/shop/Sensory-Datchiki/ultrazvukovogo-datchika-hc-sr04-rasstoyaniya-dvizheniya.html
//	Датчик HC_SR04 подключается к любым двум выводам arduino.
//
//	Подключение библиотеки:
//	#include <iarduino_HC_SR04.h>
//	iarduino_HC_SR04 переменная(TRIG, ECHO); // Объявляем переменную, для работы с библиотекой (указывая номера выводов arduino к которым подключены контакты TRIG и ECHO датчика).
//	
//	В библиотеке реализована только 1 функция:
//	
//	Функция distance     Назначение:            Возвращает расстояние до преграды или объекта в см
//	                     Синтаксис:             distance([int8_t t°C])
//	                     Параметры:             t°C - температура воздуха в °C (необязательный параметр) по умолчанию +23°C
//	                     Возвращаемые значения: long расстояние в см

#ifndef iarduino_HC_SR04_h
#define iarduino_HC_SR04_h

#if defined(ARDUINO) && (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

class	iarduino_HC_SR04{
	public:		iarduino_HC_SR04(uint8_t, uint8_t);	//	инициализация модуля	(вывод TRIG, вывод ECHO)
		long	distance		(int8_t=23);		//	определение расстояния	([t°C])
	private:
		long	HC_fun_duration	();					//	определение длительности импульса ECHO
		uint8_t	HC_pin_TRIG;						//	№ вывода подключённого к TRIG
		uint8_t	HC_pin_ECHO;						//	№ вывода подключённого к ECHO
};

#endif
