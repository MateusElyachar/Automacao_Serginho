#include <Arduino.h>

#include "PinDefinitionsAndMore.h" // Define macros for input and output pin etc.


// MARK_EXCESS_MICROS is subtracted from all marks and added to all spaces before decoding,
// to compensate for the signal forming of different IR receiver modules. See also IRremote.hpp line 142.
#define MARK_EXCESS_MICROS    20    // Adapt it to your IR receiver module. 20 is recommended for the cheap VS1838 modules.

//#define DEBUG // Activate this for lots of lovely debug output from the decoders.

#include <IRremote.hpp>

int SEND_BUTTON_PIN = APPLICATION_PIN;

int DELAY_BETWEEN_REPEAT = 50;

// Storage for the recorded code
struct storedIRDataStruct {
    IRData receivedIRData;
    // extensions for sendRaw
    uint8_t rawCode[220]; // The durations if raw
    uint8_t rawCodeLength; // The length of the code
} sStoredIRData;

bool NumeroDoAr;
bool Liga_Desliga;

void storeCode();
void sendCode();

String data;
char d1; 


void setup() {
    pinMode(SEND_BUTTON_PIN, INPUT_PULLUP);

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
  if(Serial.available()){
    data = Serial.readString();
    d1 = data.charAt(0);

    switch(d1){   
      case 'L':
        NumeroDoAr = 1;
        Liga_Desliga = 1;
        sendCode();
      break;

      case 'D':
        NumeroDoAr = 1;
        Liga_Desliga = 0;
        sendCode();
      break;
    }

  }
}

void sendCode() {
  if(Liga_Desliga){
    Serial.println("Ar 1 ligado");
    uint32_t tRawData[]={0x126CB23, 0x8032400, 0x3D, 0x8100};
    IrSender.sendPulseDistanceWidthFromArray(38, 3250, 1750, 350, 1300, 350, 450, &tRawData[0], 112, PROTOCOL_IS_LSB_FIRST, 0, NO_REPEATS);
  }
  else{
    uint32_t tRawData[]={0x126CB23, 0x8032000, 0x3D, 0x7D00};
    IrSender.sendPulseDistanceWidthFromArray(38, 3400, 1600, 450, 1200, 450, 400, &tRawData[0], 112, PROTOCOL_IS_LSB_FIRST, 0, NO_REPEATS);
  }
    


    
}
