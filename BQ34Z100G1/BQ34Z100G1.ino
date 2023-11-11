// main.ino

#include "BQ34Z100-G1.h"

BQ34Z100_G1 bq34z100_g1(21, 22); // Crear una instancia del BQ34Z100-G1

void setup() {
  Serial.begin(9600); // Iniciar la comunicación serial
  while (!Serial) {
    ; // Esperar a que la comunicación serial esté lista
  }
}

void loop() {
  uint16_t oldPackConfig = bq34z100_g1.getOldPackConfiguration(); // Leer la configuración de pack antigua
  //bq34z100_g1.modifyDataFlash(); // Hacer la configuración de pack
  uint16_t newPackConfig = bq34z100_g1.getNewPackConfiguration(); // Leer la configuración de pack antigua
  Serial.print("Old Pack Configuration: "); // Imprimir el mensaje
  Serial.println(oldPackConfig, BIN); // Imprimir la configuración de pack antigua
  Serial.print("New Pack Configuration: "); // Imprimir el mensaje
  Serial.println(newPackConfig, BIN); // Imprimir la configuración de pack nueva
  Serial.println("Gas Gauge PACK_CONFIG");
  char* values = bq34z100_g1.Status();
  Serial.println(*values , BIN); 
  delay(4000); // Esperar un segundo
}


