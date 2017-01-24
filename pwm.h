/******************************************************************
* Area Ingenieria de Sistemas y Automatica
* Sistemas Empotrados
*******************************************************************
* nombre fichero : pwm.h 
*******************************************************************
* descripcion    : Fichero de cabecera modulo PWM
* programador    : JL Villarroel (JLV) 
* lenguaje       : ANSI C para CodeWarrior
* fecha          : 18 marzo 2014
********************************************************************/

#ifndef pwm_h
#define pwm_h

void Init_PWM (void) ;
void Set_Value (float value) ;
void Set_Value_10b (unsigned int value) ;

#endif
