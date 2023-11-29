// BQ34Z100-G1.cpp

#include "BQ34Z100-G1.h"

// Definir las direcciones de los comandos del BQ34Z100-G1
#define CONTROL 0x00
#define BLOCK_DATA_CONTROL 0x61
#define DATA_FLASH_CLASS 0x3E
#define DATA_FLASH_BLOCK 0x3F
#define BLOCK_DATA 0x40
#define BLOCK_DATA_CHECKSUM 0x60

// Definir las claves de desbloqueo
#define UNSEAL_KEY_1 0x0414
#define UNSEAL_KEY_2 0x3672

// Definir la subclase y el desplazamiento de la configuración de pack
#define PACK_CONFIG_SUBCLASS 0x40
#define PACK_CONFIG_OFFSET 0x03
#define PACK_CONFIG_OFFSET_DEC 0

// Definir el bit de selección de voltaje
#define VOLTSEL_BIT 0x08
#define BQ34Z100 0x55    

BQ34Z100_G1::BQ34Z100_G1(uint8_t sda, uint8_t scl) : _sda(sda), _scl(scl) {
  Wire.begin(_sda, _scl);
}

void BQ34Z100_G1::writeControl(uint16_t command) {
  Wire.beginTransmission(0x55); // Dirección del dispositivo
  Wire.write(CONTROL); // Comando de control
  Wire.write(command & 0xFF); // Byte bajo del comando
  Wire.write(command >> 8); // Byte alto del comando
  Wire.endTransmission(); // Finalizar la transmisión
}

uint8_t BQ34Z100_G1::readByte(uint8_t command) {
  Wire.beginTransmission(0x55); // Dirección del dispositivo
  Wire.write(command); // Comando a leer
  Wire.endTransmission(false); // Finalizar la transmisión sin liberar el bus
  Wire.requestFrom(0x55, 1); // Solicitar un byte de datos
  return Wire.read(); // Leer el byte y devolverlo
}

uint16_t BQ34Z100_G1::readWord(uint8_t command) {
  Wire.beginTransmission(0x55); // Dirección del dispositivo
  Wire.write(command); // Comando a leer
  Wire.endTransmission(); // Finalizar la transmisión sin liberar el bus
  Wire.requestFrom(0x55, 2); // Solicitar un byte de datos
  uint16_t data = Wire.read();
  return data |= (Wire.read() << 8); // Combina el primer byte leído con el segundo byte (importante: el byte menos significativo debe leerse primero)
}


char *BQ34Z100_G1::Read(uint8_t address, uint8_t length)  //member of bq34z100  
{
  char *buffer = (char*)malloc (10);;
  uint16_t returnVal = 0; //revisar si se puede de 16 bits    
  Wire.beginTransmission(0x55);
  Wire.write(address);
  Wire.endTransmission();
  delay(5);
  Wire.requestFrom( (int)0x55, (int)length);
  delay(5);
  
  while(Wire.available()){
    //Wire.read();
    Wire.readBytes(buffer, length);
  }
    return buffer;
}

void BQ34Z100_G1::writeByte(uint8_t command, uint8_t data) {
  Wire.beginTransmission(0x55); // Dirección del dispositivo
  Wire.write(command); // Comando a escribir
  Wire.write(data); // Dato a escribir
  Wire.endTransmission(); // Finalizar la transmisión
}

void BQ34Z100_G1::unseal() {
  writeControl(UNSEAL_KEY_1); // Escribir la primera clave de desbloqueo
  writeControl(UNSEAL_KEY_2); // Escribir la segunda clave de desbloqueo
}

void BQ34Z100_G1::enableBlockDataControl() {
  writeByte(BLOCK_DATA_CONTROL, 0x00); // Escribir 0x00 al registro de control
}

void BQ34Z100_G1::selectDataFlashClass(uint8_t subclass) {
  writeByte(DATA_FLASH_CLASS, subclass); // Escribir el ID de la subclase
}

void BQ34Z100_G1::selectDataFlashBlock(uint8_t block) {
  writeByte(DATA_FLASH_BLOCK, block); // Escribir el offset del bloque
}

uint8_t BQ34Z100_G1::readBlockDataChecksum() {
  return readByte(BLOCK_DATA_CHECKSUM); // Leer el byte del checksum
}

void BQ34Z100_G1::writeBlockDataChecksum(uint8_t checksum) {
  writeByte(BLOCK_DATA_CHECKSUM, checksum); // Escribir el byte del checksum
}

//NO TOCAAAAAR------------------
uint8_t BQ34Z100_G1::readBlockDataByte(uint8_t offset) {
  return readByte(BLOCK_DATA + offset ); // Leer el byte del offset
}
//-------------------------------
uint8_t BQ34Z100_G1::readDataByte(uint8_t offset) {
  return readByte(BLOCK_DATA + (offset % 32)); // Leer el byte del offset
}

uint16_t BQ34Z100_G1::readBlockDataWord(uint8_t offset) {
  return 0;//Read(BLOCK_DATA + offset, 2); // Leer el byte del offset
}

