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
	fakeDictionary<int, int> DCcodeLength;
	fakeDictionary<int, uint8_t> DCcode;
	fakeDictionary<int, int> ACcodeLength;
	fakeDictionary<int, uint8_t> ACcode;
	huffmanTable() : DCcodeLength(fakeDictionary<int, int>()), ACcodeLength(fakeDictionary<int, int>()), DCcode(fakeDictionary<int, uint8_t>()), ACcode(fakeDictionary<int, uint8_t>()) {}
	void frequency(int dim, int* table);
	void huffmanCodes();
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