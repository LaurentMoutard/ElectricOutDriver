
/*
  Drives three electrical outlets Manually or Online
  by L.Moutard
  This example code is in the public domain :-)
  modified 05 mars 2017
*/
#include <ArduinoJson.h> //information format is a Json
// Example :
//{"programme":"8","delai":"2000"}
//or
//{"programmeEnCours":"8","delaiEnCours":"2000"}

#include <SoftwareSerial.h>
// Serial Port used for the comm with 8266
SoftwareSerial mySerial(11, 12); //RX, TX
String json;

// Pins of BCD to 7 Segment LED Display Decoder.
const int bit_A = 2;
const int bit_B = 3;
const int bit_C = 4;
const int bit_D = 5;

const int buttonManuOnline = 13; // Switch between OnLine Mode and Manual Mode
int buttonState = 0; // Detect toggle between Manual and OnLine
int mode = 1; // OnLine Mode and Manual Mode

// Connexion pin of the pushButton (-> increment programme var) -
// ! This button is Bistable switch  -> one push : open / another push close !
const int buttonProgramme = 6;
const int ledManu = 10; // led for showing manual pushButton
const int potarDelai = 0; // Pin number of the Potentiometer for Delay
int etatButton; // Detect pushButton changing state
int memoireButton = LOW; // State of the program pushbutton (on manual mode)

int      programme; // Runnning program
int delai = 500; // Delay of the chase and thee alea

int dureeDuGetProgramme = 0; // Useful for calulating delay au Chase and alea
int dureeDuPutProgramme = 0; // Useful for calulating delay au Chase and alea

unsigned long previousMillis = 0; // last time of esp8266 checking
long intervalDInterrogation = 1000; // Interval of request to the esp8266


const int relay[] = {0 , 7, 8, 9}; // pins of electrical relays 1, 2, 3. (0 is unused)

void setup() {
  pinMode(ledManu, OUTPUT);
  pinMode(buttonProgramme, INPUT);
  pinMode(buttonManuOnline, INPUT);
  Serial.begin(9600);           // Serial port on 9600 bauds
  mySerial.begin(9600);         // softwareSerial port on 9600 bauds

  // Pins of the BCD to 7 Segment LED Display Decoder (CD4543BE)
  //  https://zestedesavoir.com/tutoriels/686/arduino-premiers-pas-en-informatique-embarquee/743_gestion-des-entrees-sorties/3424_afficheurs-7-segments/
  pinMode(bit_A, OUTPUT);
  pinMode(bit_B, OUTPUT);
  pinMode(bit_C, OUTPUT);
  pinMode(bit_D, OUTPUT);

  // Relays Pins on Output and 0
  for (int i = 1; i < 4; i++) {
    pinMode(relay[i], OUTPUT);
    digitalWrite(relay[i], LOW);
  }

  // Led Display shows 0
  digitalWrite(bit_A, LOW);
  digitalWrite(bit_B, LOW);
  digitalWrite(bit_C, LOW);
  digitalWrite(bit_D, LOW);
}

void loop() {
  getMode(); // Check if Manual or onLine
  if (mode == 0) { // manual mode
    programme = readPushButton(); // Check if pushButton has been pushed
    afficher(programme); // display to 7 segments
    modePrise(programme); // select the electrical outlet mode

  } else { // onLine Mode
    unsigned long currentMillis = millis();
    // If period of checking esp8266 (intervalDInterrogation) is ok
    if (currentMillis - previousMillis > intervalDInterrogation) {
      previousMillis = currentMillis;
      Serial.println("getProgramme");
      getProgramme(); // check information from esp8266
      afficher(programme);
      modePrise(programme);
    }

    Serial.println("putProgramme");
    putProgramme(); // push information to esp8266
    Serial.println("Fin de putProgramme");
  }

}

void getMode() { // Check if Manual or Online
  buttonState = digitalRead(buttonManuOnline);
  if (buttonState == HIGH) {
    mode = 1; // OnLine
  } else {
    mode = 0; // Manual
    int valDelai = analogRead(potarDelai); // Delay for chase and alea red on the potentiometer
    delai = map(valDelai, 0, 1023, 5000, 100); // transform potentometer Delay from 5000ms max to 100ms min

  }
}


// Check Program from esp 8266 and read the Received Json
void getProgramme() {
  int current = millis(); // currentTime To know the duration getProgramme

  // Read from softwareSerial
  Serial.print("getProgramme json : ");
  //  mySerial.listen();
  if (mySerial.available() > 0 ) {
    json = mySerial.readStringUntil('\n');
  }
  Serial.println(json);

  // Decode the Json if OK
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  if (root.success()) // if it's a well formed Json
  {
    int progRecu = root["programme"];
    Serial.print("progrec : ");
    Serial.println(progRecu);
    if (root["programme"] != NULL && int(progRecu) >= 0 && int(progRecu) <= 9) {// programme must be between 0 and 9
      programme  = progRecu;
    }
    Serial.print("programme : ");
    Serial.println(programme);

    int delaiRecu = root["delai"];
    Serial.print("delaiRecu : ");
    Serial.println(delaiRecu);
    if (root["delai"] != NULL && int(delaiRecu) >= 0 && int(delaiRecu) <= 30000) {// delay must be between 0 and 30s
      delai  = delaiRecu;
    }
    Serial.print("delai : ");
    Serial.println(delai);
  }
  dureeDuGetProgramme = millis() - current ; // Used for chase and alea programms
  Serial.print("--------------------------------------->duree : ");
  Serial.println(dureeDuGetProgramme);
}

