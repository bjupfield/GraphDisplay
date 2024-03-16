#include <iostream>
#include <fstream>
#include "GraphPresets.hpp"

graphPresets::graphPresets(int fileAmount, char** fileNames) {

	//this function creats and saves a graphpreset for future use
	//Im just saving it in a static class so for memory,
	//i dont know it seems like what I should do

	this->exists = true;

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
			this->exists = false;
			std::cout << "DID NOT OPEN: " << std::endl;
		}
		openFile.close();
	}
}
bool graphPresets::exist()
{
	return this->exists;
}
int graphPresets::retrieveHeight()
{
	return this->imageHeight;
}
int graphPresets::retrieveLength()
{
	return this->imageLength;
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
		return images[0];
	}
}
Pixel graphPresets::retrievePixel(int pos)
{
	return image[pos];
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
		graphPresets b = accessImage::recieveOrCreate(fileName);
		if (b.exist() == false) return -1;
		return accessImage::retrieveLabel(fileName);
	}
}
graphPresets accessImage::graphPresetLabelled(int label)
{
	return images[label];
}
Pixel accessImage::retrievePixel(int label, int pos) 
{
	return images[label].retrievePixel(pos);
}
graphMap::graphMap(int Length, int Height)
{
	this->mapHeight = Height;
	this->mapLength = Length;
}
int graphMap::pathToLabel(char* fileName)
{
	return accessImage::retrieveLabel(fileName);
}
void graphMap::labelMap(int* map)
{
	this->mapMap = map;
}
int* graphMap::retrieveMap()
{
	return this->mapMap;
}
int* graphMap::retrieveDimensions()
{
	int graphPixelLength = -1;
	int graphPixelHeight = -1;
	for(int i = 0; i < this->mapHeight; i++)
	{
		for(int j = 0; j < this->mapLength; j++)
		{
			int length = accessImage::graphPresetLabelled(this->mapMap[j + (i * this->mapLength)]).retrieveLength();
			int height = accessImage::graphPresetLabelled(this->mapMap[j + (i * this->mapLength)]).retrieveHeight();
			if (length > graphPixelLength) graphPixelLength = length;
			if (height > graphPixelHeight) graphPixelHeight = height;
		}
	}
	return new int[2] { graphPixelHeight * this->mapHeight, graphPixelLength * this->mapLength };
}
Pixel graphMap::retrievePixel(int pos)
{
	Pixel n = { 0, 0, 0 };
	int* heightLength = this->retrieveDimensions();
	int heightMod = heightLength[1] * (heightLength[0] / mapHeight);

	//find which graph preset image inside the graphmap the pixel is in
	int heightPos = pos / heightMod;
	int lengthPos = ((pos % heightMod) % heightLength[1]) / (heightLength[1] / this->mapLength);

	//find where the pixel is inside this image
	int YPixelPosInImage = ((pos % heightMod) / heightLength[1]);
	int XPixelPosInImage = ((pos % heightMod) % heightLength[1]) % (heightLength[1] / this->mapLength);

	//retrieve pixel form graphpreset
	n = accessImage::retrievePixel(this->mapMap[heightPos * mapLength + lengthPos], (YPixelPosInImage * (heightLength[1] / mapLength)) + XPixelPosInImage);

	return n;
}
