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
	int retrieveLength();
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
	static graphPresets graphPresetLabelled(int label);
};
class graphMap {
private:
	int mapHeight;
	int mapLength;
	int* mapMap;
	/*
	lol, maplabel and pathnameslabels are linked arrays which refer to each other, uint8_t maplabel defines the short term references held in mapmap
	mapMap holds the actual map, which can be any ratio due to mapHeight and mapLength
	*/
public:
	//initializer,  only assign height and length, assign map later with labels after retrieving the labels with pathtolabel
	graphMap(int Height, int Length);
	//used to retrieve labels from static class
	int pathToLabel(char* fileName);
	//assigns graph array
	void labelMap(int* map);
	//retrieves graph array
	int* retrieveMap();
	//retrieves image dimensions, in a size 2 int array	
	int* retrieveDimensions();
};

#endif





// okay the problem Im having is Im trying to read the size of an array, which cannot be done...
//dont do that you have to save the size of an array, very mean...