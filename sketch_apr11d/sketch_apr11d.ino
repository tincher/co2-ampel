#include <Arduino.h>
#include "MHZ19.h"                                        
#include <SoftwareSerial.h>
#define RX_PIN 5         //MH-Z19 RX-PIN                                         
#define TX_PIN 4         //MH-Z19 TX-PIN  
#define MYLEDPIN 6                                 
#define BAUDRATE 9600    //Terminal Baudrate
#define SCHWELLE 1500 //Schwellwert wenn die LED von Grün auf Rot wechseln soll.


//Klassen definierung
MHZ19 myMHZ19;                               
SoftwareSerial mySerial(RX_PIN, TX_PIN); 
unsigned long getDataTimer = 0;

void setup(){
    pinMode(MYLEDPIN, OUTPUT);
      //Serielle Intialisierung
    Serial.begin(BAUDRATE);
    mySerial.begin(BAUDRATE);                   
    myMHZ19.begin(mySerial);
}
void loop(){
      //Prüfung ob 2 Sekunden um sind.
    if (millis() - getDataTimer >= 2000){
        int CO2; //Variable darf nur Zahlen enthalten
        CO2 = myMHZ19.getCO2(); //Variable schreiben
        //Ermittel ob der Schwellwert überschritten wurde und Schalten der LED
        if(CO2 > SCHWELLE){ 
            digitalWrite(MYLEDPIN, HIGH);
            delay(100);
        }else {
            digitalWrite(MYLEDPIN, LOW);
            delay(100);
        }
        //Ausgabe auf den Seriellen Monitor
        Serial.print("CO2 (ppm): ");
        Serial.println(CO2);
        getDataTimer = millis();
    }
}
