//	���������� ������ arduino � �������� HC_SR04 http://iarduino.ru/shop/Sensory-Datchiki/ultrazvukovogo-datchika-hc-sr04-rasstoyaniya-dvizheniya.html
//	������ HC_SR04 ������������ � ����� ���� ������� arduino.
//
//	����������� ����������:
//	#include <iarduino_HC_SR04.h>
//	iarduino_HC_SR04 ����������(TRIG, ECHO); // ��������� ����������, ��� ������ � ����������� (�������� ������ ������� arduino � ������� ���������� �������� TRIG � ECHO �������).
//	
//	� ���������� ����������� ������ 1 �������:
//	
//	������� distance     ����������:            ���������� ���������� �� �������� ��� ������� � ��
//	                     ���������:             distance([int8_t t�C])
//	                     ���������:             t�C - ����������� ������� � �C (�������������� ��������) �� ��������� +23�C
//	                     ������������ ��������: long ���������� � ��

#ifndef iarduino_HC_SR04_h
#define iarduino_HC_SR04_h

#if defined(ARDUINO) && (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

class	iarduino_HC_SR04{
	public:		iarduino_HC_SR04(uint8_t, uint8_t);	//	������������� ������	(����� TRIG, ����� ECHO)
		long	distance		(int8_t=23);		//	����������� ����������	([t�C])
	private:
		long	HC_fun_duration	();					//	����������� ������������ �������� ECHO
		uint8_t	HC_pin_TRIG;						//	� ������ ������������� � TRIG
		uint8_t	HC_pin_ECHO;						//	� ������ ������������� � ECHO
};

#endif
