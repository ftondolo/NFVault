#include <PN532_I2C.h>
#include <NfcAdapter.h>
#include <Keyboard.h>
#include <AES.h>

// NFC Initializations
PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);
byte nuidPICC[4];

// AES Initializations
AES128 aes128;
byte key[16];
byte passwords[1][4][16] =  {{{0x3C, 0x03, 0x8D, 0x8C, 0x16, 0x7E, 0xAC, 0xB4, 0x59, 0xFB, 0xC5, 0x71, 0x6D, 0x5A, 0xC3, 0x38},
                              {0x6D, 0xE7, 0xF9, 0xB6, 0xFC, 0xE6, 0x0, 0x98, 0xC3, 0x30, 0xD9, 0x5F, 0x25, 0x40, 0xD8, 0x5C},
                              {0xA7, 0x0, 0x18, 0x5C, 0x4B, 0xEB, 0x92, 0xD3, 0x5F, 0x32, 0x6C, 0xDE, 0x6, 0x2D, 0x78, 0x59},
                              {0xD, 0x13, 0x65, 0x75, 0x53, 0xBB, 0xDB, 0xA0, 0x42, 0xBA, 0x6A, 0xC, 0xC5, 0x77, 0xAC, 0x5E}}};
byte decryptedtext[16];
String out;

int is_locked[1] = {1};
long int last_login[1] = {0};

void setup(void) {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Keyboard.begin();
  nfc.begin();
}
 
void loop() {
  if (nfc.tagPresent()){
    NfcTag tag = nfc.read();
    tag.getUid(key, 7);
    for (int i = 0; i<7; i++){
      key[i+6] = key[i];
    }
    aes128.setKey(key,16);
    Serial.print("DECRYPTING: ");
    for (int x=0; x<sizeof(passwords);x++){
      for (int y=0; x<sizeof(passwords[x]);y++){
        aes128.decryptBlock(decryptedtext,passwords[x][y]);
        out = String((char*)decryptedtext).substring(0, 16);
        for (int i=0; i<sizeof(out); i++){
          if (not isAscii(out[i])){
            if (y == sizeof(passwords[x])-1){
              Serial.println("INVALID KEY");
              goto bail;
            }
            else{
              continue;
            }
          }
        }
        for(int j=0;j<sizeof(decryptedtext);j++){
          Serial.print(decryptedtext[j], HEX);
          if (j != (sizeof(decryptedtext)-1)){
            Serial.print(" ");
          }
        }
        Serial.print(" |-----> ");
        Serial.println(out);
        if (is_locked[x] == 1){
          digitalWrite(LED_BUILTIN, HIGH);
          Serial.print("LOGGING IN: (");
          Keyboard.println();
          delay(500);
          Keyboard.println(out);
          is_locked[x] = 0;
          last_login[x] = millis();
          Serial.print(last_login[x]/1000);
          Serial.println(")");
        }
        else{
          digitalWrite(LED_BUILTIN, LOW);
          Serial.println("LOGGING OUT");
          is_locked[x] = 1;
          Keyboard.press(KEY_LEFT_CTRL);
          Keyboard.press(KEY_LEFT_GUI);
          Keyboard.print("q");
          Keyboard.releaseAll();
        }
        goto bail;
      }
    }
    bail:
      Serial.println();
      delay(500);
  }
}
