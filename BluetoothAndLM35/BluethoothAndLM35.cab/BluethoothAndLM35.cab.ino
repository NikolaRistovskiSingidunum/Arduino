#include <TimerOne.h>

#define SensorPin A0
const unsigned long SensorReadPeriod = 10000; //microseconds
//Temperature variable
volatile  int CurrentTemp;
volatile  int TempSum;
volatile  int Temp;
volatile int counter =0;
volatile const int SamplesNo = 100;
bool cekamoOdgovor = false;
//ako je true onda kada stigne potvrdan odgovor palimo sijalicu, a ako je false onda je gasimo
bool upali = true;

//kada je HIGH vrednost releju su ukljuceni
//menja rezim rada izmedju prekidaca i releja2, kada je relej1 = HIGH onda redi prekidac,a kada je relej1 = LOW onda relej2 kontrolise diodu
//tj. kada je relej1 HIGH onda je rezim manualni(rucni), inace kada je relej1=LOW rezim je automacki
const unsigned int relej1 = 4;
//relej2 kontrolise diodu kada je rezim upravljanja autom
const unsigned int relej2 = 5;
void TimerISR()
{
CurrentTemp=(analogRead(SensorPin)*500.0)/1024.0;

Serial.println(CurrentTemp);

//Add to the temperature sum;

TempSum+=CurrentTemp;
counter++;
if(counter == SamplesNo){
//Count average
Temp = TempSum/SamplesNo;

TempSum = 0;
counter = 0;

}
}

void setup() {
  // put your setup code here, to run once:
  //analogReference(INTERNAL);
  pinMode(SensorPin,INPUT);
  pinMode(relej1,OUTPUT);
  pinMode(relej2,OUTPUT);
  digitalWrite(relej1,LOW);
  digitalWrite(relej2,LOW);
  Serial.begin(9600);
  Timer1.initialize(SensorReadPeriod);
  Timer1.attachInterrupt(TimerISR);
}

void loop() {


  //ako ima neprocitanih poruka
  if(Serial.available() > 0)
  {
  //citamo poruku     
  String msg = Serial.readString();
  //ako ne cekamo na odgovor
  if(cekamoOdgovor == false)
  {
  //ukoliko se trazi temperatura  
  if(msg == "temp")
  {
   Serial.println("temeratura je");
   Serial.println(Temp); 
  }
  //ukoliko se zeli da se predje u rezim rada auto   
  else if(msg == "auto")
  {
    digitalWrite(relej1,LOW);
    Serial.println("pokrenut rezim auto");
  }
  //ukoliko se zeli da se predje u rezim manualni
  else if(msg == "manual")
  {
    digitalWrite(relej1,HIGH);
   Serial.println("pokrenut rezim manual");
   
  }
  //ukoliko je komanda upali i rezim rada je automatik
  else if(msg == "upali" && digitalRead(relej1)==LOW)
  {
    
    digitalWrite(relej2,HIGH);
    Serial.println("sijalica je upaljena");   
  }
  //ukoliko je komanda ugasi i rezim rada je automatik
  else if(msg == "ugasi" && digitalRead(relej1)==LOW)
  {
    digitalWrite(relej2,LOW);
    Serial.println("sijalica je ugasena");   
  }
  //ukoliko je komanda upali i rezim rada je manualni
  else if(msg == "upali")
  {
    //specifikujemo kakav odgovor cekamao
    cekamoOdgovor = true;
    upali = true;
    //poruka korisniku
    Serial.println("Da li zelis da predjes u rezim auto");
  }
  //ukoliko je komanda ugasi i rezim rada je manualni
   else if(msg == "ugasi")
  {
    //specifikujemo kakav odgovor cekamo
    cekamoOdgovor = true;
    upali = false;
    //poruka korisniku
    Serial.println("Da li zelis da predjes u rezim auto");
  }
  
  }
  else
  {
    cekamoOdgovor = false;
    if(msg == "da")
    {
      digitalWrite(relej1,LOW);
      Serial.println("pokrenut rezim auto");
      if(upali)
      digitalWrite(relej2,HIGH);
      else
      digitalWrite(relej2,LOW);
    }
    else
    {
      Serial.println("rezim idalje manualni");
    }
  }

  }
 

}
