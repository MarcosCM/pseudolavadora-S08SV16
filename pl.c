/******************************************************************
* Area Ingenieria de Sistemas y Automatica
* Sistemas Empotrados
*******************************************************************
* nombre fichero : pl.c 
*******************************************************************
* descripcion    : modulo principal lavadora
* programador    : Marcos Canales Mayo
* lenguaje       : ANSI C para CodeWarrior
* fecha          : marzo 2015
********************************************************************/

#include "derivative.h"
#include "clock.h"

#define CTFG_NORMAL 3
#define CTFG_FUERTE 4

void Init_PL(void);
void configura_accion(void);
void configura_pl(unsigned char input);
void decrementar_paso(void);
unsigned char leer_programa(void);
unsigned char leer_paso(void);
unsigned char leer_ctfg(void);
unsigned char leer_estado_pl(void);
unsigned int leer_duracion(void);
float leer_v_giro_deseada(void);

static unsigned char programa, paso;
static unsigned char estado_pl; //0 = parada; 1 = marcha
static unsigned char ctfg; //0 = normal; 1 = fuerte
static unsigned int duracion;
static float v_giro_deseada;

extern unsigned char btn_marcha;

void Init_PL(){
  estado_pl = 0;
  programa = 1;
  paso = 0;
  ctfg = 0;
  duracion = 0;
  v_giro_deseada = 0;
}

void configura_accion(){
  switch(programa){
    case 1:
      switch(leer_paso()){
        case 0:
          v_giro_deseada = 0;
          duracion = 0;
          break;
        case 1:
          if (ctfg) v_giro_deseada = CTFG_FUERTE;
          else v_giro_deseada = CTFG_NORMAL;
          duracion = 10;
          break;
        case 2:
          v_giro_deseada = 0;
          duracion = 4;
          break;
        case 3:
          v_giro_deseada = 1;
          duracion = 4;
          break;
        case 4:
          v_giro_deseada = -1;
          duracion = 4;
          break;
        case 5:
          v_giro_deseada = 1;
          duracion = 4;
          break;
        default:
          break;
      }
      break;
    case 2:
      switch(leer_paso()){
        case 0:
          v_giro_deseada = 0;
          duracion = 0;
          break;
        case 1:
          if (ctfg) v_giro_deseada = CTFG_FUERTE;
          else v_giro_deseada = CTFG_NORMAL;
          duracion = 6;
          break;
        case 2:
          v_giro_deseada = 0;
          duracion = 2;
          break;
        case 3:
          v_giro_deseada = 1;
          duracion = 4;
          break;
        case 4:
          v_giro_deseada = -0.5;
          duracion = 2;
          break;
        default:
          break;
      }
      break;
    case 3:
      switch(leer_paso()){
        case 0:
          v_giro_deseada = 0;
          duracion = 0;
          break;
        case 1:
          if (ctfg) v_giro_deseada = -CTFG_FUERTE;
          else v_giro_deseada = -CTFG_NORMAL;
          duracion = 3;
          break;
        case 2:
          if (ctfg) v_giro_deseada = CTFG_FUERTE;
          else v_giro_deseada = CTFG_NORMAL;
          duracion = 3;
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

void configura_pl(unsigned char input)
{
  unsigned char estado = leer_estado_pl();
  if (input & (1 << 1) && !btn_marcha){
    if (estado){
      estado_pl = 0;
      //Si han parado la maquina antes de que termine
      Remove_Timer();
      paso = 0;
    }
    else{
      switch(leer_programa()){
        case 1:
          paso = 5;
          break;
        case 2:
          paso = 4;
          break;
        case 3:
          paso = 2;
          break;
        default:
          break;
      }
      estado_pl = 1;
    }
  } else if (input & (1 << 2) && !estado){
    ctfg = 0;
  } else if (input & (1 << 3) && !estado){
    ctfg = 1;
  } else if (!estado){
    //Solo cambia de programa si la maquina esta parada
    if (input & (1 << 5)){
      paso = 0;
      programa = 1;
    } else if (input & (1 << 6)){
      paso = 0;
      programa = 2;
    } else if (input & (1 << 7)){
      paso = 0;
      programa = 3;
    }
  }
}

void decrementar_paso(void){
  paso--;
  if (leer_paso() == 0){
    estado_pl = 0;
    Remove_Timer();
  }
}

unsigned char leer_programa(void){
  unsigned char aux;
  
  asm sei;
  aux = programa;
  asm cli;
  
  return aux;
}

unsigned char leer_paso(void){
  unsigned char aux;
  
  asm sei;
  aux = paso;
  asm cli;
  
  return aux;
}

unsigned char leer_ctfg(void){
  unsigned char aux;
  
  asm sei;
  aux = ctfg;
  asm cli;
  
  return aux;
}

unsigned char leer_estado_pl(void){
  unsigned char aux;
  
  asm sei;
  aux = estado_pl;
  asm cli;
  
  return aux;
}

unsigned int leer_duracion(void){
  unsigned int aux;
  
  asm sei;
  aux = duracion;
  asm cli;
  
  return aux;
}

float leer_v_giro_deseada(void){
  float aux;
  
  asm sei;
  aux = v_giro_deseada;
  asm cli;
  
  return aux;
} 
