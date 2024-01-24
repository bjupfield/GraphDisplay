#ifndef GRAPHPRESETS_HPP
#define GRAPHPRESETS_HPP

struct Pixel
{
	int red;
	int green;
	int blue;
};

class graphPresets {
private:
	int imageLength;
	int imageHeight;
	Pixel* image;
public:
	graphPresets(int fileAmount, char** fileNames);
	int retrieveHeight();
};
static class accessImage {
private:
	static char** pathNames;
	static int pathNamesSize;
	static graphPresets* images;
	static int imagesSize;
public:
	static graphPresets recieveOrCreate(char* fileName);
	static int retrieveLabel(char* fileName);
	//label is just the int that the pathname is stored in in the array
	static void currentImages();
};
class graphMap {
private:
	int mapHeight;
	int mapLength;
	uint8_t* mapMap;
	/*
	lol, maplabel and pathnameslabels are linked arrays which refer to each other, uint8_t maplabel defines the short term references held in mapmap
	mapMap holds the actual map, which can be any ratio due to mapHeight and mapLength
	*/
public:
	graphMap(int Height, int Length, uint8_t map /*1-D Array representing Two Dimensional Space*/);
	//dont know what to do here, creating the map needs a label, but labels need to be created in the function hmmm
};

#endif





// okay the problem Im having is Im trying to read the size of an array, which cannot be done...
//dont do that you have to save the size of an array, very mean...