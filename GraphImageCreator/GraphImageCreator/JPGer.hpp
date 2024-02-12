#ifndef JPGER_HPP
#define JPGER_HPP
#include "GraphPresets.hpp"
#include "Mcuer.hpp"

void graphMaptoJPG(graphMap map);

class intMcus {
	int* Y;
	int* Cb;
	int* Cr;
public:
	intMcus() : Y(nullptr), Cb(nullptr), Cr(nullptr) {}
	intMcus(MCU uintMcu, dimensions dim);
};
class mcuHuffmanContainer {
private:
	int mcuLength;
	int mcuHeight;
	dimensions dim;
public:
	intMcus* mcus;
	mcuHuffmanContainer(MCUS origin);
};

#endif