#include <TFT.h>
//#include <SPI.h>
//PIN-Einstellungen
#define cs 10 
#define dc 9  
#define rst 8

TFT TFTscreen = TFT(cs, dc, rst);
TFT TFTscreen2 = TFT(7, 6, 3);
void setup() {
  Serial.begin(9600);
  TFTscreen.begin();
  TFTscreen2.begin();
  //Hintergrund: Schwarz
  TFTscreen.background(255, 255, 255);TFTscreen2.background(255, 255, 255);
  //Textfarbe: Weiß
  TFTscreen.stroke(0, 0, 2); TFTscreen2.stroke(0, 0, 2);
}

void loop() {



  //waits while the user does not start data
  while (!Serial.available()) {
    TFTscreen.stroke(0, 0, 2);TFTscreen2.stroke(0, 0, 2);
    TFTscreen.setTextSize(6);TFTscreen2.setTextSize(6);
    //Text ausgeben
    TFTscreen.text("CHA", 30, 45);TFTscreen2.text("LA", 30, 45);
    TFTscreen2.stroke(225, 225, 225);
    TFTscreen2.text("T", 100, 45);
    //Schriftgröße einstellen
    delay(250);
  };

  //retrieves the chosen option
  int op = (int)Serial.read();

  //remove all characters after option (as \n per example)
  while (Serial.available()) {
    if (Serial.read() == '\n') break;
    else {TFTscreen.background(255, 255, 255);TFTscreen2.background(255, 255, 255);}
    Serial.read();
  }
  //Schriftgröße einstellen
  if (op - 48 == 1) {
    TFTscreen.background(10, 255, 5);
  }
  else if (op - 48 == 0) {
    TFTscreen.background(255, 5, 5);
  }

}
