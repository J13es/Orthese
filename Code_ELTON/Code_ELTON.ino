#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "EMGFiltersB.h"
#include "EMGFiltersT.h"
#include "Encoder.h"


#define SensorInputPin_triceps A0 // Numéro de la broche d'entrée du capteur EMG1, à mettre sur le triceps 
#define SensorInputPin_biceps A1 // Numéro de la broche d'entrée du capteur EMG2 à mettre sur le biceps
#define ENCA 3 // numéro de la broche du signal A de l'encodeur
#define ENCB 4 // numéro de la broche du signal B de l'encodeur
const int L_PWM = 11;  // Broche pour activer le moteur vers la gauche
const int R_PWM = 10; //Broche pour activer le moteur vers la droite


Encoder myEnc(ENCA, ENCB);

EMGFiltersT myFilterT; // Crée une instance de la classe EMGFilters
EMGFiltersB myFilterB; // Crée une instance de la classe EMGFilters

SAMPLE_FREQUENCY_T sampleRateT = SAMPLE_FREQ_1000HZ_T; // Taux d'échantillonnage à 1000 Hz
NOTCH_FREQUENCY_T humFreqT = NOTCH_FREQ_50HZ_T; // Fréquence du réseau électrique à 50 Hz

SAMPLE_FREQUENCY_B sampleRateB = SAMPLE_FREQ_1000HZ_B; // Taux d'échantillonnage à 1000 Hz
NOTCH_FREQUENCY_B humFreqB = NOTCH_FREQ_50HZ_B; // Fréquence du réseau électrique à 50 

static int Threshold_triceps =400; // le thresold du triceps, à faire baisser progressivement depuis 1000 s'il y a des problèmes
static int Threshold_biceps =300; // le thresold du biceps, à faire baisser progressivement depuis 1000 s'il y a des problèmes
static int Threshold_MAX = 6000; // le thresold max, toutes valeurs > sera mise à 0

unsigned long timeStamp;
// Déclaration d'une variable timeStamp de type unsigned long. Cette variable est utilisée pour stocker un horodatage (timestamp) en microsecondes.
unsigned long timeBudget;
// Déclaration d'une variable timeBudget de type unsigned long. Cette variable est utilisée pour définir le budget de temps disponible pour chaque itération de la boucle principale du programme, mesuré en microsecondes.

long pos; //position important de la mettre en type long 
float comp;  //comparaison entre les 2 emg

void setup() {

   // Initialise la communication série entre l'Arduino et le pc avec un débit de 115200 bauds
   // Faire attention -> est-ce que le pc et l'Arduino peuvent fonctionner avec cette valeur ?
    Serial.begin(115200);
    
    // Initialise le filtre EMG avec le taux d'échantillonnage et la fréquence du réseau électrique
    myFilterT.init(sampleRateT, humFreqT, true, true, true);
    myFilterB.init(sampleRateB, humFreqB, true, true, true);
   
    // Calcul du budget de temps pour chaque boucle en microsecondes
    timeBudget = 1e6 / sampleRateT;
}

void loop() {
    // Mesure du temps écoulé depuis le début de la boucle en microsecondes
    timeStamp = micros();
    
    // Lecture de la valeur du capteur EMG
    int Value_biceps = analogRead(SensorInputPin_biceps);
    int Value_triceps = analogRead(SensorInputPin_triceps);

    // Filtrage du signal EMG
    int DataAfterFilter_biceps = myFilterT.update(Value_biceps);
    int DataAfterFilter_triceps = myFilterB.update(Value_triceps);
    
    // Calcul de l'enveloppe du signal (carré de la valeur après filtrage)
    int envlope_biceps = sq(DataAfterFilter_biceps);
    int envlope_triceps = sq(DataAfterFilter_triceps);

    // Si la valeur est inférieure au seuil, elle est fixée à zéro
    envlope_biceps = (envlope_biceps > Threshold_biceps) ? envlope_biceps : 0;
    envlope_triceps = (envlope_triceps > Threshold_triceps) ? envlope_triceps : 0;


    pos = myEnc.read(); //pour avoir la position de l'encodeur, pas besoin de mettre d'interupt (déjà dans la bibliothèque)
    
    // Calcul du temps écoulé depuis le début de la boucle en microsecondes
    timeStamp = micros() - timeStamp;


    if(pos <= 0){ //condition angle max
      envlope_triceps = 0; //envlope du biceps mise à zéros 
    }

    if(pos >= 2096){ //condition angle max
      envlope_biceps = 0; //envlope du triceps mise à zéros
    }

    
    comp = envlope_biceps - envlope_triceps; //facteur modifiable devant le triceps

       
    if(abs(comp)<10){
        analogWrite(L_PWM, 0); 
        analogWrite(R_PWM, 0);
  }

    else if (comp >0){

          analogWrite(L_PWM, 0); 
          analogWrite(R_PWM, calcul_power());  
          Serial.println(calcul_power());
    }

    else if (comp<0){
      
      analogWrite(L_PWM, calcul_power());
      analogWrite(R_PWM, 0);  
    }


    
    delayMicroseconds(500);
    }


float calcul_power(){
  
    int val=abs(comp);
    
    if (val>=Threshold_MAX){
      return 0; 
    }else{
      return 255;
    }
}
    