// send Json on softwareSerial port
void putProgramme() {
  int current = millis();// currentTime To know the duration getProgramme

  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  root["programmeEnCours"] = String(programme);
  root["delaiEnCours"] = String(delai);

  //  root.printTo(Serial);
  Serial.print("putPrograme json : ");
  root.printTo(Serial);
  Serial.println("");
  //  root.prettyPrintTo(mySerial);

  // send to softwareSerial
  root.printTo(mySerial);
  mySerial.write('\n');

  dureeDuPutProgramme = millis() - current ;
  Serial.print("--------------------------------------->duree : ");
  Serial.println(dureeDuPutProgramme);
}


int readPushButton() //read pushbutton to check (on manual mode) if we need to increment programme
{
  int i = programme ;
  etatButton = digitalRead(buttonProgramme);
  //  Serial.println(i, DEC); // Message sur le port série
  if ((etatButton != memoireButton)) {
    clignote(2, 50);
    if (i < 9) {
      i++;
    }
    else
    {
      i = 0;
    }
  }
  memoireButton = etatButton; // Save the position of the pushButton
  return i;
}

void modePrise(int chiffre) // 9 programms to select Electrical Outlet Relays
{
  Serial.print("modePrise : ");
  Serial.println(chiffre);
  switch (chiffre)
  {
    case 0 :// all down
      digitalWrite(relay[1], LOW);
      digitalWrite(relay[2], LOW);
      digitalWrite(relay[3], LOW);
      break;
    case 1 : // 1st realy on
      digitalWrite(relay[1], HIGH);
      digitalWrite(relay[2], LOW);
      digitalWrite(relay[3], LOW);
      break;
    case 2 :
      digitalWrite(relay[1], LOW);
      digitalWrite(relay[2], HIGH);
      digitalWrite(relay[3], LOW);
      break;
    case 3 :
      digitalWrite(relay[1], LOW);
      digitalWrite(relay[2], LOW);
      digitalWrite(relay[3], HIGH);
      break;
    case 4 ://1 & 2 relays on
      digitalWrite(relay[1], HIGH);
      digitalWrite(relay[2], HIGH);
      digitalWrite(relay[3], LOW);
      break;
    case 5 :
      digitalWrite(relay[1], HIGH);
      digitalWrite(relay[2], LOW);
      digitalWrite(relay[3], HIGH);
      break;
    case 6 :
      digitalWrite(relay[1], LOW);
      digitalWrite(relay[2], HIGH);
      digitalWrite(relay[3], HIGH);
      break;
    case 7 :
      digitalWrite(relay[1], HIGH);
      digitalWrite(relay[2], HIGH);
      digitalWrite(relay[3], HIGH);
      break;
    case 8 :// chase
      chenillard();
      break;
    case 9 :// alea
      dansealea();
      break;
  }
}
void chenillard () { // Programme of the chase : 1,2,3,1,2,3,1,2,3
  while (programme == 8) {
    for (int j = 1; j < 4; j++) {
      digitalWrite(relay[j], HIGH);
      getMode();
      if (mode == 0) {
        programme = readPushButton();
        delay(delai);
      }
      else {
        getProgramme();
        putProgramme();
        if ((dureeDuGetProgramme + dureeDuPutProgramme) < delai) { // remove the "push and get" duration to obtain the good delay if necessary
          delay(delai - dureeDuGetProgramme + dureeDuPutProgramme);
        }
        else {
          delay(delai);
        }
      }
      digitalWrite(relay[j], LOW); // open relay before closing the next relay
    }
  }
}

void dansealea () { // Alea sequence
  while (programme == 9) {
    int alea = random(1, 4);
    Serial.println(alea, DEC);
    digitalWrite(relay[alea], HIGH);
    getMode();
    if (mode == 0) {
      programme = readPushButton();
      delay(delai);
    }
    else {
      getProgramme();
      putProgramme();
      if ((dureeDuGetProgramme + dureeDuPutProgramme) < delai) { // remove the "push and get" duration to obtain the good delay if necessary
        delay(delai - dureeDuGetProgramme + dureeDuPutProgramme);
      }
      else {
        delay(delai);
      }
    }
    digitalWrite(relay[alea], LOW);
  }
}

void afficher(int chiffre) // Display on 7 segments with BCD to 7 Segment LED Display Decoder
{
  // decoder pins on 0
  digitalWrite(bit_A, LOW);
  digitalWrite(bit_B, LOW);
  digitalWrite(bit_C, LOW);
  digitalWrite(bit_D, LOW);

  if (chiffre >= 8)
  {
    digitalWrite(bit_D, HIGH);
    chiffre = chiffre - 8;
  }
  if (chiffre >= 4)
  {
    digitalWrite(bit_C, HIGH);
    chiffre = chiffre - 4;
  }
  if (chiffre >= 2)
  {
    digitalWrite(bit_B, HIGH);
    chiffre = chiffre - 2;
  }
  if (chiffre >= 1)
  {
    digitalWrite(bit_A, HIGH);
    chiffre = chiffre - 1;
  }
}

void clignote(int nbrClignotements, int duree) { // blinking (nbr of blinks, duration of light)
  for (int i = 0; i < nbrClignotements; i++) {
    digitalWrite(ledManu, HIGH);   // Turn the LED on
    delay(duree);
    digitalWrite(ledManu, LOW);   // Turn the LED off
    delay(duree);
  }
}

