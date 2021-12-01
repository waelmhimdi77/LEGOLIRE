#include <MFRC522.h> //library responsible for communicating with the module RFID-RC522
#include <SPI.h> //library responsible for communicating of SPI bus
#define NR_OF_READERS   2
#define SS_1_PIN        21         // Configurable, take a unused pin, only HIGH/LOW required, must be different to SS 2
#define SS_2_PIN        17 
#define BUTTON       4 
byte ssPins[] = {SS_1_PIN, SS_2_PIN};   
#define SS_PIN    21
#define RST_PIN   22
#define SIZE_BUFFER     18
#define MAX_SIZE_BLOCK  16
boolean PRESS_BUTTON = false;
//used in authentication
MFRC522::MIFARE_Key key;
//authentication return status code
MFRC522::StatusCode status;
// Defined pins to module RC522
MFRC522 mfrc522[NR_OF_READERS];   // Create MFRC522 instance.

byte nuidPICC[NR_OF_READERS][4];
char str [32] = "";
String strs[NR_OF_READERS];
String UID[]={"0D1B64A3","76D1611F", "044267AA","04292442", "048475BA"};
String correct[2]={"che", "val"};
String syllables[] = {"cha","cal", "che","val", "leur"};
String valeur_lu[NR_OF_READERS];
 boolean response[2];
 void IRAM_ATTR isr(){
         Serial.println("PUSH");
         PRESS_BUTTON = true;
         delay(500);
}

#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
const char* ssid = "pi";
const char* password =  "123456789";
const char* mqtt_server = "broker.mqttdashboard.com";
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
void setup() 
{
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  pinMode(BUTTON, INPUT_PULLUP);
  attachInterrupt(BUTTON, isr, FALLING);
  
  SPI.begin(); // Init SPI bus
 user1();
  // Init MFRC522
  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
    mfrc522[reader].PCD_Init(ssPins[reader], RST_PIN);
   }
  Serial.println("Approach your reader card...");
  Serial.println();

  user2();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("clientId-JAoUu11GeC")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("syllabe1");
      client.subscribe("syllabe2");    
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte* message, unsigned int length) {
  for(int i=0; i<2; i++){
    correct[i]="";
  }
    Serial.print("Message arrived on topic: ");
    Serial.print(topic);
    Serial.print(". Message: ");
    String messageTemp; 
    int t=0  ;
    for (int i = 0; i < length; i++) {
      if((char)message[i] != ' '){
        correct[t]+= (char)message[i];
      }
      else{
        
        t++;
      }
      Serial.print((char)message[i]);
      messageTemp += (char)message[i];
    }
    Serial.println( messageTemp);
    
     if(String(topic) == "syllabe1") {
      for(int i=0; i<2; i++)
          Serial.println("syllabe  : "  + correct[i]);

     }
  }
  void loop() 
{
 /* if (!client.connected()) {
    reconnect();
  }*/
 // client.loop();
  
  if(PRESS_BUTTON){
    Serial.println("read");
    PRESS_BUTTON = false;
  }
  if(Serial.available()){
    char lu = Serial.read();
    switch(lu){
      case '0':
        readd();
        get_WORD(strs,valeur_lu, 2);
        Serial.println("");
        for(int i=0; i<2; i++){
          Serial.print("UID " + i + strs[i]);
          Serial.print(" ----------> "+valeur_lu[i]);
          Serial.println("");
         }
         compareWords(correct, valeur_lu, response, 2);
         Serial.println("");
        for(int i=0; i<2; i++){
          Serial.print("Syllabe " + String(i) + " -----------> ");
          Serial.println(response[i]);
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
    //Serial.println("UID " + reader + strs[reader]);
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
    for(int j=0; j<5; j++){
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
    for(int j=0; j<5; j++){
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
void user1(){
  String syllab[] = {"che"};
  String mot[] = {"cha", "cal"};
  String mot2[] = {"che", "val"};
  String UID_LIST[1];
  String UID_LIST1[2];
  String UID_LIST2[2];
   get_UID_LIST_FROM_WORD(syllab, UID_LIST, 1);
   get_UID_LIST_FROM_WORD(mot, UID_LIST1, 2);
   get_UID_LIST_FROM_WORD(mot2, UID_LIST2, 2);
  
  Serial.println("");
  for(int i=0; i<1; i++){
    Serial.println("Syllabe :" + syllab[i]+ " ----> UID : "+ UID_LIST[i]);
    Serial.println("");
  }
 
   Serial.println("");
  for(int i=0; i<2; i++){
    Serial.println("Syllabe :" + mot[i]+ " ----> UID : "+ UID_LIST1[i]);
  }
  Serial.println("---------------------------------------------------------------");
   Serial.println("");
  for(int i=0; i<2; i++){
    Serial.println("Syllabe :" + mot2[i]+ " ----> UID : "+ UID_LIST2[i]);
  }
  Serial.println("-------------------------------------------------------------");
}

void user2(){
  String words[10] = {"che val", "ra me", "chè vre", "vé lo", "va che", "ru che", "é pée", "chê ne", "ca deau", "gâ teau"};
  Serial.println("LISTES DES MOTS PREENREGISTRES");
  for(int i=0; i<10; i++){
    String mo = "";
    for(int j=0; j< words[i].length(); j++){ 
      if(words[i][j] != ' ')
      {
        mo+= words[i][j];
      }
    }
    Serial.println(String(i+1)+ " -------------------->" + mo);
  }
   Serial.println("--------------------------------------------------");
   Serial.print("COMBIEN DE MOTS VOULEZ VOUS CHOISIR _ ");
   while(!Serial.available()){} 
   int nbre_mot = Serial.parseInt();
   String liste_mot[nbre_mot];
   Serial.println(String(nbre_mot));
   Serial.println("ENTREZ LES NUMEROS DES MOTS");
   for(int i=0; i<nbre_mot; i++){
    Serial.println("mot " + String(i+1)+ " ");
    while(!Serial.available()){} 
    int mot = Serial.parseInt();
    liste_mot[i] = words[mot-1];
   }

  for(int i=0; i<(nbre_mot); i++){
    String mo = "";
    for(int j=0; j<liste_mot[i].length(); j++){ 
      if(liste_mot[i][j] != ' ')
      {
        mo+= liste_mot[i][j];
      }
    }
    Serial.println(String(i+1)+ " -------------------->" + mo);
  }

int h=0;
  while(h <nbre_mot){
    for(int i=0; i<2; i++){
       correct[i] ="";
    }
    int n=0;
    for(int j=0; j<liste_mot[h].length(); j++){ 
      if(liste_mot[h][j] != ' ')
      {
        correct[n] += liste_mot[h][j];
      }
      else{
        n++;
      }
    }

    while(!Serial.available()){} 
  
    char lu = Serial.read();
    switch(lu){
      case '0':
        readd();
        get_WORD(strs,valeur_lu, 2);
        Serial.println("");
        for(int i=0; i<2; i++){
          Serial.print("UID " + i + strs[i]);
          Serial.print(" ----------> "+valeur_lu[i]);
          Serial.println("");
         }
         compareWords(correct, valeur_lu, response, 2);
         Serial.println("");
        for(int i=0; i<2; i++){
          Serial.print("Syllabe " + String(i) + " -----------> ");
          Serial.println(response[i]);
        }
        if((response[0] == response[1]) && response[0]==1){
          h++; 
        }
        break;
      default:
        break;
    }
  }
}



