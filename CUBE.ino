#include <TFT.h>
#include <SoftwareSerial.h> //librairie pour la liaison série
#include <SPI.h>
//PIN-Einstellungen
#define cs 10 
#define dc 9  
#define rst 8
String lu = "";
String lus[3] = {"", "",""};
char c= ' ';

TFT TFTscreen = TFT(cs, dc, rst); //définir les afficheurs
TFT TFTscreen2 = TFT(7, 6, 3);
void setup() {
  Serial.begin(9600); //initialiser la liaison série
  TFTscreen.begin(); //initialiser les afficheurs
  TFTscreen2.begin();
  //Hintergrund: Schwarz
  TFTscreen.background(255, 255, 255);TFTscreen2.background(255, 255, 255);
  //Textfarbe: Weiß
  TFTscreen.stroke(0, 0, 2); TFTscreen2.stroke(0, 0, 2);
  Serial.println("Arduino avec HC-0x");
}

void loop() {


  //retrieves the chosen option
  if(Serial.available()){ //si le cube réçoit une information
   lu = Serial.readString(); //lit l'information réçue
   
   if(lu=="1"){ //si l'info lu est "1" alors met le fond des afficheurs en vert
      char correct[lus[0].length()+1];
      char incorrect[lus[2].length()+1];
      int val = lus[1].toInt();
      lus[0].toCharArray(correct, (lus[0].length()+1));
      lus[2].toCharArray(incorrect, (lus[2].length()+1));
      char* syllabes[2] = {correct, incorrect};
      
       TFTscreen.background(10, 255, 5);
       TFTscreen2.background(10, 255, 5);
      TFTscreen.stroke(0, 0, 2);
      TFTscreen2.stroke(0, 0, 2);
      TFTscreen.setTextSize(6);
      TFTscreen2.setTextSize(6);
      TFTscreen.text(syllabes[val], 30, 45);
      TFTscreen2.text(syllabes[1-val], 30, 45);
   }
   else if(lu=="0"){ //si l'info lu est "1" alors met le fond des afficheurs en rouge
     char correct[lus[0].length()+1];
      char incorrect[lus[2].length()+1];
      int val = lus[1].toInt();
      lus[0].toCharArray(correct, (lus[0].length()+1));
      lus[2].toCharArray(incorrect, (lus[2].length()+1));
      char* syllabes[2] = {correct, incorrect};
      
       TFTscreen.background(255, 5, 5);
       TFTscreen2.background(255, 5, 5);
      TFTscreen.stroke(0, 0, 2);
      TFTscreen2.stroke(0, 0, 2);
      TFTscreen.setTextSize(6);
      TFTscreen2.setTextSize(6);
      TFTscreen.text(syllabes[val], 30, 45);
      TFTscreen2.text(syllabes[1-val], 30, 45);
   }
   else{ //si le cube reçoit un message
      Serial.println(lu); //affiche le message dans le moniteur
       int nb = 0;
       int k=0;

       for(int i =0; i<3; i++){// boucle pour découper l'information reçu en 3 (syllabe correcte, numero de l'afficheur 0 ou 1, syllabe incorrecte)
        lus[i]="";
       }
       for(int i =0; i<lu.length(); i++){
          if(lu[i] == ' '){
            nb++;
          }
          else{
            lus[nb]+= lu[i];
          }
       }
      
      char correct[lus[0].length()+1];
      char incorrect[lus[2].length()+1];
      int val = lus[1].toInt();
      lus[0].toCharArray(correct, (lus[0].length()+1)); //converti la syllabe correcte (string ) en tableau de char
      lus[2].toCharArray(incorrect, (lus[2].length()+1));  //converti la syllabe incorrecte (string ) en tableau de char
      //Serial.println(String(lu.length()));
      char* syllabes[2] = {correct, incorrect};
      
       TFTscreen.background(255, 255, 255); //mettre le fond des deux afficheurs en blancs
       TFTscreen2.background(255, 255, 255); //mettre le fond des deux afficheurs en blancs
      TFTscreen.stroke(0, 0, 2);
      TFTscreen2.stroke(0, 0, 2);
      TFTscreen.setTextSize(6); //fixer la police du texte à afficher
      TFTscreen2.setTextSize(6);
      TFTscreen.text(syllabes[val], 30, 45); //écrire la syllabe 1
      TFTscreen2.text(syllabes[1-val], 30, 45); //écrire la syllabe 1
   }
  }
 
}
