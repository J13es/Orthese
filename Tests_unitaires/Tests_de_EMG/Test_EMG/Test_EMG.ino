#if defined(ARDUINO) && ARDUINO >= 100
// Si le code est compilé pour une plateforme Arduino avec une version supérieure ou égale à 1.0
#include "Arduino.h"
// Inclure le fichier d'en-tête standard d'Arduino, qui contient les déclarations nécessaires pour les fonctions et les classes utilisées dans le code Arduino.
#else
// Sinon, si le code n'est pas compilé pour une plateforme Arduino avec une version supérieure ou égale à 1.0
#include "WProgram.h"
// Inclure le fichier d'en-tête WProgram.h, qui était utilisé dans les versions plus anciennes de l'IDE Arduino pour inclure les déclarations nécessaires.
#endif


#include "EMGFilters.h"
// Inclure le fichier d'en-tête EMGFilters.h, qui contient les définitions de classe et les déclarations nécessaires pour les filtres EMG.

#define TIMING_DEBUG 1
// Définir une constante TIMING_DEBUG à 1 pour activer le mode de débogage. Permet d'afficher des informations de débogage, telles que les données de l'enveloppe du signal. Pour désactiver le mode de débogage, définir cette constante à 0 ou la commenter.


#define SensorInputPin A0 // Numéro de la broche d'entrée du capteur EMG

EMGFilters myFilter; // Crée une instance de la classe EMGFilters

SAMPLE_FREQUENCY sampleRate = SAMPLE_FREQ_1000HZ; // Taux d'échantillonnage à 1000 Hz
NOTCH_FREQUENCY humFreq = NOTCH_FREQ_50HZ; // Fréquence du réseau électrique à 50 Hz

static int Threshold = 0; // Seuil de détection de l'EMG, modifier à la valeur maximal du test, toute valeur inférieur à cette valeur sera considérée comme un 0

unsigned long timeStamp;
// Déclaration d'une variable timeStamp de type unsigned long. Cette variable est utilisée pour stocker un horodatage (timestamp) en microsecondes.
unsigned long timeBudget;
// Déclaration d'une variable timeBudget de type unsigned long. Cette variable est utilisée pour définir le budget de temps disponible pour chaque itération de la boucle principale du programme, mesuré en microsecondes.


void setup() {
    // Initialise le filtre EMG avec le taux d'échantillonnage et la fréquence du réseau électrique
    myFilter.init(sampleRate, humFreq, true, true, true);
    
    // Initialise la communication série entre l'Arduino et le pc avec un débit de 115200 bauds
    // Faire attention -> est-ce que le pc et l'Arduino peuvent fonctionner avec cette valeur ? Vérifier sur l'Arduino et sinon modifier à 9600bauds
    Serial.begin(115200);
    
    // Calcul du budget de temps pour chaque boucle en microsecondes
    timeBudget = 1e6 / sampleRate;
}

void loop() {
    // Mesure du temps écoulé depuis le début de la boucle en microsecondes
    timeStamp = micros();
    
    // Lecture de la valeur du capteur EMG
    int Value = analogRead(SensorInputPin);
    
    // Filtrage du signal EMG
    int DataAfterFilter = myFilter.update(Value);
    
    // Calcul de l'enveloppe du signal (carré de la valeur après filtrage)
    int envlope = sq(DataAfterFilter);
    
    // Si la valeur est inférieure au seuil, elle est fixée à zéro
    envlope = (envlope > Threshold) ? envlope : 0;
    
    // Calcul du temps écoulé depuis le début de la boucle en microsecondes
    timeStamp = micros() - timeStamp;
    
    // Affichage de la valeur de l'enveloppe du signal
    if (TIMING_DEBUG) {
        Serial.print("Données au carré : ");
        Serial.println(envlope);
    }
    
    // Attente de 500 microsecondes avant de reprendre la boucle
    delayMicroseconds(500);
}
