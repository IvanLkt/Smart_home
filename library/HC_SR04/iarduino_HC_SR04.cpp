#include "iarduino_HC_SR04.h"
//		������������� ������
		iarduino_HC_SR04::iarduino_HC_SR04(uint8_t i, uint8_t j){	//	i: � ������ TRIG, j: � ������ ECHO)
		HC_pin_TRIG=i; pinMode(HC_pin_TRIG, OUTPUT);				//	��������� � ������ TRIG � ������������� ��� ����� ������ �� �����
		HC_pin_ECHO=j; pinMode(HC_pin_ECHO, INPUT );				//	��������� � ������ ECHO � ������������� ��� ����� ������ �� ����
		digitalWrite(HC_pin_TRIG, LOW);								//	������������� ������� ����������� �0� �� ������ TRIG
}
//		����������� ����������
long	iarduino_HC_SR04::distance(int8_t i){						//	i: ������� ����������� � �C
		long		j=HC_fun_duration();							//	��������� ������������ �������� �� ������ ECHO
		if(j<400){	j=HC_fun_duration();}							//	�������� ��������� ������������ �������� (��� ���������� "������" ��������� ��������)
					j=j*sqrt(273+i)/1000;							//	���������� ����������: L = Echo * ������(t+273) / 1000
		if(j>400){	j=400;}											//	�������������� ����������� ���������� �����������
		return j;
}
//		����������� ������������
long	iarduino_HC_SR04::HC_fun_duration(){
		digitalWrite(HC_pin_TRIG, HIGH);							//	������������� ������� ���������� �1� �� ������ TRIG
		delayMicroseconds(10);										//	��� 10 ���
		digitalWrite(HC_pin_TRIG, LOW);								//	������������� ������� ����������� �0� �� ������ TRIG
		return pulseIn(HC_pin_ECHO, HIGH);							//	��������� ������������ �������� �� ������ ECHO
}
