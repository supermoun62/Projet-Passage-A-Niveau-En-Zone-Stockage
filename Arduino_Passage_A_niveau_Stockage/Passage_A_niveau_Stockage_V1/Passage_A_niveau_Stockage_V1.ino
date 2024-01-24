/* Synopsis: Passage a niveau avant stockage
   Carte   : Arduino Nano
   Capteur : Capteurs ILS  
             
  Biblio:  Servo.h 
             
  Source:  https://www.locoduino.org/spip.php?article262
           https://www.locoduino.org/spip.php?article62
           https://www.volta.ma/comment-controler-les-servomoteurs-avec-arduino/arduino/
           https://www.carnetdumaker.net/articles/controler-un-servomoteur-avec-une-carte-arduino-genuino/
           https://eskimon.fr/tuto-arduino-602-un-moteur-qui-a-de-la-tête-le-servomoteur
           
   https://electroniquepassion.fr/ 
   C.Mercier  62128 Boiry-Becquerelle  
*/
//-----------------------------------------------------------------
#include <Servo.h>
Servo monServo1;
Servo monServo2;
int vitesse;
int angle1;
int angle2;
const int angleMin1  = 1000;
const int angleMax1 = 2000;
const int angleMin2  = 1000;
const int angleMax2 = 2000;
const int ANGLE1SOUSTENSION = 1000;
const int ANGLE2SOUSTENSION = 2000;
bool cdeServos = 0;

const int DET1 = A0;
const int DET2 = A1;

const byte LEDZONEOCCUPEE = 13;// Utilisee pour informtion zone occupee
bool memDet1;
bool memDet2;

unsigned long compteurTempsFermetureBarrieres = 0;
unsigned long compteurTempsOuvertureBarrieres = 0;
int retardFermetureBarrieres = 1000;//  En ms
int retardOuvertureBarrieres = 1000;//  En ms

bool fermetureBarrieres = 0;
bool ouvertureBarrieres = 0;
bool zoneOccupee = 0;// Zone entre les deux capteurs
int etatDet1 = 1023;
int etatDet2 = 1023;

