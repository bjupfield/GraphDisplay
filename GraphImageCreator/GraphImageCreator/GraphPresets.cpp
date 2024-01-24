#include <iostream>
#include <fstream>
#include "GraphPresets.hpp"


graphPresets::graphPresets(int fileAmount, char** fileNames) {

	//this function creats and saves a graphpreset for future use
	//Im just saving it in a static class so for memory,
	//i dont know it seems like what I should do

	std::ifstream openFile;
	char* buffer = new char[20];
	for (int i = 0; i < fileAmount; i++)
	{
		openFile.open(fileNames[i]);
		if (openFile.is_open())
		{
			openFile.seekg(0, openFile.end);
			int fileLength = openFile.tellg();
			openFile.seekg(0, openFile.beg);


			//this just gets the size of the file
			//because its text files I just stored it in
			//the first line in the format "Height x Line"
			openFile.getline(buffer, 20);
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

			//create pixels array based on the height and length defined in the first line
			image = (Pixel*)malloc(imageHeight * imageLength * sizeof(Pixel));
			delete[] buffer;


			//iterates through file for all the pixels,
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
int accessImage::pathNamesSize = 0;
int accessImage::imagesSize = 0;
graphPresets accessImage::recieveOrCreate(char* fileName) {
	//checks to see if this file exist and it does returns it, if it does not creates it
	if (pathNames != nullptr) {
		bool found = false;
		int i;
		if (found == false) {
			for (i = 0; i < pathNamesSize; i++)
			{
				if (fileName == pathNames[i]) found = true;
			}
		}
		if (found == false) {
			pathNames = (char**)realloc(pathNames, sizeof(char*) * pathNamesSize);
			pathNames[i] = fileName;
			pathNamesSize += 1;
			if (sizeof(images) > 0) {
				imagesSize += 1;
				images = (graphPresets*)realloc(images, sizeof(graphPresets) * imagesSize);
				images[i] = graphPresets(1, new char* [1] {fileName});
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
		pathNamesSize = 1;
		imagesSize = 1;
	}
}
void accessImage::currentImages() {
	std::cout << "iS THIS WHAT IT IS" << std::endl;
}
int accessImage::retrieveLabel(char* fileName)
{
	//get label
	bool found = false;
	int i;
	if(pathNames != nullptr)
	{
		if (found == false) 
		{
			for (i = 0; i < pathNamesSize; i++)
			{
				if (strcmp(fileName, pathNames[i]) == 0) 
				{ 
					found = true; 
					break;
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
		return accessImage::retrieveLabel(fileName);
	}
}
graphMap::graphMap(int Height, int Length, uint8_t map)
{
	this->mapHeight = Height;
	this->mapLength = Length;

}