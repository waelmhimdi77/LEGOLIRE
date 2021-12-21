#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

String MACadd = "FC:A8:9A:00:81:F8";
uint8_t address[6]  = {0xFC, 0xA8, 0x9A, 0x00, 0x81, 0xF8}; //adresse Mac du bluetooth du cube 1
uint8_t address2[6] = {0xFC, 0xA8, 0x9A, 0x00, 0x3F, 0x7B}; //adresse Mac du bluetooth du cube 2

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
String UID[]={"0D1B64A3","5C769C21", "C012AC4C", "F50818C5"}; //liste des différents tags dans les cubes
String correct[2]={"che", "val"}; //variable qui contient à chaque fois les syllabes correctes (initialisé à che-val)
String syllables[] = {"cha","cal", "che","val"}; // liste des syllabes associés à chaque tag
String valeur_lu[NR_OF_READERS]; //variable qui contient la liste des UIDs lus
 boolean response[2]; //variable qui contient le résultat après vérification
 

#include <WiFi.h>
#include <Wire.h>
const char* ssid = "pi";
const char* password =  "123456789";

void setup() 
{

  SerialBT.begin("ESP32test", true); //initialiser le bluetooth de l'esp avec le nom ESP32
  Serial.begin(115200);
  pinMode(BUTTON, INPUT_PULLUP);  //initialiser le bouton de validation
  SPI.begin(); // Init SPI bus
  
  // Init MFRC522
  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
    mfrc522[reader].PCD_Init(ssPins[reader], RST_PIN);
   }
  Serial.println("Approach your reader card...");
  Serial.println();
}


void loop() 
{
  user2(); //fonction principale qui se repète en boucle
}


void readd(){ //fonction permettant juste de lire les UIDs des différents tags posés
  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
    strs[reader]="NA";
    if (mfrc522[reader].PICC_IsNewCardPresent() && mfrc522[reader].PICC_ReadCardSerial()) 
    {
          // Store NUID into nuidPICC array
          for (byte i = 0; i < 4; i++) {
            nuidPICC[reader][i] = mfrc522[reader].uid.uidByte[i];
          }
          array_to_string(nuidPICC[reader], 4, str);
          strs[reader] = String(str); //variable qui contient la liste des tags lus
      //instructs the PICC when in the ACTIVE state to go to a "STOP" state
      mfrc522[reader].PICC_HaltA(); 
      // "stop" the encryption of the PCD, it must be called after communication with authentication, otherwise new communications can not be initiated
      mfrc522[reader].PCD_StopCrypto1(); 
    }  
  }  
}



