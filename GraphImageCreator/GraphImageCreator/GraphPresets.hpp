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

#endif