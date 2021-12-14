//#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
//#include "http_server.h"
#include "ArduinoJson.h"
#include "BluetoothSerial.h"
#include <HardwareSerial.h>
#include <WiFi.h>
BluetoothSerial SerialBT;

uint8_t address[6]  = {0xFC, 0xA8, 0x9A, 0x00, 0x81, 0xF8};
uint8_t address2[6] = {0xFC, 0xA8, 0x9A, 0x00, 0x3F, 0x7B};

bool connected;
bool connected2;
String envoi ="";
boolean appui = false;
int num_cube;
#include <MFRC522.h> //library responsible for communicating with the module RFID-RC522
#include <SPI.h> //library responsible for communicating of SPI bus
#define NR_OF_READERS   2
#define SS_1_PIN        21         // Configurable, take a unused pin, only HIGH/LOW required, must be different to SS 2
#define SS_2_PIN        17 
#define BUTTON       15 
byte ssPins[] = {SS_1_PIN, SS_2_PIN};   
#define SS_PIN    21
#define RST_PIN   22
#define SIZE_BUFFER     18
#define MAX_SIZE_BLOCK  16
char voyelle[6] = {'a', 'e', 'i', 'o', 'u','y'};
//used in authentication
MFRC522::MIFARE_Key key;
//authentication return status code
MFRC522::StatusCode status;
// Defined pins to module RC522
MFRC522 mfrc522[NR_OF_READERS];   // Create MFRC522 instance.

byte nuidPICC[NR_OF_READERS][4];
char str [32] = "";
String strs[NR_OF_READERS];
String UID[]={"0D1B64A3","5C769C21", "C012AC4C", "F50818C5"};
String correct[2]={"che", "val"};
String syllables[] = {"cha","cal", "che","val"};
String valeur_lu[NR_OF_READERS];
boolean response[2];
 
const char *ssid = "myhuawei";
const char *password = "koffi123";
String recus;
const int led = 2;
const int capteurLuminosite = 34;
String exercice;
AsyncWebServer server(80);

void setup()
{
  SerialBT.begin("ESP32test", true); 
  Serial.begin(115200);
  pinMode(BUTTON, INPUT_PULLUP);  
  SPI.begin(); // Init SPI bus
  
  // Init MFRC522
  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
    mfrc522[reader].PCD_Init(ssPins[reader], RST_PIN);
   }
  Serial.println("Approach your reader card...");
  Serial.println();
  //----------------------------------------------------Serial
 
  Serial.println("\n");

  //----------------------------------------------------GPIO
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  pinMode(capteurLuminosite, INPUT);

  //----------------------------------------------------SPIFFS
  if(!SPIFFS.begin())
  {
    Serial.println("Erreur SPIFFS...");
    return;
  }

  File root = SPIFFS.open("/");
  File file = root.openNextFile();

  while(file)
  {
    Serial.print("File: ");
    Serial.println(file.name());
    file.close();
    file = root.openNextFile();
  }

  //----------------------------------------------------WIFI
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);
  Serial.print("Tentative de connexion...");
  
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
  }
  
  Serial.println("\n");
  Serial.println("Connexion etablie!");
  Serial.print("Adresse IP: ");
  Serial.println(WiFi.localIP());

  //----------------------------------------------------SERVER
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    int paramsNr = request->params();
    Serial.println(paramsNr);
    for(int i=0;i<paramsNr;i++){
        AsyncWebParameter* p = request->getParam(i);
        Serial.print("Param name: ");
        Serial.println(p->name());
        Serial.print("Param value: ");
        Serial.println(p->value());
        Serial.println("------");
    }
    request->send(SPIFFS, "/index.html", "text/html");
  });
  
  server.on("/w3.css", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/w3.css", "text/css");
  });
  server.on("/logo.png", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/logo.png");
  });
  server.on("/kids.jpg", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/kids.jpg");
  });
  server.on("/solution.jpg", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/solution.jpg");
  });
  server.on("/vache.png", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/vache.png");
  });
  server.on("/chacal.png", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/chacal.png");
  });
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/script.js", "text/javascript");
  });


  server.on("/lireLuminosite", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    int val = analogRead(capteurLuminosite);
    String luminosite = String(val);
    request->send(200, "text/plain", luminosite);
  });

  server.on("/save",
        HTTP_POST,
        [](AsyncWebServerRequest * request){},
        NULL,
        [](AsyncWebServerRequest * request, uint8_t *data, size_t len,
        size_t index, size_t total) {
            Serial.println("POST RECEIVED"); // Just for debug
            StaticJsonBuffer<50> JSONBuffer; // create a buffer that fits for you
            JsonObject& parsed = JSONBuffer.parseObject(data); //Parse message
            const char* received_data = parsed["words"]; //Get data
            Serial.println(received_data);
            exercice = String(received_data);
            user2(exercice);
            request->send(200);
    });


  server.begin();
  Serial.println("Serveur actif!");
}