void array_to_string(byte array[], unsigned int len, char buffer[])
{ //fonction permettant de convertir un tableau de bytes en tableau de carractèrs
    for (unsigned int i = 0; i < len; i++)
    {
        byte nib1 = (array[i] >> 4) & 0x0F;
        byte nib2 = (array[i] >> 0) & 0x0F;
        buffer[i*2+0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
        buffer[i*2+1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
    }
    buffer[len*2] = '\0';
}


void get_WORD(String UID_LIST[], String words[], int syllabe_size){ //fonction permettant de déterminer la liste des syllabes lues à parti des UIDs lus
  for(int i=0; i<syllabe_size; i++){
    words[i]="N/A"; //variable qui contiendra la liste des syllabes lues (si l'UID n'est pas reconnu, la syllabe serait alors égale à N/A)
    for(int j=0; j<4; j++){
     if(UID_LIST[i] == UID[j]){
      words[i] = syllables[j];
      break;
     }
    }
  }
}


void compareWords(String correct[], String readed[], boolean response[], int len){ //fonction permettant de comparer la liste des syllabes lu et la liste des syllabes correctes
  for(int i=0; i<len; i++){
    if(correct[i] == readed[i]){
      response[i] = true;
    }
    else{
      response[i] = false;
    }   
  }
}


void user2(){ //fonction principale
  String words[11] = {"ar bre", "ba teau", "che val", "co chon", "li vre", "lu ne", "mou ton", "por te", "sou ris", "sty lo", "va che"}; //liste des mots que peut choisir l'enseignante
  Serial.println("LISTES DES MOTS PREENREGISTRES");
  for(int i=0; i<11; i++){ //boucle pour afficher la liste des mots sur le moniteur série
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
   int nbre_mot = Serial.parseInt(); //variable qui contient le nombre de mots choisis
   String liste_mot[nbre_mot]; //variable qui contiendra la liste des mots choisis
   Serial.println(String(nbre_mot));
   Serial.println("ENTREZ LES NUMEROS DES MOTS");
   for(int i=0; i<nbre_mot; i++){ //boucle pour choisir les identifiants des mots par l'enseignante
    Serial.println("mot " + String(i+1)+ " ");
    while(!Serial.available()){} 
    int mot = Serial.parseInt();
    liste_mot[i] = words[mot-1];
   }

  for(int i=0; i<(nbre_mot); i++){ //boucle pour afficher la liste des mots choisis par l'enseignante
    String mo = "";
    for(int j=0; j<liste_mot[i].length(); j++){ 
      if(liste_mot[i][j] != ' ')
      {
        mo+= liste_mot[i][j];
      }
    }
    Serial.println(String(i+1)+ " -------------------->" + mo);
  }
  
 Serial.println(" **************************************************************************");
Serial.println(" COMMENCEZ LA PARTIE");

int h=0;
envoi_vers_cube(liste_mot[0]); //envoie le premier mot sur les cubes
  while(h <nbre_mot){ //boucle pour envoyer la liste de mots choisis 1 par 1
     Serial.println(" retrouvez le mot -------------------->" +String(h+1));
    for(int i=0; i<2; i++){
       correct[i] =""; //initialiser la variable qui contiendra le mot que l'enfant doit retrouver
    }
    int n=0;
    for(int j=0; j<liste_mot[h].length(); j++){ //boucle pour affecter les syllabes du mots que l'enfant doit maintenant retrouver
      if(liste_mot[h][j] != ' ')
      {
        correct[n] += liste_mot[h][j];
      }
      else{
        n++;
      }
    }
    
    while(!PRESSED_BUTTON()){} //attendre que l'enfant appuie sur le bouton de validation
        readd(); //lorsque bouton appuyé, lire les UIDs des tags
        get_WORD(strs,valeur_lu, 2); //déterminer à partir des tags, les syllabes correspondantes
         compareWords(correct, valeur_lu, response, 2); //comparer les syllabes lus et les syllabes correctes
     
        if((response[0] == response[1]) && response[0]==1){ //si le mot écrit par l'enfant est correcte
           h++; //passer au mot suivant dans la liste des mots choisis par l'enseignante
           backdroung(String(1), String(1)); //mettre le fonds des écrans en vert
           delay(3000); //attendre 3 secondes avant d'envoyer le deuxième mot sur les afficheurs
          if(h<nbre_mot){ //si on n'a pas encore fini tous les mots de la liste choisis
            envoi_vers_cube(liste_mot[h]); //envoie maintenant le mot suivant sur les afficheurs
          } 
          else{ //si on a fini tous les mots de la liste choisis par l'enseignante
            envoi_BRAVO(); //ecrire bravo sur les afficheurs
            backdroung(String(1), String(1)); //mettre le fond des afficheurs en vert
            delay(5000);
          }  
        }
        else{ //si le mot écrit par l'enfant est incorrecte
          backdroung(String(response[num_cube]), String(response[1-num_cube])); //mettre le fond des afficheurs du cube incorrecte en rouge et celui du cube correcte en vert
        }
       
    } 
    
    Serial.println("FINI"); //écrire FINI dans le moniteur série lorsque l'enfant à trouver tous les mots choisis (finit l'exercice)
   
  }  

void envoi_vers_cube(String mots){ //fonction permettant d'écrire les syylabes correctes et incorrectes sur les cubes. prend en paramètre le mot à écrire
      num_cube = random(0,2); //choisir aléatoirement le cube qui va contenir la première syllabe
      int pos_cube1 = random(0,2); //choisir aléatoirement la face qui contient la syllabe correcte pour chaque cube
      int pos_cube2 = random(0,2);
      int pose_cube[2] = {pos_cube1,  pos_cube2};
      String syllabe_c[2] = {"",""}; //variable qui contiendra les 2 syllabes correctes
      String syllabe_i[2] = {"",""}; //variable qui contiendra les 2 syllabes incorrectes
       String syllabe[2] = {"",""}; //variables qui contiendra pour chaque cube, les deux syllabes qui doivent s'y afficher ainsi que le numero de l'afficheur qui doit écrire la syllabe correcte
      int nb = 0;
      for(int i=0; i<mots.length(); i++){ //boucle pour découper le mot en deux syllabes
        if(mots[i] == ' '){
          syllabe_c[nb] =  syllabe[nb];
          syllabe[nb]+= " " + String(pose_cube[nb]);
          nb++;
        } else{
          syllabe[nb]+= mots[i];
        }    
      }
      syllabe_c[nb] =  syllabe[nb];
      syllabe[nb]+= " " + String(pose_cube[nb]);
    
      for(int i=0; i<2; i++){ //boucle pour former les syllabes incorreces
        String syl = syllabe_c[i];
        for(int j=0; j<syl.length(); j++){
          for(int k=0; k<6; k++){
            if(syl[j] == voyelle[k]){ //on recherche la voyelle dans la syllabes correcte et on la change par une autre voyelle
              syl[j] = voyelle[((k+1)%4)];
              break;
            }
          }
         }
         syllabe_i[i] = syl;
         syllabe[i]+= " " + syl;
      }
    
       if(num_cube==0){ //affecter les nouvelles syllabes à chaque tag 
           syllables[pos_cube1] = syllabe_c[0];  
           syllables[1-pos_cube1] = syllabe_i[0];
            syllables[2+pos_cube2] = syllabe_c[1];
            syllables[2+(1-pos_cube2)] = syllabe_i[1];
       }
       if(num_cube==1){ //affecter les nouvelles syllabes à chaque tag 
        syllables[pos_cube2] = syllabe_c[1];  
        syllables[1-pos_cube2] = syllabe_i[1];
        syllables[2+pos_cube1] = syllabe_c[0];
        syllables[2+(1-pos_cube1)] = syllabe_i[0];
       }

      
      char message1[syllabe[num_cube].length()+1];
      syllabe[num_cube].toCharArray(message1, (syllabe[num_cube].length()+1)); //convertir le string en tablau de char pour l'envoyer au cube
      char message2[syllabe[1-num_cube].length()+1];
      syllabe[1-num_cube].toCharArray(message2, (syllabe[1-num_cube].length()+1)); //convertir le string en tablau de char pour l'envoyer au cube
       Serial.println("***************************************************************************");
      delay(1000);
      connected = SerialBT.connect(address2); //conecte la base au bluetooth du cube 1
      while(!connected)
      {
        delay(500);
        connected = SerialBT.connect(address2);
      }
      if(connected) {
        SerialBT.print( message1); //envoi les syllabes à afficher sur les afficheurs
         if (SerialBT.disconnect()) { //deconnecte le bluetooh
          Serial.println("Disconnected  1 Succesfully!");
        } 
      }

      delay(1000);
      connected = SerialBT.connect(address); //conecte la base au bluetooth du cube 1
      while(!connected)
      {
        delay(500);
        connected = SerialBT.connect(address);
      }
      if(connected) {
        SerialBT.print( message2); //envoi les syllabes à afficher sur les afficheurs
         if (SerialBT.disconnect()) { //deconnecte le bluetooh
            Serial.println("Disconnected  2 Succesfully!");
        }
      } 
}

boolean PRESSED_BUTTON(){ //fonction pour determiner sur le bouton de validation est appuyé ou pas
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

void backdroung(String cube1, String cube2){ //fonction pour changer la couleur de fond des afficheurs
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
        } 
      }

      delay(1000);
      connected = SerialBT.connect(address);
      if(connected) {
        Serial.println("Connected  2 Succesfully!");
        SerialBT.print( message2);
         if (SerialBT.disconnect()) {
            Serial.println("Disconnected  2 Succesfully!");
        }
      } 
}

void envoi_BRAVO(){ //fonction pour écrive bravo sur les afficheurs
       connected = SerialBT.connect(address2);
      if(connected) {
        Serial.println("Connected  1 Succesfully!");
        SerialBT.print("BRA 1 VO");
         if (SerialBT.disconnect()) {
          Serial.println("Disconnected  1 Succesfully!");
        } 
      }

      delay(1000);
      connected = SerialBT.connect(address);
      if(connected) {
        Serial.println("Connected  2 Succesfully!");
        SerialBT.print("BRA 1 VO");
         if (SerialBT.disconnect()) {
            Serial.println("Disconnected  2 Succesfully!");
        }
      } 
}

