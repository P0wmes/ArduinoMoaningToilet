#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <Servo.h>

#define TRIG_PIN 7 //lila
#define ECHO_PIN 6 //grau
#define SERVO_PIN 8

Servo myservo;

SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

int pos = 0;
int openedAngle = 90;
int closedAngle = 3;

void setup()
{
  mySoftwareSerial.begin(9600);
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT); // Trigger-Pin ist ein Ausgang
  pinMode(ECHO_PIN, INPUT); // Echo-Pin ist ein Eingang

  myservo.attach(SERVO_PIN);


  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true) {
      delay(10); // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer Mini online."));

  myDFPlayer.volume(23);  //Set volume value. From 0 to 30

  closeCover();
}

void loop()
{

  long entfernung = getDistance();

  if (entfernung > 10 && entfernung  <= 100)
  {
    delay(15000);
    openCover();
    delay(1000);

    if (myDFPlayer.available()) {
      printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
    }

    Serial.println("Play sound");

    myDFPlayer.next();  //Play next mp3

    if (myDFPlayer.available()) {
      printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
    }

    delay(25000);

    closeCover();
    delay(180000);
    
  }
  delay(1500);
}


int getDistance() {
  long dauer = 0;
  long entfernung = 0;
  digitalWrite(TRIG_PIN, LOW); //Hier nimmt man die Spannung für kurze Zeit vom Trigger-Pin, damit man später beim senden des Trigger-Signals ein rauschfreies Signal hat.
  delay(5); //Dauer: 5 Millisekunden
  digitalWrite(TRIG_PIN, HIGH); //Jetzt sendet man eine Ultraschallwelle los.
  delay(10); //Dieser „Ton“ erklingt für 10 Millisekunden.
  digitalWrite(TRIG_PIN, LOW);//Dann wird der „Ton“ abgeschaltet.
  dauer = pulseIn(ECHO_PIN, HIGH); //Mit dem Befehl „pulseIn“ zählt der Mikrokontroller die Zeit in Mikrosekunden, bis der Schall zum Ultraschallsensor zurückkehrt.

  entfernung = (dauer / 2) * 0.03432; //Nun berechnet man die Entfernung in Zentimetern. Man teilt zunächst die Zeit durch zwei (Weil man ja nur eine Strecke berechnen möchte und nicht die Strecke hin- und zurück). Den Wert multipliziert man mit der Schallgeschwindigkeit in der Einheit Zentimeter/Mikrosekunde und erhält dann den Wert in Zentimetern.


  if (entfernung >= 400 || entfernung <= 0) //Wenn die gemessene Entfernung über 400cm oder unter 0cm liegt,…
  {
    Serial.print(entfernung); //…soll der Wert der Entfernung an den serial monitor hier ausgegeben werden.
    Serial.println("Kein Messwert oder ungültiger Wert"); //dann soll der serial monitor ausgeben „Kein Messwert“, weil Messwerte in diesen Bereichen falsch oder ungenau sind.
    return 0;
  }
  else
  {
    Serial.print(entfernung); //…soll der Wert der Entfernung an den serial monitor hier ausgegeben werden.
    Serial.println(" cm"); // Hinter dem Wert der Entfernung soll auch am Serial Monitor die Einheit "cm" angegeben werden.
  }

  return entfernung;
}

void openCover() {
  Serial.println("OpenCover");

  for (int i = closedAngle; i < openedAngle; i++)
  {
    delay(5);
    myservo.write(i);
  }
}

void closeCover() {
  Serial.println("CloseCover");

  for (int i = openedAngle; i >= closedAngle; i--)
  {
    delay(5);
    myservo.write(i);
  }
}




void printDetail(uint8_t type, int value) {
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println("USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      Serial.println("USB Removed!");
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }

}
