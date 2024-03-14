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

			if (j < 3 && i == 0)
			{
				std::cout << "Cr: \n";
				for (int n = 0; n < 8; n++)
				{
					for (int b = 0; b < 8; b++)
					{
						std::cout << undivided[n * 8 + b + 128] << ", ";
					}
					std::cout << "\n";
				}
			}

			copyArray(undivided, Yvalue, 0, 64, 0);
			copyArray(undivided, Cbvalue, 0, 64, 64);
			copyArray(undivided, Crvalue, 0, 64, 128);
			
			if (j < 3 && i == 0)
			{
				std::cout << "Cr Post Transfer: \n";
				for (int n = 0; n < 8; n++)
				{
					for (int b = 0; b < 8; b++)
					{
						std::cout << Crvalue[n * 8 + b] << ", ";
					}
					std::cout << "\n";
				}
			}
			if (j < 3 && i == 0)
			{
				std::cout << "G Post Transfer: \n";
				for (int n = 0; n < 8; n++)
				{
					for (int b = 0; b < 8; b++)
					{
						std::cout << buffer[n * 8 + b].red << ", ";
					}
					std::cout << "\n";
				}
			}

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
		//float Ey = (pixel[i].green + (Kr * (pixel[i].red - pixel[i].green)) + (Kb * (pixel[i].blue - pixel[i].green))) / 255;
		//float Eb = (pixel[i].blue - Ey) / (largeKb);
		//float Er = (pixel[i].red - Ey) / (largeKr);


		//int Y = std::min(128, std::max(-128, (int)(219 * Ey)));
		//int Cb = std::min(128, std::max(-128, (int)(224 * Eb)));
		//int Cr = std::min(128, std::max(-128, (int)(224 * Er)));

		//int Y = (int)(219 * Ey);
		//int Cb = (int)(224 * Eb);
		//int Cr = (int)(224 * Er);

		//int Y = std::min(255, std::max(16 + (int)(65.48 * pixel[i].red) + (int)(128.553 * pixel[i].green) + (int)(24.966 * pixel[i].blue), 0));
		//int Cb = std::min(255, std::max(128 + (int)(-37.797 * pixel[i].red) + (int)(74.203 * pixel[i].green) + (int)(112 * pixel[i].blue), 0));
		//int Cr = std::min(255, std::max(128 + (int)(112 * pixel[i].red) + (int)(93.786 * pixel[i].green) + (int)(18.214 * pixel[i].blue), 0));

		//int Cb = 0;
		//int Cr = 0;

		//int Y = pixel[i].green;

		int Y = (int)(pixel[i].red * Yr + pixel[i].green * Yg + pixel[i].blue * Yb) - 128;
		int Cb = -(int)((pixel[i].red) * Cbr + (pixel[i].green) * Cbg + (pixel[i].blue) * Cbb);
		int Cr = +(int)((pixel[i].red) * Crr + (pixel[i].green) * Crg + (pixel[i].blue) * Crb);

		myYCbCrArray[i] = Y;
		myYCbCrArray[j] = Cb;
		myYCbCrArray[k] = Cr;
	}
	return myYCbCrArray;
};