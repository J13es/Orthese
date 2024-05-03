// Définition des broches connectées au pont en H
const int L_PWM = 11;  // Broche pour activer le moteur vers la gauche
const int R_PWM = 10;  // Broche pour activer le moteur vers la droite
#include "EMGFilters.h"

#define SensorInputPin A0 // Numéro de la broche d'entrée du capteur EMG

EMGFilters myFilter; // Crée une instance de la classe EMGFilters

SAMPLE_FREQUENCY sampleRate = SAMPLE_FREQ_1000HZ; // Taux d'échantillonnage à 1000 Hz
NOTCH_FREQUENCY humFreq = NOTCH_FREQ_50HZ; // Fréquence du réseau électrique à 50 Hz

static int Threshold = 40; // Seuil de détection de l'EMG à redifnir à chaqeues fois

void setup() {
  // Initialisation des broches en tant que sorties
  pinMode(L_PWM, OUTPUT);
  pinMode(R_PWM, OUTPUT);

  // Initialise le filtre EMG avec le taux d'échantillonnage et la fréquence du réseau électrique
  myFilter.init(sampleRate, humFreq, true, true, true);
  
  // Initialisation de la communication série
  Serial.begin(115200);
}

void loop() {
  // Lecture de la valeur du capteur EMG
  int Value = analogRead(SensorInputPin);
  
  // Filtrage du signal EMG
  int DataAfterFilter = myFilter.update(Value);
  
  // Calcul de l'enveloppe du signal (carré de la valeur après filtrage)
  int envelope = sq(DataAfterFilter);
  
  // Si la valeur est supérieure au seuil, une contraction musculaire est détectée
  if (envelope > Threshold) {
    // Activer le moteur vers la gauche
    analogWrite(R_PWM, 0); // le moteur vers la droite est désactivé
    analogWrite(L_PWM, 255);  // Active le moteur à pleine vitesse vers la gauche
  } else {
    // Si aucune contraction n'est détectée, arrêter le moteur
    analogWrite(L_PWM, 0);
    analogWrite(R_PWM, 255);
  }
}
