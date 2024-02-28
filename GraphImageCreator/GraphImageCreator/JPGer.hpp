#ifndef JPGER_HPP
#define JPGER_HPP
#include "GraphPresets.hpp"
#include "Mcuer.hpp"
#include "DumbFuncsNClasses.hpp"

void graphMaptoJPG(graphMap map);
class intMcus {
public:
	struct intMcu
	{
		int* Y;
		int* Cb;
		int* Cr;

		int* operator[](int i)
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
				return nullptr;
			}
		}
		int*& operator()(int i)
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
	intMcu yCbCr;
	intMcus() : yCbCr{nullptr, nullptr, nullptr} {}
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
	huffmanTable* yCHuffman;
	mcuHuffmanContainer(MCUS origin);
};
struct hInfoStruct {
	int pixelHeight;//picture size
	int pixelLength;//picture size
	int samplingY;//y compression amount, standard is 1
	int samplingCr;//cr compression amount, standard is 2
	int samplingCb;//cb compression amount, standard is 2
	int densityY;//should almost always be 1
	int densityX;//should almost always be 1
	int componentNums;//should be 1 or 3, as this is the color components adn is either monchrome or not...

};
void testIntMcus(mcuHuffmanContainer mine, int num);
void actualJpg(hInfoStruct hInfo, mcuHuffmanContainer mcuHuffman, char *fileName);

#endif