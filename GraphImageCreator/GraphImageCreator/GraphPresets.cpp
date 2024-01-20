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
					std::cout << "CurPixel || RED: " << curPixel.red << " || GREEN: " << curPixel.green << " || BLUE: " << curPixel.blue << std::endl;
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
char ** accessImage::pathNames = nullptr;
graphPresets* accessImage::images = nullptr;
graphPresets accessImage::recieveOrCreate(char* fileName) {
	int pathNameSize = sizeof(pathNames);
	std::cout << "PathNamesSize: " << pathNameSize << std::endl;
	if (pathNames != nullptr) {
		bool found = false;
		int i;
		for (i = 0; i < pathNameSize; i++)
		{
			if (fileName == pathNames[i]) found = true;
		}
		if (found == false) {
			pathNames = (char**)realloc(pathNames, sizeof(char*) + sizeof(pathNames));
			pathNames[i] = fileName;
			if (sizeof(images) > 0) {
				images = (graphPresets*)realloc(images, sizeof(images) + sizeof(graphPresets));
				images[i] = graphPresets(1, new char* [1] {fileName});
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