#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "EMGFiltersB.h"
#include "EMGFiltersT.h"
// Inclure le fichier d'en-tête EMGFilters.h, qui contient les définitions de classe et les déclarations nécessaires pour les filtres EMG.

#define TIMING_DEBUG 1
// Définir une constante TIMING_DEBUG à 1 pour activer le mode de débogage. Permet d'afficher des informations de débogage, telles que les données de l'enveloppe du signal. Pour désactiver le mode de débogage, définir cette constante à 0 ou la commenter.


#define SensorInputPin1 A0 // Numéro de la broche d'entrée du capteur EMG1
#define SensorInputPin2 A1 // Numéro de la broche d'entrée du capteur EMG2

EMGFiltersT myFilterT; // Crée une instance de la classe EMGFilters
EMGFiltersB myFilterB; // Crée une instance de la classe EMGFilters

SAMPLE_FREQUENCY_T sampleRateT = SAMPLE_FREQ_1000HZ_T; // Taux d'échantillonnage à 1000 Hz
NOTCH_FREQUENCY_T humFreqT = NOTCH_FREQ_50HZ_T; // Fréquence du réseau électrique à 50 Hz

SAMPLE_FREQUENCY_B sampleRateB = SAMPLE_FREQ_1000HZ_B; // Taux d'échantillonnage à 1000 Hz
NOTCH_FREQUENCY_B humFreqB = NOTCH_FREQ_50HZ_B; // Fréquence du réseau électrique à 50 

static int Threshold1 = 20; // Seuil de détection de l'EMG, modifier à la valeur maximal du test, toute valeur inférieur à cette valeur sera considérée comme un 0
static int Threshold2 = 20; // Seuil de détection de l'EMG, modifier à la valeur maximal du test, toute valeur inférieur à cette valeur sera considérée comme un 0

unsigned long timeStamp;
// Déclaration d'une variable timeStamp de type unsigned long. Cette variable est utilisée pour stocker un horodatage (timestamp) en microsecondes.
unsigned long timeBudget;
// Déclaration d'une variable timeBudget de type unsigned long. Cette variable est utilisée pour définir le budget de temps disponible pour chaque itération de la boucle principale du programme, mesuré en microsecondes.


void setup() {
    // Initialise le filtre EMG avec le taux d'échantillonnage et la fréquence du réseau électrique
    myFilterT.init(sampleRateT, humFreqT, true, true, true);
    myFilterB.init(sampleRateB, humFreqB, true, true, true);
    
    // Initialise la communication série entre l'Arduino et le pc avec un débit de 115200 bauds
    // Faire attention -> est-ce que le pc et l'Arduino peuvent fonctionner avec cette valeur ? Vérifier sur l'Arduino et sinon modifier à 9600bauds
    Serial.begin(115200);
    
    // Calcul du budget de temps pour chaque boucle en microsecondes
    timeBudget = 1e6 / sampleRateT;
}

void loop() {
    // Mesure du temps écoulé depuis le début de la boucle en microsecondes
    timeStamp = micros();

    
    
    // Lecture de la valeur du capteur EMG
    int Value1 = analogRead(SensorInputPin1);
    int Value2 = analogRead(SensorInputPin2);
    
    // Filtrage du signal EMG
    int DataAfterFilter1 = myFilterT.update(Value1);
    int DataAfterFilter2 = myFilterB.update(Value2);
    
    // Calcul de l'enveloppe du signal (carré de la valeur après filtrage)
    int envlope1 = sq(DataAfterFilter1);
    int envlope2 = sq(DataAfterFilter2);

    // Si la valeur est inférieure au seuil, elle est fixée à zéro
    envlope1 = (envlope1 > Threshold1) ? envlope1 : 0;
    envlope2 = (envlope2 > Threshold2) ? envlope2 : 0;
    
    // Calcul du temps écoulé depuis le début de la boucle en microsecondes
    timeStamp = micros() - timeStamp;
    
    // Affichage de la valeur de l'enveloppe du signal
    if (TIMING_DEBUG) {

    if (envlope1 > envlope2){

          Serial.print("biceps est supérieur : ");
          Serial.println(envlope1);
    }

    else if (envlope2 > envlope1){

      Serial.print("triceps est supérieur : ");
      Serial.println(envlope2);
    }

  else {

    Serial.println("valeurs égales");
  }
  
    // Attente de 500 microsecondes avant de reprendre la boucle
    delayMicroseconds(500);

    }
    
}
