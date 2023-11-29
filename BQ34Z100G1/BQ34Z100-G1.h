// BQ34Z100-G1.h

#ifndef BQ34Z100_G1_H
#define BQ34Z100_G1_H

#include <Wire.h>
#include <stdlib.h>
#include <stdio.h>

class BQ34Z100_G1 {
public:
  BQ34Z100_G1(uint8_t sda, uint8_t scl);
  void modifyDataFlash();
  uint16_t getOldPackConfiguration();
  uint16_t getNewPackConfiguration();
  char* Status();

private:
  uint8_t _sda, _scl;
  void writeControl(uint16_t command);
  uint8_t readByte(uint8_t command);
  uint16_t readWord(uint8_t command);
  char *Read(uint8_t address, uint8_t length); 
  void writeByte(uint8_t command, uint8_t data);
  void unseal();
  void enableBlockDataControl();
  void selectDataFlashClass(uint8_t subclass);
  void selectDataFlashBlock(uint8_t block);
  uint8_t readBlockDataChecksum();
  void writeBlockDataChecksum(uint8_t checksum);
  uint8_t readBlockDataByte(uint8_t offset);
  uint8_t readDataByte(uint8_t offset);
  uint16_t readBlockDataWord(uint8_t offset);
  void writeBlockDataByte(uint8_t offset, uint8_t data);
};

#endif // BQ34Z100_G1_H


