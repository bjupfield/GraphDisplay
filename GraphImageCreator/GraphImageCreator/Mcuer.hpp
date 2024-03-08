#ifndef MCUER_HPP
#define MCUER_HPP
#include <iostream>
#include <iomanip>
#include "GraphPresets.hpp"

struct dimensions {
	int Y = 8;
	int Cb = 4;
	int Cr = 4;
	int operator[](int i)
	{
		switch (i)
		{
		case 0:
			return Y;
		case 1:
			return Cb;
		case 2:
			return Cr;
		default:
			return Y;
		}
	}
};
struct YCBCR {

	uint8_t* Y;
	uint8_t* Cb;
	uint8_t* Cr;

	uint8_t* operator[](int i)
	{
		switch (i)
		{
		case 0:
			return Y;
		case 1:
			return Cb;
		case 2:
			return Cr;
		default:
			return Y;
		}
	}
};
class MCU
{
public:
	YCBCR ycbcr;
	MCU() : ycbcr{nullptr, nullptr, nullptr} {}
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