//Constantes
const int AnalogPin=A0;
const int stepPin=3;
const int dirPin=2;
const int enablePin=4;
bool mainmenu=true;

//Paramètres par défaut
unsigned int Microstepping=16; 
float DegreePerSeconds=60;
float DegreePerStep=1.8;
unsigned long stepDelay=(unsigned long)(round((DegreePerStep/(float)Microstepping)/(2*DegreePerSeconds/1000000)));

//Initialisation
void setup() {
  Serial.begin(9600);
  while(!Serial){}
  Serial.println(" ");
  pinMode(stepPin,OUTPUT);
  pinMode(dirPin,OUTPUT);
  pinMode(enablePin,OUTPUT);
}

//Fonctions
int ReadToInt() {//lis le reste du buffer et convertit en int(ex. 123)
  String inString="";
  while (Serial.available() > 0) {
    int inChar = Serial.read();
    if (inChar != '\n') {
      inString += (char)inChar;
    }
  }
//  Serial.println(inString.toInt());          //Debug
  return inString.toInt();
}
float ReadToFloat() {//lis le reste du buffer et convertit en float(ex. 123.45)
  String inString="";
  while (Serial.available() > 0) {
    int inChar = Serial.read();
    if (inChar != '\n') {
      inString += (char)inChar;
    }  
  }
//  Serial.println(inString.toFloat());          //Debug
  return inString.toFloat();
}
void steps(long num) {//effectue un nombre 'num' de step au moteur pas à pas
//  Serial.print("steps ");                    //Debug
//  Serial.println(num);                       //Debug
//  unsigned long start=0;                     //Debug
//  unsigned long finish=0;                    //Debug
//  start=micros();                            //Debug
  for(int i=0;i<num;i++) {
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(stepDelay-7);
    digitalWrite(stepPin,LOW);
    delayMicroseconds(stepDelay-7);
  }
//  finish=micros();                           //Debug
//  Serial.println(finish-start);              //Debug
}
void StepChange(long num) {//Décide la direction de rotation du moteur pas à pas et effectue le nombre num de pas
  if (num>0) {
    digitalWrite(dirPin,HIGH);
    steps(abs(num));
    digitalWrite(dirPin,LOW);
  }
  if(num<0) {
  steps(abs(num));
  }
  Serial.println(" ");
}  
void DegreeChange(float degree) {//Décide la direction de rotation et nombre de pas pour tourner d'un certain nombre de degrés (PAS UTILISÉ DANS LABVIEW (je pense))
  float num=degree*(float)Microstepping/DegreePerStep;
  if (degree>0) {
    digitalWrite(dirPin,HIGH);
    steps((long)round(abs(num)));
    digitalWrite(dirPin,LOW);
  }
  if(degree<0) {
  steps((int)round(abs(num)));
  }
  Serial.println(" ");
  
}
void MeasureSample(int sampleSize) {//prend une mesure et l'envoie à l'ordinateur et répète 'sampleSize' fois
  for(int i=0;i<sampleSize;i++) {
    Serial.println(analogRead(AnalogPin));
  }
}
void MeasureUntil() {//prend une mesure et l'envoie à l'ordinateur sans cesse jusqu'a ce que l'ordinateur envoie 's'
  bool enough=true;
  while(enough) {
    Serial.println(analogRead(AnalogPin));
    if(Serial.available()>0) {
      char Cmd=Serial.read();
      if(Cmd=='s'){
        enough=false;
      }
    }
  }
}
void Parameters() {//Permet de modifier temporairement les paramètres par défaut(reste en mémoire jusqu'à la fermeture du port en série ou le redémarrage de la Arduino) 
  if (Serial.available()>0) {
    char Cmd=Serial.read();
    switch (Cmd) {
      case 'm' :
      Microstepping=ReadToInt();// Doit être la même valeur que celle du driver. pas besoin de changer si c'est déjà le paramêtre par défaut
//      Serial.println(Microstepping);         //Debug
      break;
      case 'e' :
      if(ReadToInt()>0) {// C'est avec ceci que la pin enable du Driver peut être activée et désactivée (allumer ou éteindre le stepper motor)
        digitalWrite(enablePin,HIGH);
      }
      else {
        digitalWrite(enablePin,LOW); 
      }
      break;
      case 'd' :
      DegreePerStep=ReadToFloat();//Caractéristique du stepper motor (pas besoin d'être modifié à moins que le moteur change)
      break;
      case 's' :
      DegreePerSeconds=ReadToFloat();//vitesse de rotation du moteur
//      Serial.println(DegreePerSeconds);      //Debug
      break;
    }
    stepDelay=(int)(round((DegreePerStep/(float)Microstepping)/(2*DegreePerSeconds/1000000)));
//    Serial.println(stepDelay);               //Debug
  }
  Serial.println(" ");//Le subVI labView attend de recevoir un caractère pour confirmer que la arduino est prête à recevoir d'autre commandes
}

//Code qui est exécuté sans arrêt
void loop() {
//  if (mainmenu==true) {                      //Debug
//    mainmenu=false;                          //Debug
//    Serial.println("Main menu");             //Debug
//  }                                          //Debug
  if (Serial.available()>0) {
    delay(10);
//    mainmenu=true;                           //Debug
    char Cmd=Serial.read();
    switch (Cmd) {
      case 'd':
//      Serial.println("DegreeChange");        //Debug
      DegreeChange(ReadToFloat());
      break;
      case 's':
      StepChange(ReadToInt());
      break;
      case 'm':
//      Serial.println("MeasureSample");       //Debug
      MeasureSample(ReadToInt());
      break;
      
      case 'n':
//      Serial.println("MeasureUntil");        //Debug
      MeasureUntil();
      break;

      case 'p':
//      Serial.println("Parameters");          //Debug
      Parameters();
      break;
      
      default: break;
    }
  }
}
