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
	int* imageSize;
	Pixel** image;
public:
	graphPresets(int fileAmount, char** fileNames, int* map);
};

#endif