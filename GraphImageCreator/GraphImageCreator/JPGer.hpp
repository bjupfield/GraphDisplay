#ifndef JPGER_HPP
#define JPGER_HPP
#include "GraphPresets.hpp"
#include "Mcuer.hpp"

void graphMaptoJPG(graphMap map);

class intMcus {
public:
	int* Y;
	int* Cb;
	int* Cr;
	intMcus() : Y(nullptr), Cb(nullptr), Cr(nullptr) {}
	intMcus(MCU uintMcu, dimensions dim);
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