// Définition des broches connectées au pont en H
const int L_PWM = 11;  // Broche pour activer le moteur vers la gauche
const int R_PWM = 10; //Broche pour activer le moteur vers la droite

void setup() {
  // Initialisation des broches en tant que sorties
  pinMode(L_PWM, OUTPUT);
  pinMode(R_PWM, OUTPUT);
}

void loop() {
  // Activation du moteur dans un sens pendant 2 secondes
  analogWrite(R_PWM, 0); //on s'assure que ce soit à 0 sur le côté droit
  analogWrite(L_PWM, 255);  // Active le moteur à pleine vitesse vers la gauche (255 est la valeur maximale)

  delay(2000);  // Attend 2 secondes

  // Arrêt du moteur pendant 1 seconde
  analogWrite(L_PWM, 0);  // Désactive le moteur en mettant enablePin à 0

  delay(1000);  // Attend 1 seconde

  // Activation du moteur dans l'autre sens pendant 2 secondes
  analogWrite(L_PWM, 0); //On s'assure que ce soit à 0 du côté gauche
  analogWrite(R_PWM, 255);  // Active le moteur à pleine vitesse vers la droite

  delay(2000);  // Attend 2 secondes

  // Arrêt du moteur pendant 1 seconde
  analogWrite(R_PWM, 0);  // Désactive le moteur

  delay(1000);  // Attend 1 seconde
}