const byte FLASH = 3;// Port PD
const byte DEL_SW1_SW2 = 2;
unsigned long compteurTempsLeds = 0; //  Variable pour compter le temps pour faire clignoter à intervale fixe les leds
bool flipFlop = 0;// Allumage Leds
//-----------------------------------------------------------------
void setup()
{
monServo1.attach(4);// Port PD
monServo2.attach(8);// Port PB
angle1 = angleMin1;
angle2 = angleMin2;
vitesse = 2;
monServo1.writeMicroseconds(ANGLE1SOUSTENSION);
monServo2.writeMicroseconds(ANGLE2SOUSTENSION);

pinMode(DET1, INPUT);// Detecteur avant passage a niveau
pinMode(DET2, INPUT);// Detecteur apres passage a niveau
pinMode(FLASH, OUTPUT);
pinMode(DEL_SW1_SW2, OUTPUT);
pinMode(LEDZONEOCCUPEE, OUTPUT);

digitalWrite(FLASH,HIGH);
digitalWrite(DEL_SW1_SW2,HIGH); 
digitalWrite(LEDZONEOCCUPEE,LOW);
memDet1 = 0;
memDet2 = 1;

delay(1000);

Serial.begin(9600); 
}//FIN_SETUP
//-----------------------------------------------------------------
void loop()
{
etatDet1 = analogRead(DET1);
etatDet2 = analogRead(DET2);
//--------------------------------------------
  if(etatDet1 <= 512 && zoneOccupee == 0 && memDet2 == 1)
    {
      memDet1 = 1;
      memDet2 = 0;
      zoneOccupee = 1;
    }
//--------------------------------------------    
  if(etatDet2 <= 512 && zoneOccupee == 0 && memDet1 == 1)
    {
      memDet2 = 1;
      memDet1 = 0;
      zoneOccupee = 1;
    }
//--------------------------------------------
  if ((etatDet1 <= 512 && memDet1 == 1) || (etatDet2 <= 512 && memDet2 == 1) )
    {         
        fermetureBarrieres = 1;
        compteurTempsFermetureBarrieres = millis();         
    }
  if(fermetureBarrieres == 1)
    {
       if((millis()-compteurTempsFermetureBarrieres) >= retardFermetureBarrieres)
        { 
          fermetureBarrieres = 0; 
          cdeServos = 1; 
        }  
    }
//--------------------------------------------
  if ((etatDet2 <= 512 && zoneOccupee == 1 && memDet1 == 1) || (etatDet1 <= 512 && zoneOccupee == 1 && memDet2 == 1))
    {         
        ouvertureBarrieres = 1;
        compteurTempsOuvertureBarrieres = millis();         
    }
  if(ouvertureBarrieres == 1)
    {
       if((millis()-compteurTempsOuvertureBarrieres) >= retardOuvertureBarrieres)
        { 
          ouvertureBarrieres = 0; 
          zoneOccupee = 0;
          cdeServos = 0;
        }  
    }
//--------------------------------------------
  if(zoneOccupee == 1) 
    {  
      digitalWrite(LEDZONEOCCUPEE,HIGH);
      Signal();// appel fonction Leds
    }      
  else
    {
      digitalWrite(LEDZONEOCCUPEE,LOW);
      digitalWrite(FLASH,HIGH);               
    }   
//--------------------------------------------
if(cdeServos == 1)
  {
    ServosFermeture1();
    ServosOuverture2();// Modifie le 30/12/2023
    delay(2);
  }
//--------------------------------------------
if(cdeServos == 0)
  {
    ServosOuverture1();
    ServosFermeture2();// Modifie le 30/12/2023
    delay(2);
  }
//--------------------------------------------
if(etatDet1 < 512 || etatDet2 < 512) 
  {
    digitalWrite(DEL_SW1_SW2,LOW);
  }  
else
  {
    digitalWrite(DEL_SW1_SW2,HIGH);
  }
//--------------------------------------------
Serial.print(etatDet1);
Serial.print("\t");
Serial.println(etatDet2);
//Serial.print("\t");
//Serial.print(memDet1);
//Serial.print("\t");
//Serial.print(memDet2);
//Serial.print("\t");
//Serial.print(cdeServos);
//Serial.print("\t");
//Serial.print("\t");
//Serial.print(angle1);
//Serial.print("\t");
//Serial.print(angle2);
//Serial.print("\t");
//Serial.println(zoneOccupee);
}//FIN_LOOP
//-----------------------------------------------------------------
void ServosFermeture1()
{
monServo1.writeMicroseconds(angle1);
/* calcule le nouvel angle */
angle1 = angle1 + vitesse;
 
  if (angle1 > angleMax1)
    {
      angle1 = angleMax1;
    }
//FLASHay(2);    
}  
//-----------------------------------------------------------------
void ServosOuverture1()
{
monServo1.writeMicroseconds(angle1);
/* calcule le nouvel angle */
angle1 = angle1 - vitesse;

  if (angle1 < angleMin1)
    {
      angle1 = angleMin1;
    }
//FLASHay(2);    
}  
//-----------------------------------------------------------------
void ServosFermeture2()
{
monServo2.writeMicroseconds(angle2);
/* calcule le nouvel angle */
angle2 = angle2 + vitesse;
 
  if (angle2 > angleMax2)
    {
      angle2 = angleMax2;
    }
//FLASHay(2);    
}  
//-----------------------------------------------------------------
void ServosOuverture2()
{
monServo2.writeMicroseconds(angle2);
/* calcule le nouvel angle */
angle2 = angle2 - vitesse;

  if (angle2 < angleMin2)
    {
      angle2 = angleMin2;      
    }
//FLASHay(2);    
} 
//-----------------------------------------------------------------
void Signal()
{
  if (millis() > compteurTempsLeds + 500)
    {
      compteurTempsLeds = millis();
        if (flipFlop == false)
         {
          flipFlop = true;
          digitalWrite(FLASH,HIGH);          
         } 
        else
         {
          flipFlop = false;
          digitalWrite(FLASH,LOW);         
         }
    }
} 