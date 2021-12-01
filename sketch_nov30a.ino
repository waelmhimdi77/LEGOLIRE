#include <SoftwareSerial.h>
//SoftwareSerial BTSerial(2, 3); // RX | TX
char c = ' ';
boolean NL = true;
void setup()
{
      Serial.begin(9600);
     // BTSerial.begin(9600);   
      Serial.println("Arduino avec HC-0x");
}
void loop()
{
    // Lecture du module BT et affichage des données sur le moniteur série de l'Ordinateur
 /*   if (BTSerial.available())
    {
        c = BTSerial.read();
        Serial.write(c);
    }*/
    
    // Lecture du moniteur série et envoi des données sur le module BT
    if (Serial.available())
    {
        c = Serial.read();
        Serial.print(c);
        //BTSerial.write(c);      
        // Echo the user input to the main window. The ">" character indicates the user entered text.
       /* if (NL) { Serial.print(">");  NL = false; }
        Serial.write(c);
        if (c==10) { NL = true; }*/
    }
}
