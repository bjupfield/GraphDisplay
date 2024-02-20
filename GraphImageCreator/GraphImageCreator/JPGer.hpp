#ifndef JPGER_HPP
#define JPGER_HPP
#include "GraphPresets.hpp"
#include "Mcuer.hpp"
#include "DumbFuncsNClasses.hpp"

void graphMaptoJPG(graphMap map);

class intMcus {
public:
	int* Y;
	int* Cb;
	int* Cr;
	intMcus() : Y(nullptr), Cb(nullptr), Cr(nullptr) {}
	intMcus(MCU uintMcu, dimensions dim, int previousCoefficient);
};
class huffmanTable {
public:
	uint8_t amount[16];
	uint8_t sortedValue[255];
};
class mcuHuffmanContainer {
private:
	int mcuLength;
	int mcuHeight;
public:
	dimensions dim;
	intMcus* mcus;
	mcuHuffmanContainer(MCUS origin);
};
void testIntMcus(mcuHuffmanContainer mine, int num);

#endif