/******************************************************************
* Area Ingenieria de Sistemas y Automatica
* Sistemas Empotrados
*******************************************************************
* nombre fichero : control.c 
*******************************************************************
* descripcion    : control de la lavadora
* programador    : Marcos Canales Mayo
* lenguaje       : ANSI C para CodeWarrior
* fecha          : marzo 2015
********************************************************************/

#include "derivative.h"

#include "ad.h"
#include "pwm.h"
#include "clock.h"

#define T_HIPERPERIODO  40
#define NUM_MARCOS  8
#define T_MARCO (T_HIPERPERIODO/NUM_MARCOS)

static void InitSystem (void);
static void ConfigurarEntradas(void);
static void ConfigurarSalidas(void);
static unsigned char LeerEntrada(void);
static void display (unsigned char queDisplay, unsigned char valor);
static void visualizar(void);
static void genera_pwm(float v_giro_deseada);
static void genera_leds(void);

extern void Init_PL(void);
extern void configura_pl(unsigned char input);
extern void configura_accion(void);
extern void decrementar_paso(void);
extern unsigned char leer_programa(void);
extern unsigned char leer_paso(void);
extern unsigned char leer_ctfg(void);
extern unsigned char leer_estado_pl(void);
extern unsigned int leer_duracion(void);
extern float leer_v_giro_deseada(void);

static float uk, uk_1, ek, ek_1, v_dinamo;
static unsigned char marco, lectura_ptad;
static unsigned int siguiente;
unsigned char btn_marcha;

void main(void) {
  //inicializacion de modulos
  InitSystem();
  Init_AD();
  Init_PWM();
  ConfigurarEntradas();
  ConfigurarSalidas();
  Reset_Clock();
    
  //inicializacion de variables
  Init_PL();
  btn_marcha = 0;
  uk_1 = 0.0;
  ek_1 = 0.0;
  
  //ejecutivo ciclico
  marco = 0;
  siguiente = Get_Time();
  while(1) {
    //en todos los marcos el visualizador actuara
    PTDD_PTDD3 = 1;
    visualizar();
    switch(marco){
      case 0:
        PTDD_PTDD3 = 0;
        PTDD_PTDD2 = 1;
        genera_pwm(leer_v_giro_deseada());
        break;
      case 1:
        PTDD_PTDD3 = 0;
        PTDD_PTDD1 = 1;
        lectura_ptad = LeerEntrada();
        break;
      case 2:
        PTDD_PTDD3 = 0;
        PTDD_PTDD1 = 1;
        configura_pl(lectura_ptad);
        btn_marcha = lectura_ptad & 0x2;
        break;
      case 3:
        PTDD_PTDD3 = 0;
        PTDD_PTDD1 = 1;
        configura_accion();
        //Ha pasado el periodo del paso o acaba de pulsar marcha
        if ((leer_paso() && Time_Out()) || (leer_estado_pl() && !get_active_timer())){
          Set_Timer(leer_duracion()*1000, decrementar_paso);
        }
        break;
      case 4:
        PTDD_PTDD3 = 0;
        PTDD_PTDD2 = 1;
        genera_pwm(leer_v_giro_deseada());
        break;
      case 5:
        PTDD_PTDD3 = 0;
        genera_leds();
        break;
      case 6:
        PTDD_PTDD3 = 0;
        break;
      case 7:
        PTDD_PTDD3 = 0;
        break;
      default:
        break;
    }
    PTDD_PTDD1 = 0;
    PTDD_PTDD2 = 0;
    //nuevo marco
    marco++;
    if (marco==8) marco = 0;
    //delay
    siguiente = siguiente + T_MARCO;
    delay_until(siguiente);
  }
}

void InitSystem (void) {

  /* Internal Clock Source Configuration */
	if(NVICSTRM == 0xFF)			// If programmed trim value is blank
		ICSTRM = 0x80;				  // load default value
	else
		ICSTRM = NVICSTRM;	  	// else load programmed trim value
  ICSC2_BDIV = 1 ;          // Divides selected clock by 2		
	                          // Bus clock = 5 MHz, CPU clock = 10MHz
  IRQSC_IRQEDG = 1;      // Interrupciones en flanco ascendente
  
  SOPT1_COPT = 0 ;          // WATCHDOG disable 
}

void ConfigurarEntradas(void){
  PTADD = 0x00;         // PTA pins as inputs
  PTAPE = 0xEE;         // Input ports with pullup
  SOPT1_RSTPE = 0 ;     // PTA5 pin reset disable
  SOPT1_TCLKPEN = 0 ;   // PTA5 pin TCLK disable
  IRQSC_IRQPE = 0 ;     // PTA5 pin IRQ disable
}

void ConfigurarSalidas(void)
{
  PTBD = 0x00;
  PTBDD |= 0xFF;    // PTB0..7 as outputs
  PTCD = 0x00;
  PTCDD |= 0x7F;     // PTC0..6 as outputs
  PTDD = 0x00;
  PTDDD |= 0x1E;     // PTD1..4 as outputs
}

unsigned char LeerEntrada(void)
{
   return (~PTAD)&0xEE ;
}

void display (unsigned char queDisplay, unsigned char valor) 
{
  PTCD = ((~(0x1 << queDisplay)) << 4) | (valor&0x0F) ;
  return ;
}

void visualizar(void)
{
  if (marco%2){
    display(1, leer_programa());
  } else{
    display(0, leer_paso());
  }
}

void genera_pwm(float v_giro_deseada)
{
  //calcular proximo valor a partir de PTB0 y PTB1
  v_dinamo = Read_Value_2();
  ek = v_giro_deseada - v_dinamo;
  uk = 0.245*ek - 0.175*ek_1 + uk_1;
  //generar proximo valor por PTD4
  Set_Value(uk);
  //preparar valores para la siguiente iteracion
  uk_1 = uk;
  ek_1 = ek;
}

void genera_leds(void){
  PTBD_PTBD2 = leer_estado_pl();
  switch(leer_programa()){
    case 1:
      PTBD_PTBD3 = 1;
      PTBD_PTBD4 = 0;
      PTBD_PTBD5 = 0;
      break;
    case 2:
      PTBD_PTBD3 = 0;
      PTBD_PTBD4 = 1;
      PTBD_PTBD5 = 0;
      break;
    case 3:
      PTBD_PTBD3 = 0;
      PTBD_PTBD4 = 0;
      PTBD_PTBD5 = 1;
      break;
    default:
      break;
  }
  switch(leer_ctfg()){
    case 0:
      PTBD_PTBD6 = 1;
      PTBD_PTBD7 = 0;
      break;
    case 1:
      PTBD_PTBD6 = 0;
      PTBD_PTBD7 = 1;
      break;
    default:
      break;
  }
}