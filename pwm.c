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

#include "derivative.h"
#include "pwm.h"

void Init_PWM(void){
  
  TPM1SC_PS = 0 ;                   // Preescaler divide by 1
  TPM1SC_CPWMS = 0 ;                // Edge alligned PWM
  
  TPM1MOD = 0x03FF ;                // 10 bits PWM

  TPM1C4SC_MS4A = 0 ;               // Edge-aligned PWM
  TPM1C4SC_MS4B = 1 ; 
  TPM1C4SC_ELS4A = 0 ;              // High-tru pulses
  TPM1C4SC_ELS4B = 1 ;

  TPM1SC_CLKSA = 1 ;                // Bus clock 5MHz
  TPM1SC_CLKSB = 0 ;

}



void Set_Value_10b (unsigned int value)
{
   if (value > 0x03FF) value = 0x03FF ;
   TPM1C4V = value ;
}
 

void Set_Value (float value)
{   
    unsigned int Vaux ;

    if (value > 10.0) value = 10.0 ;
    if (value < -10.0) value = -10.0 ;
    Vaux =(unsigned int)((value+10.0)*51.15) ;
    Set_Value_10b (Vaux) ;
}
 