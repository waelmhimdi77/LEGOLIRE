#include <MFRC522.h> //library responsible for communicating with the module RFID-RC522
#include <SPI.h> //library responsible for communicating of SPI bus
#define NR_OF_READERS   2
#define SS_1_PIN        21         // Configurable, take a unused pin, only HIGH/LOW required, must be different to SS 2
#define SS_2_PIN        8 
byte ssPins[] = {SS_1_PIN, SS_2_PIN};   
#define SS_PIN    21
#define RST_PIN   22
#define SIZE_BUFFER     18
#define MAX_SIZE_BLOCK  16
//used in authentication
MFRC522::MIFARE_Key key;
//authentication return status code
MFRC522::StatusCode status;
// Defined pins to module RC522
MFRC522 mfrc522[NR_OF_READERS];   // Create MFRC522 instance.

byte nuidPICC[NR_OF_READERS][4];
char str [32] = "";
String strs[NR_OF_READERS];
String UID[]={"0D1B64A3","76D1611F", "044267AA"};
String correct[]={"cha", "cal"};
String syllables[] = {"cha","cal", "che"};
String valeur_lu[NR_OF_READERS];
 boolean response[2];
void setup() 
{
  Serial.begin(9600);
 
  SPI.begin(); // Init SPI bus
  test_get_uid();
  // Init MFRC522
  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
    mfrc522[reader].PCD_Init(ssPins[reader], RST_PIN);
   }
  Serial.println("Approach your reader card...");
  Serial.println();
}
  void loop() 
{
  
  if(Serial.available()){
    char lu = Serial.read();
    switch(lu){
      case '0':
        readd();
        get_WORD(strs,valeur_lu, 2);
        Serial.println("");
        for(int i=0; i<2; i++){
          Serial.print(valeur_lu[i]);
          Serial.print("   ");
         }
         compareWords(correct, valeur_lu, response, 2);
         Serial.println("");
        for(int i=0; i<2; i++){
          Serial.print(response[i]);
          Serial.print("   ");
        }
        break;
      default:
        break;
    }
  }
}


void readd(){
  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
    strs[reader]="NA";
    if (mfrc522[reader].PICC_IsNewCardPresent() && mfrc522[reader].PICC_ReadCardSerial()) 
    {
       //Serial.print(F("PICC type: "));
       // MFRC522::PICC_Type piccType = mfrc522[reader].PICC_GetType(mfrc522[reader].uid.sak);
      // Serial.println(mfrc522[reader].PICC_GetTypeName(piccType));
      
          // Store NUID into nuidPICC array
          for (byte i = 0; i < 4; i++) {
            nuidPICC[reader][i] = mfrc522[reader].uid.uidByte[i];
          }
          array_to_string(nuidPICC[reader], 4, str);
          strs[reader] = String(str);
      //instructs the PICC when in the ACTIVE state to go to a "STOP" state
      mfrc522[reader].PICC_HaltA(); 
      // "stop" the encryption of the PCD, it must be called after communication with authentication, otherwise new communications can not be initiated
      mfrc522[reader].PCD_StopCrypto1(); 
    }  
    Serial.println(strs[reader]);
  }  
}



void array_to_string(byte array[], unsigned int len, char buffer[])
{
    for (unsigned int i = 0; i < len; i++)
    {
        byte nib1 = (array[i] >> 4) & 0x0F;
        byte nib2 = (array[i] >> 0) & 0x0F;
        buffer[i*2+0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
        buffer[i*2+1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
    }
    buffer[len*2] = '\0';
}

void get_UID_LIST_FROM_WORD(String words[], String UID_LIST[], int syllabe_size){
  for(int i=0; i<syllabe_size; i++){
    UID_LIST[i]="N/A"; 
    for(int j=0; j<2; j++){
     if(words[i] == syllables[j]){
      UID_LIST[i] = UID[j];
      break;
     }
    }
  }
}
void get_WORD(String UID_LIST[], String words[], int syllabe_size){
  for(int i=0; i<syllabe_size; i++){
    words[i]="N/A"; 
    for(int j=0; j<3; j++){
     if(UID_LIST[i] == UID[j]){
      words[i] = syllables[j];
      break;
     }
    }
  }
}

void compareWord(String getUID[], String UID_WORD[], boolean response[], int len){
  for(int i=0; i<len; i++){
    if(getUID[i] == UID_WORD[i]){
      response[i] = true;
    }
    else{
      response[i] = false;
    }   
  }
}
void compareWords(String correct[], String readed[], boolean response[], int len){
  for(int i=0; i<len; i++){
    if(correct[i] == readed[i]){
      response[i] = true;
    }
    else{
      response[i] = false;
    }   
  }
}
void test_get_uid(){
  String test[] = {"cal", "cha"};
  String test1[] = {"cha", "cal"};
  String test2[] = {"cha", "caux"};
  String test3[] = {"che", "val"};
  String UID_LIST[2];
  String UID_LIST1[2];
  String UID_LIST2[2];
  String UID_LIST3[2];
   get_UID_LIST_FROM_WORD(test, UID_LIST, 2);
  get_UID_LIST_FROM_WORD(test1, UID_LIST1, 2);
  get_UID_LIST_FROM_WORD(test2, UID_LIST2, 2);
  get_UID_LIST_FROM_WORD(test3, UID_LIST3, 2);
   compareWord(UID_LIST1, UID,response,2);
  Serial.println("");
  for(int i=0; i<2; i++){
    Serial.print(UID_LIST[i]);
    Serial.print("   ");
  }
 
   Serial.println("");
  for(int i=0; i<2; i++){
    Serial.print(UID_LIST1[i]);
    Serial.print("   ");
  }
  
   Serial.println("");
  for(int i=0; i<2; i++){
    Serial.print(UID_LIST2[i]);
    Serial.print("   ");
  }
  Serial.println("");
  for(int i=0; i<2; i++){
    Serial.print(UID_LIST3[i]);
     Serial.print("   ");
  }
}

