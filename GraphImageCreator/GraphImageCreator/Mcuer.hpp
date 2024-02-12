#ifndef MCUER_HPP
#define MCUER_HPP
#include <iostream>
#include <iomanip>
#include "GraphPresets.hpp"

struct dimensions {
	int Y = 8;
	int Cb = 4;
	int Cr = 4;
};
class MCU
{
public:
	uint8_t* Y;
	uint8_t* Cb;
	uint8_t* Cr;
	MCU() : Y(nullptr), Cb(nullptr), Cr(nullptr) {}
	MCU(int YDim, int CbDim, int CrDm);
	void fillMcu(uint8_t* y, uint8_t* cb, uint8_t* cr);

};
class MCUS{
private:
	int mcuHeight;
	int mcuLength;
	dimensions dim;
public:
	MCU* mcuList;
	int retrieveCount();
	int retrieveLength();
	int retrieveHeight();
	dimensions retrieveDim();
	MCUS(int height, int length, int YDim, int CbDim, int CrDim);
};
MCUS graphPresetToMcus(graphMap Map);
void printMcus(MCUS mine);
#endif