void loop()
{

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
    for(int j=0; j<4; j++){
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
    for(int j=0; j<4; j++){
     if(UID_LIST[i] == UID[j]){
      words[i] = syllables[j];
      break;
     }
    }
  }
}
void envoieMsg ( String msg, String UID){
  
  
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

void user2(String got){
  String words[11] = {"ar bre", "ba teau", "che val", "co chon", "li vre", "lu ne", "mou ton", "por te", "sou ris", "sty lo", "va che"};
  /*Serial.println("LISTES DES MOTS PREENREGISTRES");
  for(int i=0; i<11; i++){
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
  }*/
      //WiFi.disconnect();
      //delay(5000);
     /* while(WiFi.status()== WL_CONNECTED)
      {
        Serial.print(".");
        delay(100);
      }*/
      delay(100);
      String wordss[11] = {"Arbre", "Bateau", "Cheval", "Cochon", "Livre", "Lune", "Mouton", "Porte", "Souris", "Stylo", "Vache"};
      int nbre_mot_recus = 0;
      for(int i=0; i<got.length(); i++){
        if(got[i]==','){
          nbre_mot_recus++;
        }
      }
      nbre_mot_recus++;
      Serial.println(String( nbre_mot_recus));
      String listmot[nbre_mot_recus];
      for(int i=0; i<nbre_mot_recus; i++){
        listmot[i]=="";
      }
      int id_list[(nbre_mot_recus/2)];
      int n=0;
      for(int i=0; i<got.length(); i++){
        if(got[i]==','){
           Serial.println(listmot[n]);
          n++;
        }
        else{
          listmot[n]+= got[i];
        }
      }
      Serial.println(listmot[n]);
      int z=0;
      for(int i=0; i<nbre_mot_recus; i+=2){
        for(int j=0; j<11; j++){
        if((listmot[i]==wordss[j])){
          id_list[z] = j;
          Serial.println(String(id_list[z]));
          z++;
          }
        }
      }

   
      String liste_mot[nbre_mot_recus/2];
      for(int i=0; i<nbre_mot_recus/2; i++){
        liste_mot[i] = words[id_list[i]];
        Serial.println(liste_mot[i]);
      }
    Serial.println(" **************************************************************************");
    Serial.println(" COMMENCEZ LA PARTIE");
int nombre = nbre_mot_recus/2;
int h=0;
envoi_vers_cube(liste_mot[0]);
  while(h <nombre){
     Serial.println(" retrouvez le mot -------------------->" +String(h+1));
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
    for(int j=0; j<2; j++){
      Serial.println(correct[j]);
    }
    while(!PRESSED_BUTTON()){}
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
          //envoi_vers_cube("1");
           h++; 
           backdroung(String(1), String(1));
           delay(3000);
          if(h<nombre){
            envoi_vers_cube(liste_mot[h]);
          } 
          else{
            envoi_BRAVO();
            backdroung(String(1), String(1));
            delay(5000);
          }  
        }
        else{
          backdroung(String(response[num_cube]), String(response[1-num_cube]));
        }
       
    } 
    
    Serial.println("FINI");
   
  }  

