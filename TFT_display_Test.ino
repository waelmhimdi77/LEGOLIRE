#include <TFT.h>
#include <SoftwareSerial.h>
#include <SPI.h>
//PIN-Einstellungen
#define cs 10 
#define dc 9  
#define rst 8
String lu = "";
char c=' ';
TFT TFTscreen = TFT(cs, dc, rst);
TFT TFTscreen2 = TFT(7, 6, 3);
void setup() {
  Serial.begin(9600);
 /* TFTscreen.begin();
  TFTscreen2.begin();
  //Hintergrund: Schwarz
  TFTscreen.background(255, 255, 255);TFTscreen2.background(255, 255, 255);
  //Textfarbe: Weiß
  TFTscreen.stroke(0, 0, 2); TFTscreen2.stroke(0, 0, 2);*/
}

void loop() {



  //waits while the user does not start data
  /*while (!Serial.available()) {
    TFTscreen.stroke(0, 0, 2);TFTscreen2.stroke(0, 0, 2);
    TFTscreen.setTextSize(6);TFTscreen2.setTextSize(6);
    //Text ausgeben
    TFTscreen.text("CHA", 30, 45);TFTscreen2.text("LA", 30, 45);
    TFTscreen2.stroke(225, 225, 225);
    TFTscreen2.text("T", 100, 45);
    //Schriftgröße einstellen
    delay(250);
  };*/

  //retrieves the chosen option
  if(Serial.available()>0){
    c = Serial.read();
    Serial.print(c);
   /*lu = Serial.readString();
     TFTscreen.background(255, 255, 255);TFTscreen2.background(255, 255, 255);
     char valeur[lu.length()];
    lu.toCharArray(valeur, lu.length());
    Serial.println(valeur);
     TFTscreen.stroke(0, 0, 2);TFTscreen2.stroke(0, 0, 2);
    TFTscreen.setTextSize(6);TFTscreen2.setTextSize(6);
    //Text ausgeben
    TFTscreen.text(valeur, 30, 45);TFTscreen2.text(valeur, 30, 45);
  */
  }
 // Serial.println(lu);
  
  //remove all characters after option (as \n per example)
/*  while (Serial.available()>0) {
    if (Serial.read() == '\n') break;
    else {TFTscreen.background(255, 255, 255);TFTscreen2.background(255, 255, 255);}
    Serial.read();
  }*/
  //Serial.println(ch);
  //Schriftgröße einstellen
   
    
  /*if (op - 48 == 1) {
    TFTscreen.background(10, 255, 5);
  }
  else if (op - 48 == 0) {
    TFTscreen.background(255, 5, 5);
  }*/

}
