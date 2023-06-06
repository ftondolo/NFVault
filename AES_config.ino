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
String password = "WHL0137-LS|march"; // REQUIRES MODIFICATION
byte *plaintext = new byte[password.length()+1];
byte *decryptedtext = new byte[password.length()];
byte *cypher = new byte[password.length()];
byte *key = new byte[password.length()];

void setup(void) 
{
  Serial.begin(115200);
  Keyboard.begin();
  nfc.begin();
  password.getBytes(plaintext, password.length()+1);
}
 
void loop() 
{
  if (nfc.tagPresent())
  {
    NfcTag tag = nfc.read();
    uint8_t uid_len = tag.getUidLength();
    tag.getUid(key, uid_len);
    for (int i = 0; i<sizeof(key); i++){
      key[i] = key[i%uid_len];
    }
    aes128.setKey(key,password.length());
    bool verify = true;
    aes128.encryptBlock(cypher,plaintext);
   String out = String((char*)plaintext);
   out = out.substring(0, sizeof(cypher));
   Serial.print("PLAINTEXT: ");
   Serial.println(out);
   Serial.print("CIPHERTEXT: ");
   for(int j=0;j<sizeof(cypher);j++){
      Serial.print("0x");
      Serial.print(cypher[j], HEX);
      if (j != (sizeof(cypher)-1)){
        Serial.print(", ");
      }
    }
    Serial.println();
    Serial.println();
  }
 delay(1000);
}
 
