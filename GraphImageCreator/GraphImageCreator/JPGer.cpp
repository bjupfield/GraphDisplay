#include <iostream>
#include <fstream>
#include "JPGer.hpp"

using namespace std;

//all tables are provided by the courtesy of Nicolas Robidoux
//at 
//https://legacy.imagemagick.org/discourse-server/viewtopic.php?t=20333

static const uint8_t lumTable[64] =
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

static const uint8_t chromaTable[64] =
{
    17, 18, 24, 47, 99, 128, 192, 256,
    18, 21, 26, 66, 99, 128, 192, 256,
    24, 26, 56, 99, 128, 192, 256, 512,
    47, 66, 99, 128, 192, 256, 512, 1024,
    99, 99, 128, 192, 256, 512, 1024, 2048,
    128, 128, 192, 256, 512, 1024, 3072, 4096,
    192, 192, 256, 512, 1024, 3072, 6144, 7168,
    256, 256, 512, 1024, 2048, 4096, 7168, 8192
};//strange possibly unreadable values

void graphMaptoJPG(graphMap map)
	{
		
        

        ofstream myJpg;
        myJpg.open("../../yahDig.jpg", ios::binary);

        if (myJpg.is_open())
        {
            cout << "HI JPG IS OPEN" << endl;
            //header and appo file write, standard information does not change
            uint8_t const headerData[] = { (uint8_t)255, (uint8_t)216, (uint8_t)255, (uint8_t)224, (uint8_t)0, (uint8_t)16, (uint8_t)74, (uint8_t)70, (uint8_t)73,  (uint8_t)70, (uint8_t)0};
            myJpg.write((const char*)headerData, sizeof(headerData));
            //jfif version density and thumbnail data, should remain the same
            uint8_t const versionDensityThumbData[] = { (uint8_t)1, (uint8_t)1, (uint8_t)0, (uint8_t)0, (uint8_t)1, (uint8_t)0, (uint8_t)1, (uint8_t)0, (uint8_t)0 };
            myJpg.write((const char*)versionDensityThumbData, sizeof(versionDensityThumbData));
            
            
            //quantization tables
            uint8_t const quantHeader[] = {(uint8_t)255, (uint8_t)219, (uint8_t)0, (uint8_t)43, (uint8_t)0};
            //lum table
            myJpg.write((const char*)quantHeader, sizeof(quantHeader));
            myJpg.write((const char*)lumTable, sizeof(lumTable));
            //chroma table
            myJpg.write((const char*)quantHeader, sizeof(quantHeader));
            myJpg.write((const char*)chromaTable, sizeof(chromaTable));


            //start of frame
            uint8_t const sofoHeader[] = { (uint8_t)255, (uint8_t)192, (uint8_t)0, (uint8_t)17, (uint8_t)8 };
            //refers to sampling amount and which quantization tables it uses
            uint8_t const sofoComponentSampling[] = { (uint8_t)3, (uint8_t)1, (uint8_t)34, (uint8_t)0, (uint8_t)2, (uint8_t)17, (uint8_t)1, (uint8_t)3, (uint8_t)1 };
            //pixel amount first bit of variable data
            int pixelHeight = map.retrieveDimensions()[0];
            int pixelLength = map.retrieveDimensions()[1];
            //pixel data is arranged in 2 bytes per dimension, first two are devoted to height second to length
            //thus a JPG can only be (256 * 256) * (256 * 256) pixels in dimensions
            uint8_t pixelDimensions[] = {(uint8_t)(pixelHeight/256), (uint8_t)(pixelHeight%256),(uint8_t)(pixelLength/256),(uint8_t)(pixelLength%256)};
            //sofo header on top
            myJpg.write((const char*)sofoHeader, sizeof(sofoHeader));
            //then pixel dimensions
            myJpg.write((const char*)pixelDimensions, sizeof(pixelDimensions));
            //then componentsampling info, we have taken the standard 4X2X0 dimensions which makes the chroma components 4x4 while the luma components are 8x8. This is standard in jpg, and it works because the human eye sucks
            myJpg.write((const char*)sofoComponentSampling, sizeof(sofoComponentSampling));
            
            std::cout << "PictureDimensions: " << pixelHeight << " || " << pixelLength << endl;

            myJpg.close();



        }

	}
