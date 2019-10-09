#include <Arduino.h>
#include <SoftwareSerial.h>
//----define de constantes-----------
#define TIMEOUT 3000
//-----------------------------------
//----define de funciones----------------
void incializa_variables_esp32();
void comandosAT_esp32(int);
void resetWifi();
void inicializando_esp32();
void ciclo_espera_wifi(int time);
void procesa_dato_eps32();
void envia_dato_esp32(const char* dato);
//-------------------------------------------------------
//----inicializaciones e variables globales--------------
SoftwareSerial esp32(14,15); //define un esp32 en esa porta serial
bool wifi_conectada;
int step_wifi;
char c;//caracter 
char reponse_esp32[200];
bool no_respuesta;
int contador_caracteres_esp32;
char cell;//identifica un cliente pelo tcp
//---------------------------------------------------------

//----setup-------------------------------------------------
void setup() {
  // put your setup code here, to run once:
  incializa_variables_esp32();
  Serial.begin(9600);//para dar logs
  esp32.begin(115200);//inciando a porta serial do esp32

}
//-----------------------------------------------------------
//----main loop---------------------------------------------
void loop() {
  
  // put your main code here, to run repeatedly:
  
    if(!wifi_conectada){
      inicializando_esp32();
    }
    else if(!esp32.available()){//todo el codigo de la logica del programa


    }
    else
    { 
      int time = millis();
      ciclo_espera_wifi(time);
       //procesa el dato que llego por el esp32
       if (reponse_esp32[0]!=0x00){

       }
      
       
    }
    
}
//------------------------------------------------------------------------------


//----funcion para enviar comandos at al esp32----------------------------------
void comandosAT_esp32(int step){
  char temp[20];
  sprintf(temp,"step wifi:%d\r\n",step);
  Serial.print(temp);
  switch(step){
    case 0:
      esp32.print("AT+RESTORE\r\n");
      break;
    case 1:
      esp32.print("ATE1\r\n");
      break;
    case 2:
      esp32.print("AT+CWMODE=2\r\n");
      break;
    case 3:
      esp32.print("AT+CWSAP=\"WIFI-R\",\"555555555\",5,3,1,0\r\n");
      break;
    case 4:
      esp32.print("AT+CIPMUX=1\r\n");
      break;
    case 5:
      esp32.print("AT+CIPSERVER=1,5005\r\n");  
      break;
    case 6:
      esp32.print("AT+CIPSTO=60\r\n");
    case 7:      
      wifi_conectada = true;
      Serial.print("Wifi conectada\r\n");
      break;

  }


}
//---- funcion para inicializar as variaveis do esp32
void incializa_variables_esp32(){
  wifi_conectada = false;
  step_wifi = 0;
  contador_caracteres_esp32 = 0;

}
//---------------------------------------------------
//----funcion q resetea la conexion------------------
void resetWifi(){
  wifi_conectada = false;
  incializa_variables_esp32();
}
//-----------------------------------------------------
//----funcion q inicializa esp32--------------------------
void inicializando_esp32(){
  //si la red wifi no esta aun acticada
    comandosAT_esp32(step_wifi);
    //esperando respuesta del esp32
    long int time = millis();
    while(!no_respuesta && (time+TIMEOUT>millis())){
      ciclo_espera_wifi(time);
      reponse_esp32[contador_caracteres_esp32] = 0x00;
      contador_caracteres_esp32 = 0;
       if(strstr(reponse_esp32,"Ready")){
          no_respuesta = false;
          time = millis();
          ciclo_espera_wifi(time);


      }
      if(strstr(reponse_esp32,"OK"))
        step_wifi++;
      else if(strstr(reponse_esp32,"ERR"))
        step_wifi = 0;//ocurrio un error
       delay(2000);//espera de dos segundos 
     
    }

}

//--------------------------------------------------------
//----funcion q espera o pacote do esp32------------------
void ciclo_espera_wifi(int time){
 while(!no_respuesta && (time+TIMEOUT>millis())){
      while(esp32.available()){
        c = esp32.read();
        reponse_esp32[contador_caracteres_esp32] =c;
        contador_caracteres_esp32++;
        no_respuesta = true;
      }
  }

}
//-------------------------------------------------------------

void procesa_dato_eps32(){
  if(strstr(reponse_esp32,"#CONNECT")){
    cell = reponse_esp32[5];
    envia_dato_esp32("BELEZA CONNECTED\r\n");
  }
  else if(strstr(reponse_esp32,",CLOSED"))
    cell = '-';
}
//----funcion q envia dados pelo esp32------------------
void envia_dato_esp32(const char* dato){
  if(cell == '-')
    return ;
  char temporal[70];
  int count = strlen(dato);
  sprintf(temporal,"AT+CIPSEND=%c,%d\r\n",cell,count);
  no_respuesta = false;
  int time = millis();
  ciclo_espera_wifi(time);
  esp32.write(dato);
}