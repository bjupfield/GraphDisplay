#include <algorithm>
#include <stdint.h>
#include "Mcuer.hpp"
#include "GraphPresets.hpp"
#include "DumbFuncsNClasses.hpp"

int* extractYCbCr(Pixel* pixel, int size);

MCUS graphPresetToMcus(graphMap Map) {
	int *heightLength = Map.retrieveDimensions();
	int mcuHeight = heightLength[0] / 8;
	int mcuLength = heightLength[1] / 8;
	MCUS myMcus = MCUS(mcuHeight, mcuLength, 8, 8, 8);//decoder im testing doesnt account for 4:2:0 so dimesnions are 8,8,8

	for(int i = 0; i < mcuLength; i++)
	{
		for(int j = 0; j < mcuHeight; j++)
		{
			Pixel* buffer = new Pixel[64];
			int StartPos = i * 8 + (j * heightLength[1] * 8);

			for (int k = 0; k < 8; k++)
			{
				for(int z = 0; z < 8; z++)
				{
					buffer[k + (z * 8)] = Map.retrievePixel(StartPos + k + (z * heightLength[1]));
				}
				
			}
			int* undivided = extractYCbCr(buffer, 64);
			int* Yvalue = new int[64];
			int* Cbvalue = new int[64];//decoder im testing with doesnt account for 4:2:0
			int* Crvalue = new int[64];


			copyArray(undivided, Yvalue, 0, 64, 0);
			copyArray(undivided, Cbvalue, 0, 64, 64);
			copyArray(undivided, Crvalue, 0, 64, 128);
			

			myMcus.mcuList[j * mcuLength + i].fillMcu(Yvalue, Cbvalue, Crvalue);

			delete[] buffer;
			delete[] undivided;

		}
	}
	return myMcus;
}
MCUS::MCUS(int height, int length, int YDim, int CbDim, int CrDim)
	: 
	mcuHeight(height), mcuLength(length), dim{YDim, CbDim, CrDim}
	{
	

	this->mcuList = new MCU[height * length];

	for(int i = 0; i < height * length; i++)
	{
		mcuList[i] = MCU(YDim, CbDim, CrDim);
	}
}
int MCUS::retrieveCount()
{
	return mcuHeight * mcuLength;
}
int MCUS::retrieveLength()
{
	return mcuLength;
}
int MCUS::retrieveHeight()
{
	return mcuHeight;
}
dimensions MCUS::retrieveDim()
{
	return this->dim;
}
MCU::MCU(int YDim, int CbDim, int CrDm) {
	//this->ycbcr.Y = new uint8_t[YDim * YDim];
	//this->ycbcr.Cb = new uint8_t[CbDim * CbDim];
	//this->ycbcr.Cr = new uint8_t[CrDm * CrDm];
}
void MCU::fillMcu(int* y, int* cb, int* cr)
{
	this->ycbcr.Y = y;
	this->ycbcr.Cb = cb;
	this->ycbcr.Cr = cr;
}
//using conversion defined by jpg, honestly dont know where couldnt find it
//just used the jed previously linked equation, but couldnt find the equation in any of the itu recommendations
const float Yr = 0.299;
const float Yg = 0.587;
const float Yb = 0.114;
const float Cbr = -0.169;
const float Cbg = -0.331;
const float Cbb = 0.500;
const float Crr = 0.500;
const float Crg = -0.419;
const float Crb = -0.081;
int* extractYCbCr(Pixel* pixel, int size) {
	int* myYCbCrArray = new int[size * 3];
	for(int i = 0, j = size, k = size + size; i < size; i++, k++, j++)
	{
		int Y = (int)(pixel[i].red * Yr + pixel[i].green * Yg + pixel[i].blue * Yb) - 128;
		int Cb = -(int)((pixel[i].red) * Cbr + (pixel[i].green) * Cbg + (pixel[i].blue) * Cbb);
		int Cr = +(int)((pixel[i].red) * Crr + (pixel[i].green) * Crg + (pixel[i].blue) * Crb);

		myYCbCrArray[i] = Y;
		myYCbCrArray[j] = Cb;
		myYCbCrArray[k] = Cr;
	}
	return myYCbCrArray;
};