#include <iostream>
#include <fstream>
#include "JPGer.hpp"

using namespace std;

//all tables are provided by the courtesy of Nicolas Robidoux
//at 
//https://legacy.imagemagick.org/discourse-server/viewtopic.php?t=20333

static const unsigned int lumTable[64] =
{
    16, 12, 12, 15, 22, 33, 55, 75,
    12, 12, 14, 18, 25, 48, 68, 81,
    12, 14, 16, 24, 40, 60, 81, 83,
    15, 18, 24, 30, 57, 81, 93, 90,
    22, 25, 40, 57, 74, 104, 117, 109,
    33, 48, 60, 81, 104, 118, 136, 115,
    55, 68, 81, 93, 117, 136, 144, 126,
    75, 81, 83, 90, 109, 115, 126, 126
};

static const unsigned int chromaTable[64] =
{
    17, 18, 24, 47, 99, 128, 192, 256,
    18, 21, 26, 66, 99, 128, 192, 256,
    24, 26, 56, 99, 128, 192, 256, 512,
    47, 66, 99, 128, 192, 256, 512, 1024,
    99, 99, 128, 192, 256, 512, 1024, 2048,
    128, 128, 192, 256, 512, 1024, 3072, 4096,
    192, 192, 256, 512, 1024, 3072, 6144, 7168,
    256, 256, 512, 1024, 2048, 4096, 7168, 8192
};

	void graphMaptoJPG(graphMap map)
	{
		
        

        ofstream myJpg;
        myJpg.open("../../yahDig.jpg", ios::binary);

        if (myJpg.is_open())
        {
            cout << "HI JPG IS OPEN" << endl;
            //header and appo file write, standard information does not change
            uint8_t headerData[] = { (uint8_t)255, (uint8_t)216, (uint8_t)255, (uint8_t)224, (uint8_t)0, (uint8_t)16, (uint8_t)74, (uint8_t)70, (uint8_t)73,  (uint8_t)70, (uint8_t)0};
            myJpg.write((const char*)headerData, sizeof(headerData));
            //jfif version density and thumbnail data, should remain the same
            uint8_t versionDensityThumbData[] = { (uint8_t)1, (uint8_t)1, (uint8_t)0, (uint8_t)0, (uint8_t)1, (uint8_t)0, (uint8_t)1, (uint8_t)0, (uint8_t)0 };
            myJpg.write((const char*)versionDensityThumbData, sizeof(versionDensityThumbData));

            myJpg.close();

            //myJpg << (uint8_t)255;
            //myJpg << (uint8_t)216;
            //myJpg << (uint8_t)255;
            //myJpg << (uint8_t)224;
            //myJpg << (uint8_t)0;
            //myJpg << (uint8_t)16;
            //myJpg << (uint8_t)74;
            //myJpg << (uint8_t)70;
            //myJpg << (uint8_t)73;
            //myJpg << (uint8_t)70;
            //myJpg << (uint8_t)0;

        }

	}