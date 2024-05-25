//resistance 1k entre pin arduino et enablePin L298N; condenseur 100µF entre enablePin L298N et GRD

// Définition des broches
const int enablePin = 10;  // ENB connecté à la pin 10 pour PWM
const int inPin = 9;       // IN3 connecté à la pin 9, pour un sens fixe
const int rpmPin = 4;      // Pin pour lire les RPM du ventilateur

void setup() {
  // Initialisation des broches
  pinMode(enablePin, OUTPUT);
  pinMode(inPin, OUTPUT);
  pinMode(rpmPin, INPUT);

  // Configuration du sens de rotation
  digitalWrite(inPin, HIGH);  // Mettre HIGH pour activer un sens fixe

}

void loop() {
  delay(500);  // Attente de 2 secondes
  analogWrite(enablePin, 250);  // Réduire la vitesse
  //delay(2000);  // Attente de 2 secondes
  //analogWrite(enablePin, 200); // Augmenter la vitesse
}
