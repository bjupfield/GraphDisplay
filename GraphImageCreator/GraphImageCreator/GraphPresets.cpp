#include <iostream>
#include <fstream>
#include "GraphPresets.hpp"


graphPresets::graphPresets(int fileAmount, char** fileNames) {
	std::ifstream openFile;
	char* buffer = new char[20];
	for (int i = 0; i < fileAmount; i++)
	{
		openFile.open(fileNames[i]);
		if (openFile.is_open())
		{
			std::cout << "Is Open" << std::endl;
			openFile.seekg(0, openFile.end);
			int fileLength = openFile.tellg();
			openFile.seekg(0, openFile.beg);
			openFile.getline(buffer, 20);
			std::cout << "FileLength: " << fileLength << std::endl;
			std::cout << "Line: " << buffer << std::endl;
			imageHeight = 0;
			imageLength = 0;
			char b = '0';
			int c = 0;
			while(b != 'x' && c < openFile.showpos)
			{
				b = buffer[c];
				if (b != 'x') 
				{
					imageHeight = imageHeight * 10 + (int)b - 48;
				}
				c++;
			}
			while(c < openFile.showpos / 8 + 1)
			{
				b = buffer[c];
				imageLength = imageLength * 10 + (int)b - 48;
				c++;
			}
			std::cout << "Height: " << imageHeight << std::endl;
			std::cout << "Length: " << imageLength << std::endl;
			image = (Pixel*)malloc(imageHeight * imageLength * sizeof(Pixel));
			delete[] buffer;
			for (int i = 0; i < imageHeight; i++)
			{
				for(int j = 0; j < imageLength; j++)
				{
					Pixel curPixel = {0,0,0};
					char smallerBuffer = 'c';
					int count = 0;
					int whichOne = 0;
					bool previousBufferInt = false;
					while (openFile.get(smallerBuffer) && smallerBuffer != ')')
					{
						if ((int)smallerBuffer > 47 && (int)smallerBuffer < 58)
						{
							previousBufferInt = true;
							switch (whichOne) 
							{
							case 0:
							{
								curPixel.red = curPixel.red * 10 + (int)smallerBuffer - 48;
								break;
							}
							case 1:
							{
								curPixel.green = curPixel.green * 10 + (int)smallerBuffer - 48;
								break;
							}
							case 2:
							{
								curPixel.blue = curPixel.blue * 10 + (int)smallerBuffer - 48;
								break;
							}
							default:
							{
								std::cout << "FAILURE" << std::endl;
							}
							}
						}
						else if(previousBufferInt)
						{
							whichOne += 1;
							previousBufferInt = false;
						}
						count++;
					}
					image[j + i * imageLength] = curPixel;
					//std::cout << "CurPixel || RED: " << curPixel.red << " || GREEN: " << curPixel.green << " || BLUE: " << curPixel.blue << std::endl;
				}
			}
		}
		else
		{
			std::cout << "DID NOT OPEN: " << std::endl;
		}
		openFile.close();
	}
}
int graphPresets::retrieveHeight()
{
	return this->imageHeight;
}
char ** accessImage::pathNames = nullptr;
graphPresets* accessImage::images = nullptr;
graphPresets accessImage::recieveOrCreate(char* fileName) {
	int pathNameSize = sizeof(pathNames) / sizeof(char*);
	std::cout << "PathNamesSize: " << pathNameSize << std::endl;
	if (pathNames != nullptr) {
		bool found = false;
		int i;
		if (found == false) {
			for (i = 0; i < pathNameSize; i++)
			{
				if (fileName == pathNames[i]) found = true;
			}
		}
		if (found == false) {
			pathNames = (char**)realloc(pathNames, sizeof(char*) + sizeof(pathNames));
			pathNames[i] = fileName;
			std::cout << "Position: " << i << std::endl;
			std::cout << "PathName: " << pathNames[i] << std::endl;
			std::cout << "PathName: " << pathNames[0] << std::endl;
			pathNameSize = sizeof(pathNames) / sizeof(char*);
			std::cout << "new Size: " << pathNameSize << std::endl;
			if (sizeof(images) > 0) {
				images = (graphPresets*)realloc(images, sizeof(images) + sizeof(graphPresets));
				images[i] = graphPresets(1, new char* [1] {fileName});
				std::cout << "HMMM" << std::endl;
				std::cout << "New GraphPreset Size: " << images[i].retrieveHeight() << std::endl;
			}
			else {
				std::cout << "iS THIS WHAT IT IS" << std::endl;
			}
			
			return images[i];
		}
		else {
			return images[i];
		}
	}
	else
	{
		pathNames = (char**)malloc(sizeof(char*));
		pathNames[0] = fileName;
		images = (graphPresets*)malloc(sizeof(graphPresets));
		images[0] = graphPresets(1, new char* [1] {fileName});
	}
}
void accessImage::currentImages() {
	std::cout << "iS THIS WHAT IT IS" << std::endl;
}
uint8_t accessImage::retrieveLabel(char* fileName)
{
	bool found = false;
	int i;
	if(pathNames != nullptr)
	{
		int pathNameSize = sizeof(pathNames) / sizeof(char*);
		if (found == false) 
		{
			std::cout << "PathNamesSize: " << pathNameSize << std::endl;
			for (i = 0; i < pathNameSize; i++)
			{
				std::cout << "fileNamme: " << fileName << std::endl;
				std::cout << "pathName[i]: " << pathNames[i] << std::endl;
				if (strcmp(fileName, pathNames[i]) == 0) 
				{ 
					found = true; 
				}
			}
		}
	}
	if(found)
	{
		return i;
	}
	else
	{
		accessImage::recieveOrCreate(fileName);
		std::cout << "Int I: " << i << std::endl;
		std::cout << "HERE!?!?!!?" << std::endl;
		return accessImage::retrieveLabel(fileName);
	}
}
graphMap::graphMap(int Height, int Length, uint8_t map)
{
	this->mapHeight = Height;
	this->mapLength = Length;

}