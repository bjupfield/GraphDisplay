#include <algorithm>
#include <stdint.h>
#include "Mcuer.hpp"
#include "GraphPresets.hpp"
#include "DumbFuncsNClasses.hpp"

uint8_t* extractYCbCr(Pixel* pixel, int size);

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
			uint8_t* undivided = extractYCbCr(buffer, 64);
			uint8_t* Yvalue = new uint8_t[64];
			uint8_t* Cbvalue = new uint8_t[64];//decoder im testing with doesnt account for 4:2:0
			uint8_t* Crvalue = new uint8_t[64];

			copyArray(undivided, Yvalue, 0, 64);
			copyArray(undivided, Cbvalue, 0, 64, 64);
			copyArray(undivided, Crvalue, 0, 64, 128);
			/*for(int k = 0; k < 16; k++)
			{
				Cbvalue[k] = (uint8_t)(((int)undivided[64 + k * 4] + (int)undivided[64 + k * 4 + 1] + (int)undivided[64 + k * 4 + 16] + (int)undivided[64 + k * 4 + 16 +1]) / 4);
			}
			for (int k = 0; k < 16; k++)
			{
				Crvalue[k] = (uint8_t)(((int)undivided[128 + k * 4] + (int)undivided[128 + k * 4 + 1] + (int)undivided[128 + k * 4 + 16] + (int)undivided[128 + k * 4 + 16 + 1]) / 4);
			}*/


			myMcus.mcuList[j * mcuLength + i].fillMcu(Yvalue, Cbvalue, Crvalue);
			//if (j * mcuLength + i == 0)
			//{
			//	std::cout << "PLZZZZ\n";

			//	for(int c = 0; c < 8; c++)
			//	{
			//		for (int d = 0; d < 8; d++)
			//		{
			//			std::cout << "(" << (int)Yvalue[c * 8 + d] << ", " << (int)myMcus.mcuList[0].ycbcr[0][c * 8 + d] << "), ";
			//		}
			//		std::cout << std::endl;
			//	}

			//	std::cout << "PLZZZZ\n";
			//}
			delete[] buffer;
			delete[] undivided;
			//delete[] Yvalue;
			//delete[] Cbvalue;
			//delete[] Crvalue;
			//if (j * mcuLength + i == 0)
			//{
			//	std::cout << "PLZZZZ\n";

			//	for (int c = 0; c < 8; c++)
			//	{
			//		for (int d = 0; d < 8; d++)
			//		{
			//			std::cout << "(" << (int)myMcus.mcuList[0].ycbcr[0][c * 8 + d] << "), ";
			//		}
			//		std::cout << std::endl;
			//	}

			//	std::cout << "PLZZZZ\n";
			//}
		}
	}
	return myMcus;
}
void printMcus(MCUS mine)
{

	//for(int i = 0; i < mine.retrieveCount(); i++)
	//{
	//	std::cout << std::endl << "MCU NUMBER:" << i << ": ";
	//	for(int j = 0; j < mine.retrieveDim().Y; j++)
	//	{
	//		for(int z = 0; z < mine.retrieveDim().Y; z++)
	//		{
	//			std::cout << (int)mine.mcuList[i].Y[j] << ",  ";
	//		}
	//	}
	//	
	//}
	std::cout << std::endl << "Dimension: " << mine.retrieveDim().Y << std::endl;
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
void MCU::fillMcu(uint8_t* y, uint8_t* cb, uint8_t* cr)
{
	this->ycbcr.Y = y;
	this->ycbcr.Cb = cb;
	this->ycbcr.Cr = cr;
}
uint8_t* extractYCbCr(Pixel* pixel, int size) {
	uint8_t* myYCbCrArray = new uint8_t[size * 3];
	for(int i = 0, j = size, k = size + size; i < size; i++, k++, j++)
	{
		float nB = pixel[i].blue / 255;
		float nR = pixel[i].red / 255;
		float nG = pixel[i].green / 255;

		float eG = .587 * nG;
		float eB = .114 * nB;
		float eY = .299 * nR + eG + eB;

		//int Y = std::min(255, std::max(0, (int)(eY * 255)));
		//int Cb = std::min(255, std::max(0, (int)((((nR - eY) * 255) / 1.402) + 128)));
		//int Cr = std::min(255, std::max(0, (int)((((nB - eY) * 255) / 1.772) + 128)));

		//int Y = std::min(255, std::max(16 + (int)(65.48 * pixel[i].red) + (int)(128.553 * pixel[i].green) + (int)(24.966 * pixel[i].blue), 0));
		//int Cb = std::min(255, std::max(128 + (int)(-37.797 * pixel[i].red) + (int)(74.203 * pixel[i].green) + (int)(112 * pixel[i].blue), 0));
		//int Cr = std::min(255, std::max(128 + (int)(112 * pixel[i].red) + (int)(93.786 * pixel[i].green) + (int)(18.214 * pixel[i].blue), 0));

		int Cb = 0;
		int Cr = 0;

		int Y = pixel[i].green;

		myYCbCrArray[i] = (uint8_t)Y;
		myYCbCrArray[j] = (uint8_t)Cb;
		myYCbCrArray[k] = (uint8_t)Cr;
	}
	return myYCbCrArray;
};