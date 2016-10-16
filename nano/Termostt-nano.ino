/* 
 * Slave code
 * (Arduino Nano)
 */
#include <IRremote.h>
#include <SoftEasyTransfer.h>
#include <SoftwareSerial.h>

#define STT_PIN 7

SoftwareSerial mySerial(5, 6);
SoftEasyTransfer ET;

IRsend irsend;

struct RECEIVE_DATA_STRUCTURE {
  byte type;
  byte temperature;
  byte len;
};
RECEIVE_DATA_STRUCTURE IRsignal;

byte codeType;
unsigned long codeValue;
byte codeLen;
int toggle = 0;

unsigned long consul[] = {
  0xC1602040, //on
  0xC1600040, //18
  0xC1600048, //19
  0xC1600044, //20
  0xC160004C, //21
  0xC1600042, //22
  0xC160004A, //23
  0xC1600046, //24
  0xC160004E, //25
  0xC1600041, //26
  0xC1600049, //27
  0xC1600045, //28
  0xC160004D, //29
  0xC1600043, //30
  0xC1602040, //off
};

unsigned long lg[] = {
  0x8800347, //on
  0x880834F, //18
  0x8808440, //19
  0x8808541, //20
  0x8808642, //21
  0x8808743, //22
  0x8808844, //23
  0x8808945, //24
  0x8808A46, //25
  0x8808B47, //26
  0x8808C48, //27
  0x8808D49, //28
  0x8808E4A, //29
  0x8808F4B, //30
  0x88C0051, //off
};

void setup() {
  mySerial.begin(9600);
  ET.begin(details(IRsignal), &mySerial);
  
  pinMode(STT_PIN, OUTPUT);
  digitalWrite(STT_PIN, LOW);
}

void loop() {
  if (ET.receiveData()) {
    codeType  = IRsignal.type;
    codeLen   = IRsignal.len;
    byte t_ac = IRsignal.temperature;
    if (codeType == 3) {
      codeValue = consul[t_ac-17];
    }
    else if (codeType == 10) {
      codeValue = lg[t_ac-17];
    }

    digitalWrite(STT_PIN, HIGH);
    sendCode();
    digitalWrite(STT_PIN, LOW);
  }
  delay(150);
}

// Enviar sinal IR para o emissor (IR LED)
void sendCode() {
  if (codeType == NEC) {
    irsend.sendNEC(codeValue, codeLen);
  }
  else if (codeType == SONY) {
    irsend.sendSony(codeValue, codeLen);
  }
  else if (codeType == RC5 || codeType == RC6) {
    toggle = 1 - toggle;
    // Put the toggle bit into the code to send
    codeValue = codeValue & ~(1 << (codeLen - 1));
    codeValue = codeValue | (toggle << (codeLen - 1));
    if (codeType == RC5) {
      irsend.sendRC5(codeValue, codeLen);
    }
    else {
      irsend.sendRC6(codeValue, codeLen);
    }
  }
  else if (codeType == LG) {
    irsend.sendLG(codeValue, codeLen);
  }
  /*// i.e. raw
  else if (codeType == UNKNOWN) {
    // Assume 38 KHz
    irsend.sendRaw(rawCodes, codeLen, 38);
  }
  */
}