void BQ34Z100_G1::writeBlockDataByte(uint8_t offset, uint8_t data) {
  writeByte(BLOCK_DATA + offset, data); // Escribir el byte al offset
}

void BQ34Z100_G1::modifyDataFlash() {
  unseal(); // Desbloquear el dispositivo
  enableBlockDataControl(); // Habilitar el control de la memoria flash de datos
  selectDataFlashClass(PACK_CONFIG_SUBCLASS); // Seleccionar la subclase de configuración de pack
  selectDataFlashBlock(0x00); // Seleccionar el primer bloque de la subclase
  uint8_t oldPackConfigMSB = readBlockDataByte(PACK_CONFIG_OFFSET); // Leer el byte alto de la configuración de pack
  uint8_t oldChecksum = readBlockDataChecksum(); // Leer el checksum del bloque
  uint8_t newPackConfigMSB = 0x10; //oldPackConfigMSB | VOLTSEL_BIT; // Establecer el bit de selección de voltaje
  writeBlockDataByte(PACK_CONFIG_OFFSET, newPackConfigMSB); // Escribir el nuevo byte alto de la configuración de pack
  uint8_t temp = (255 - oldChecksum - oldPackConfigMSB) % 256; // Calcular el valor temporal
  uint8_t newChecksum = 255 - (temp + newPackConfigMSB) % 256; // Calcular el nuevo checksum
  writeBlockDataChecksum(newChecksum); // Escribir el nuevo checksum del bloque
}


uint16_t BQ34Z100_G1::getOldPackConfiguration() {
  unseal();
  enableBlockDataControl();
  selectDataFlashClass(PACK_CONFIG_SUBCLASS);
  selectDataFlashBlock(0x00);
  uint8_t oldPackConfigLSB = readBlockDataByte(PACK_CONFIG_OFFSET); // Leer el byte bajo de la configuración de paquete
  uint8_t oldPackConfigMSB = readBlockDataByte(PACK_CONFIG_OFFSET + 1); // Leer el byte alto de la configuración de paquete
  uint16_t packConfigValue = (oldPackConfigLSB << 8) | oldPackConfigMSB; // Combinar los bytes en el orden inverso
  return packConfigValue; // Devolver el valor de 16 bits
}

uint16_t BQ34Z100_G1::getNewPackConfiguration() {
  unseal(); // Desbloquear el dispositivo
  uint16_t oldPackConfigMSB = 0;
  enableBlockDataControl(); // Habilitar el control de la memoria flash de datos
  selectDataFlashClass(PACK_CONFIG_SUBCLASS); // Seleccionar la subclase de configuración de pack
  selectDataFlashBlock(0x00); // Seleccionar el primer bloque de la subclase
  oldPackConfigMSB = readDataByte(PACK_CONFIG_OFFSET); // Leer el byte alto de la configuración de pack
  oldPackConfigMSB | 256*readDataByte(PACK_CONFIG_OFFSET); //oldPackConfigMSB | VOLTSEL_BIT; // Establecer el bit de selección de voltaje y devolver el nuevo byte alto de la configuración de pack
  return oldPackConfigMSB;
}

/*
uint8_t BQ34Z100_G1::getOldPackConfiguration() {
  unseal(); // Desbloquear el dispositivo
  enableBlockDataControl(); // Habilitar el control de la memoria flash de datos
  selectDataFlashClass(PACK_CONFIG_SUBCLASS); // Seleccionar la subclase de configuración de pack
  selectDataFlashBlock(0x00); // Seleccionar el primer bloque de la subclase
  return readBlockDataByte(PACK_CONFIG_OFFSET); // Leer y devolver el byte alto de la configuración de pack
}

uint8_t BQ34Z100_G1::getNewPackConfiguration() {
  unseal(); // Desbloquear el dispositivo
  enableBlockDataControl(); // Habilitar el control de la memoria flash de datos
  selectDataFlashClass(PACK_CONFIG_SUBCLASS); // Seleccionar la subclase de configuración de pack
  selectDataFlashBlock(0x00); // Seleccionar el primer bloque de la subclase
  uint8_t oldPackConfigMSB = readBlockDataByte(PACK_CONFIG_OFFSET); // Leer el byte alto de la configuración de pack
  return oldPackConfigMSB | VOLTSEL_BIT; // Establecer el bit de selección de voltaje y devolver el nuevo byte alto de la configuración de pack
}
*/

char* BQ34Z100_G1::Status()
{
  unseal(); // Desbloquear el dispositivo
  enableBlockDataControl(); // Habilitar el control de la memoria flash de datos
  selectDataFlashClass(PACK_CONFIG_SUBCLASS); // Seleccionar la subclase de configuración de paquete
  selectDataFlashBlock(0x00); // Seleccionar el primer bloque de la subclase
  char *PackConfig = (char*)malloc (10);
  PackConfig = Read(PACK_CONFIG_OFFSET, 2); // Leer los 2 bytes de la configuración de paquete
  //Serial.println (PackConfig);
  return PackConfig; // Devolver los 16 bits de la configuración de paquete
}