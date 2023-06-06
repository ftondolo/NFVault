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
byte passwords[1][4][16] =  {{{0x23, 0x5E, 0x2E, 0x6B, 0xAC, 0xA, 0x86, 0x3C, 0x80, 0xCF, 0x50, 0xB3, 0xB7, 0x26, 0x2F, 0x95},
                              {0x32, 0xA2, 0xC9, 0x9C, 0x50, 0x23, 0x61, 0x3D, 0xD1, 0x9E, 0x27, 0xBE, 0x74, 0x3F, 0xF6, 0xCA},
                              {0x5A, 0x39, 0xA6, 0xE7, 0x27, 0x35, 0x24, 0xA1, 0x67, 0x9A, 0x6D, 0x6, 0xB8, 0x9C, 0xE5, 0xA},
                              {0xAE, 0x54, 0x2B, 0xE5, 0xCC, 0x19, 0xA9, 0xE0, 0xB9, 0x34, 0x74, 0x3D, 0x65, 0x99, 0x46, 0x30}}};
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
    uint8_t uid_len = tag.getUidLength();
    tag.getUid(key, uid_len);
    aes128.setKey(key,16);
    for (int x=0; x<1;x++){
      for (int y=0; y<4;y++){
        aes128.decryptBlock(decryptedtext,passwords[x][y]);
        out = String((char*)decryptedtext).substring(0, 16);
        for (int i=0; i<16; i++){
          if (not isAscii(out[i])){
            goto next;
          }
        }
        Serial.print("DECRYPTING: ");
        for(int j=0;j<16;j++){
          Serial.print(decryptedtext[j], HEX);
          if (j != (16)-1){
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
        next:
        continue;
      }
    }
    delay(500);
  }
}
