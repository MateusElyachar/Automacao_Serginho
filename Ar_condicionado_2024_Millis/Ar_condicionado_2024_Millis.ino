#include <Arduino.h>
#include "PinDefinitionsAndMore.h" // Define macros for input and output pin etc.
#define MARK_EXCESS_MICROS    20    // Adapt it to your IR receiver module. 20 is recommended for the cheap VS1838 modules.
#include <IRremote.hpp>
#include "DHT.h"

#define DHTPIN 12
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);



int SEND_BUTTON_PIN = APPLICATION_PIN;
int ledUm = 6;
int ledZero = 7;
int ar_agora = 1;

int DELAY_BETWEEN_REPEAT = 50;

void sendCode();

unsigned long currentMillis;
char d1; 
unsigned long time_now = -1;
const unsigned long interval = 200; // Interval between dht readings in milliseconds
unsigned long time_now2; // variavel pra caso de emergencia
unsigned long time_now3 = -1; // variavel de automação dos ar
const unsigned long interval2 = 43200000; // Intervalo de automação dos ar
float temperaturaF;
int DezMin =0;



void setup() {
    pinMode(SEND_BUTTON_PIN, INPUT_PULLUP);
    pinMode(ledZero, OUTPUT);
    pinMode(ledUm, OUTPUT);
    dht.begin();

    Serial.begin(9600);
#if defined(__AVR_ATmega32U4__) || defined(SERIAL_PORT_USBVIRTUAL) || defined(SERIAL_USB) /*stm32duino*/|| defined(USBCON) /*STM32_stm32*/|| defined(SERIALUSB_PID) || defined(ARDUINO_attiny3217)
    delay(4000); // To be able to connect Serial monitor after reset or power up and before first print out. Do not wait for an attached Serial Monitor!
#endif
    // Just to know which program is running on my Arduino
    Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE));

    // Start the receiver and if not 3. parameter specified, take LED_BUILTIN pin from the internal boards definition as default feedback LED
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
    Serial.print(F("Ready to receive IR signals of protocols: "));
    printActiveIRProtocols(&Serial);
    Serial.println(F("at pin " STR(IR_RECEIVE_PIN)));

    IrSender.begin(); // Start with IR_SEND_PIN as send pin and enable feedback LED at default feedback LED pin
    Serial.print(F("Ready to send IR signals at pin " STR(IR_SEND_PIN) " on press of button at pin "));
    Serial.println(SEND_BUTTON_PIN);
}

void loop() {
  temperatura_tempo();
  contaDez();
  alterna_ar();

  switch(d1){   
    case 'L':
      sendCode(1,1);
      d1 = 'o'; //significa nada
    break;

    case 'D':
      sendCode(1,0);
      d1 = 'o';
    break;

    case 'l':
      sendCode(0,1);
      d1 = 'o';
    break;

    case 'd':
      sendCode(0,0);
      d1 = 'o';
    break;
        
  }
}


void temperatura_tempo() {
  // Get the current time
  currentMillis = millis();
  
  // Check if it's time to take a new reading
  if (time_now == -1 || currentMillis - time_now >= interval) {
    // Update the last reading time
    time_now = currentMillis;
    temperaturaF = dht.readTemperature();

    if(temperaturaF >= 23.0 && DezMin == 0){
      sendCode(0,1);
      sendCode(1,1);
      DezMin = 1;
      time_now2 = currentMillis; 
      Serial.print("Alerta alta temperatura! ");
    }

    Serial.print("temperatura: ");
    Serial.println(temperaturaF);
    
  }
}

void alterna_ar(){
  if (time_now3 == -1 || currentMillis - time_now3 >= interval2) {//altera ar a cada 12 hrs
    time_now3 = currentMillis;
    sendCode(ar_agora,0);
    sendCode(-ar_agora,1);
    ar_agora = -ar_agora;
    DezMin == 0;
  }
}

void contaDez() {
  // Check if it's time to take a new reading
  if (DezMin == 1 && currentMillis - time_now2 >= 600000) {//verifica se é para contar 10 min
    sendCode(ar_agora,0);
    sendCode(-ar_agora,1);
    ar_agora = -ar_agora;
    DezMin = 0;
  }
}

int sendCode(int NumeroDoAr, int Liga_Desliga) {
  if(NumeroDoAr == 1){ //controla primeiro ar
    if(Liga_Desliga){
      Serial.println("Ar 1 ligado");
      digitalWrite(ledUm, 1);
      digitalWrite(ledZero, 0);
      uint32_t tRawData[]={0x126CB23, 0x8032400, 0x3D, 0x8100};
      IrSender.sendPulseDistanceWidthFromArray(38, 3250, 1750, 350, 1300, 350, 450, &tRawData[0], 112, PROTOCOL_IS_LSB_FIRST, 0, NO_REPEATS);
    }
    else{
      Serial.println("Ar 1 desligado");
      digitalWrite(ledUm, 1);
      digitalWrite(ledZero, 0);    
      uint32_t tRawData[]={0x126CB23, 0x8032000, 0x3D, 0x7D00};
      IrSender.sendPulseDistanceWidthFromArray(38, 3400, 1600, 450, 1200, 450, 400, &tRawData[0], 112, PROTOCOL_IS_LSB_FIRST, 0, NO_REPEATS);
    }
  }
  else{ //controla segundo ar
    if(Liga_Desliga){
      Serial.println("Ar 2 ligado");
      digitalWrite(ledUm, 0);
      digitalWrite(ledZero, 1);
      uint32_t tRawData[]={0x126CB23, 0x8032400, 0x3D, 0x8100};
      IrSender.sendPulseDistanceWidthFromArray(38, 3250, 1750, 350, 1300, 350, 450, &tRawData[0], 112, PROTOCOL_IS_LSB_FIRST, 0, NO_REPEATS);
    }
    else{
      Serial.println("Ar 2 desligado");
      digitalWrite(ledUm, 0);
      digitalWrite(ledZero, 1);
      uint32_t tRawData[]={0x126CB23, 0x8032000, 0x3D, 0x7D00};
      IrSender.sendPulseDistanceWidthFromArray(38, 3400, 1600, 450, 1200, 450, 400, &tRawData[0], 112, PROTOCOL_IS_LSB_FIRST, 0, NO_REPEATS);    
    }
  }
    
}