//this is a forward dct that only uses integer multiplication and addition
//it is completely based on the two code snippets provided by
//Emil Mikulic
//Pratyush Kumar Ranjan
//both AAN implementation found in library
//and https://drdobbs.com/parallel/algorithm-alley/184410889
//multipliers
int m1 = 1448; /*.707 (sqrt(2)/2) this is what the multiplier is naturally, instead we use 1448 which is sqrt(2)/2 << 11 or sqrt(2)/2 * 2^11*/;
int m2;
int m3 = 1448;//^.707 same as above
int m4;
int m5;



void dct_II_uint_8t_int_8x8(int* target_mcu, uint8_t *source_mcu)
{
    //row DCT
    for (int i = 0; i < 8; i++)
    {

        //initialization
        int f0, f1, f2, f3, f4, f5, f6, f7;
        f0 = source_mcu[i * 8];
        f1 = source_mcu[1 + i * 8];
        f2 = source_mcu[2 + i * 8];
        f3 = source_mcu[3 + i * 8];
        f4 = source_mcu[4 + i * 8];
        f5 = source_mcu[5 + i * 8];
        f6 = source_mcu[6 + i * 8];
        f7 = source_mcu[7 + i * 8];
        
        //iterations before multipliers
        f0 -= f7;
        f1 -= f6;
        f2 -= f5;
        f3 -= f4;
        f4 += f3;
        f5 += f2;
        f6 += f1;
        f7 += f0;
        int x8 = f7;//cant do next step without storing varible

        //top four
        int x1;
        int a1, a2;
        int b1, b4, b2, b3;
        x1 = a1 + a2;
        a1 = b1 + b4;
        a2 = b2 + b3;
        b1 = f0 + f7;
        b4 = f3 + f4;
        b2 = f1 + f6;
        b3 = f2 + f5;

        int x2;
        x2 = a1 - a2;

        int x3;
        int a3, a4;
        x3 = a3 + a4;
        a3 = b2 - b3;
        a4 = b1 - b4;

        int x4;
        x4 = a4;

        //bottom four
        f3 = -f3 - f2;
        f2 += f1;
        f1 += f0;
        
        int x5;
        int a5;
        int b5, b6;
        x5 = a5;
        a5 = -b5 - b6;
        b5 = f3 - f4;
        b6 = f2 - f5;

        int x6;
        int a6;
        int b7;
        x6 = a6;
        a6 = b6 + b7;
        b7 = f1 - f6;

        int x7;
        int a7;
        int b8;
        x7 = a7;
        a7 = b7 + b8;
        b8 = f0 - f7;

        int x8;
        int a8;
        x8 = a8;
        a8 = b8;

        //post multiplication
        
        //top four
        int F0;
        int d1;
        int c1;
        F0 = d1 >> 3;
        d1 = c1;
        c1 = x1;

        int F4;
        int d2;
        int c2;
        F4 = d2 >> 4;
        d2 = c2;
        c2 = x2;

        int F2;
        int d3;
        int c3, c4;
        F2 = d3 >> 4;
        d3 = c3 + c4;
        c3 = x3 * m1;
        c4 = x4;

        int F6;
        int d4;
        F6 = d4 >> 4;
        d4 = c4 - c3;

        //bottom four

        int F5;
        int d8, d5;
        int c5, c8, c6;
        int x9;
        F5 = (d8 + d5) >> 4;
        d5 = c5;
        d8 = c8 - c6;
        c5 = -(x5 * m2) - (x9 * m5);
        c8 = x8;
        c6 = x6 * m3;
        x9 = x5 + x7;

        int F1;
        int d7, d6;
        int c7;
        F1 = (d7 + d6) >> 4;
        d6 = c6 + c8;
        d7 = c7;
        c7 = (x7 * m4) - (x9 * m5);

        int F7;
        F7 = (d5 - d7) >> 4;

        int F3;
        F3 = (d8 - d5) >> 4;

    }
}