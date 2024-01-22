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
};
static class accessImage {
private:
	static char** pathNames;
	static graphPresets* images;
public:
	static graphPresets recieveOrCreate(char* fileName);
	static void currentImages();
};
class graphMap {
private:
	int mapHeight;
	int mapLength;
	uint8_t* mapLabel;
	char** pathNamesLabel;
	uint8_t* mapMap;
	/*
	lol, maplabel and pathnameslabels are linked arrays which refer to each other, uint8_t maplabel defines the short term references held in mapmap
	mapMap holds the actual map, which can be any ratio due to mapHeight and mapLength
	*/
public:
	graphMap(int Height, int Length, char** filePaths, uint8_t* labels /*these labels correspond to the pathnames*/, uint8_t map /*1-D Array representing Two Dimensional Space*/);
	//dont know what to do here, creating the map needs a label, but labels need to be created in the function hmmm
};

#endif