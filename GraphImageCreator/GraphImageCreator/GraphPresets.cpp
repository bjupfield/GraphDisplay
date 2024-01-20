#include <iostream>
#include <fstream>
#include "GraphPresets.hpp"


graphPresets::graphPresets(int fileAmount, char** fileNames, int* map) {
	imageSize = new int[fileAmount];
	image = new Pixel * [fileAmount];
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
			int height = 0;
			int length = 0;
			char b = '0';
			int c = 0;
			while(b != 'x' && c < openFile.showpos)
			{
				b = buffer[c];
				if (b != 'x') 
				{
					height = height * 10 + (int)b - 48;
				}
				c++;
			}
			while(c < openFile.showpos / 8 + 1)
			{
				b = buffer[c];
				length = length * 10 + (int)b - 48;
				c++;
			}
			std::cout << "Height: " << height << std::endl;
			std::cout << "Length: " << length << std::endl;
			for (int i = 0; i < height; i++) 
			{
				for(int j = 0; j < length; j++)
				{
					char smallerBuffer = 'c';
					int count = 0;
					while (openFile.get(smallerBuffer) && smallerBuffer != ')')
					{
						if (smallerBuffer != ')')
						{
							buffer[count] = smallerBuffer;
						}
						count++;
					}
					std::cout << buffer << std::endl;
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