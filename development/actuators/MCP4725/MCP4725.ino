// Controls peristatic pumps via MCP4725  (DAC - Digital-to-Analogique Converter)
  // https://github.com/adafruit/Adafruit_MCP4725
  // https://passionelectronique.fr/tutorial-mcp4725/
  // Install "Adafruit MCP4725" library


const float tensionVoulueEnSortie =   0.50;     // Tension souhaitée en sortie du MCP4725, exprimée en volts (entre 0 et 5V)              //ERREUR : -0.10 à 0.20 volts dans les faits => pb calibration du multimetre ou mauvaise carte?

#define memorisationTensionDeSortie   false     // Si "false", le DAC ajustera sa tension de sortie sans la mémoriser ; si "true", cette valeur sera dispo, même après coupure de courant
#define adresseI2CduMCP4725           0x61      // Adresse de votre MCP4725, sur le bus I2C (à ajuster, au besoin)

      // Remarques, concernant les adresses possibles du MCP4725 
      // *******************************************************
      // Pour un MCP4725 "standard"     (MCP4725A0), les adresses I2C possibles sont 0x60 (si broche A0 mise à la masse) ou 0x61 (si broche A0 reliée à Vcc)
      // Pour un MCP4725 "personnalisé" (MCP4725A1), les adresses I2C possibles sont 0x62 (si broche A0 mise à la masse) ou 0x63 (si broche A0 reliée à Vcc)
      // Pour un MCP4725 "personnalisé" (MCP4725A2), les adresses I2C possibles sont 0x64 (si broche A0 mise à la masse) ou 0x65 (si broche A0 reliée à Vcc)
      // Pour un MCP4725 "personnalisé" (MCP4725A3), les adresses I2C possibles sont 0x66 (si broche A0 mise à la masse) ou 0x67 (si broche A0 reliée à Vcc)

#include <Adafruit_MCP4725.h>
Adafruit_MCP4725 dac;

// ========================
// Initialisation programme
// ========================
void setup() {

  // Initialise la liaison série (arduino nano -> PC)
  Serial.begin(115200);
  Serial.println(F("================================================================================"));
  Serial.println(F("PRG1 - Demande au MCP4725 de sortir une tension particulière, sur sa sortie Vout"));
  Serial.println(F("================================================================================"));
  Serial.println("");

  // Démarrage du DAC externe (le MCP 4725)
  dac.begin(0x61);

  // Définition de la valeur correspondante à la tension voulue en sortie du DAC
  int valeurAenvoyerAuMCP2547 = tensionVoulueEnSortie * 4095 / 5;
            // Nota : la tension souhaitée ici est exprimée en volts (entre 0 et 5V), mais la valeur à transmettre
            // pour ce faire doit être comprise entre 0 et 4095. D'où la nécessité de faire une conversion avant tout.

  // Demande au DAC de changer sa tension de sortie, suivant cette nouvelle consigne
  dac.setVoltage(valeurAenvoyerAuMCP2547, memorisationTensionDeSortie);

  // Rappel de la tension entrée dans ce programme, sur le moniteur série
  Serial.print(F("Tension souhaitée en sortie du DAC = "));
  Serial.print(tensionVoulueEnSortie);
  Serial.print(F(" volts"));
  Serial.println("");
}

// =================
// Boucle principale
// =================
void loop() {
  
  // Rien ici, car tout se passe dans la fonction setup !
  
}