void envoi_vers_cube(String mots){
      num_cube = random(0,2);
      int pos_cube1 = random(0,2);
      int pos_cube2 = random(0,2);
      int pose_cube[2] = {pos_cube1,  pos_cube2};
      String syllabe_c[2] = {"",""};
      String syllabe_i[2] = {"",""};
       String syllabe[2] = {"",""};
      int nb = 0;
      for(int i=0; i<mots.length(); i++){
        if(mots[i] == ' '){
          syllabe_c[nb] =  syllabe[nb];
          Serial.println(syllabe_c[nb]);
          syllabe[nb]+= " " + String(pose_cube[nb]);
          nb++;
        } else{
          syllabe[nb]+= mots[i];
        }    
      }
      syllabe_c[nb] =  syllabe[nb];
      syllabe[nb]+= " " + String(pose_cube[nb]);
      Serial.println(syllabe_c[nb]);

      for(int i=0; i<2; i++){
        String syl = syllabe_c[i];
        for(int j=0; j<syl.length(); j++){
          for(int k=0; k<6; k++){
            if(syl[j] == voyelle[k]){
              syl[j] = voyelle[((k+1)%4)];
              break;
            }
          }
         }
         syllabe_i[i] = syl;
         //Serial.println(syl);
         Serial.println(syllabe_i[i]);
         syllabe[i]+= " " + syl;
      }
    
       if(num_cube==0){
           syllables[pos_cube1] = syllabe_c[0];  
           syllables[1-pos_cube1] = syllabe_i[0];
            syllables[2+pos_cube2] = syllabe_c[1];
            syllables[2+(1-pos_cube2)] = syllabe_i[1];
       }
       if(num_cube==1){
        syllables[pos_cube2] = syllabe_c[1];  
        syllables[1-pos_cube2] = syllabe_i[1];
        syllables[2+pos_cube1] = syllabe_c[0];
        syllables[2+(1-pos_cube1)] = syllabe_i[0];
       }
       
       
       for(int i=0; i<4; i++){
        Serial.println("syllable : " + syllables[i]);
       }

       Serial.println(syllabe[0]);
       Serial.println(syllabe[1]);
      char message1[syllabe[num_cube].length()+1];
      syllabe[num_cube].toCharArray(message1, (syllabe[num_cube].length()+1));
      char message2[syllabe[1-num_cube].length()+1];
      syllabe[1-num_cube].toCharArray(message2, (syllabe[1-num_cube].length()+1));
       delay(500);
       Serial.println("***************************************************************************");
       
       Serial.println(message1);
        Serial.println(message2);
         for(int i=0; i<4; i++){
        Serial.println("syllable : " + syllables[i]);
        }
      delay(1000);
      connected = SerialBT.connect(address2);
      while(!connected)
      {
        delay(500);
        connected = SerialBT.connect(address2);
      }
      if(connected) {
        Serial.println("Connected  1 Succesfully!");
        SerialBT.print( message1);
         if (SerialBT.disconnect()) {
          Serial.println("Disconnected  1 Succesfully!");
         //connected = !connected;
        } 
      }

      delay(1000);
      connected = SerialBT.connect(address);
      while(!connected)
      {
        delay(500);
        connected = SerialBT.connect(address);
      }
      if(connected) {
        Serial.println("Connected  2 Succesfully!");
        SerialBT.print( message2);
         if (SerialBT.disconnect()) {
            Serial.println("Disconnected  2 Succesfully!");
            //connected = !connected;
        }
      } 
}

boolean PRESSED_BUTTON(){
  boolean sortie= false;
   if(!(digitalRead(BUTTON)) && (appui==false)){
    appui=true;
    sortie = true;
  }
  if(digitalRead(BUTTON)){
    appui = false;
    delay(200);
  }

  return sortie;
}

void backdroung(String cube1, String cube2){
      char message1[cube1.length()+1];
      cube1.toCharArray(message1, (cube1.length()+1));
      char message2[cube2.length()+1];
      cube2.toCharArray(message2, (cube2.length()+1));
       delay(500);
      connected = SerialBT.connect(address2);
      if(connected) {
        Serial.println("Connected  1 Succesfully!");
        SerialBT.print( message1);
         if (SerialBT.disconnect()) {
          Serial.println("Disconnected  1 Succesfully!");
         //connected = !connected;
        } 
      }

      delay(1000);
      connected = SerialBT.connect(address);
      if(connected) {
        Serial.println("Connected  2 Succesfully!");
        SerialBT.print( message2);
         if (SerialBT.disconnect()) {
            Serial.println("Disconnected  2 Succesfully!");
            //connected = !connected;
        }
      } 
}

void envoi_BRAVO(){
       connected = SerialBT.connect(address2);
      if(connected) {
        Serial.println("Connected  1 Succesfully!");
        SerialBT.print("BRA 1 VO");
         if (SerialBT.disconnect()) {
          Serial.println("Disconnected  1 Succesfully!");
         //connected = !connected;
        } 
      }

      delay(1000);
      connected = SerialBT.connect(address);
      if(connected) {
        Serial.println("Connected  2 Succesfully!");
        SerialBT.print("BRA 1 VO");
         if (SerialBT.disconnect()) {
            Serial.println("Disconnected  2 Succesfully!");
            //connected = !connected;
        }
      } 
}

void getwords(String got){
 
}
