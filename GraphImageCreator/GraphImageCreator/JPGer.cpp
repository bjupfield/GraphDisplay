#include <iostream>
#include <fstream>
#include "JPGer.hpp"

using namespace std;

//all tables are provided by the courtesy of Nicolas Robidoux
//at 
//https://legacy.imagemagick.org/discourse-server/viewtopic.php?t=20333

static const int lumTable[64] =
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

static const int chromaTable[64] =
{
    17, 18, 24, 47, 99, 128, 192, 256,
    18, 21, 26, 66, 99, 128, 192, 256,
    24, 26, 56, 99, 128, 192, 256, 512,
    47, 66, 99, 128, 192, 256, 512, 1024,
    99, 99, 128, 192, 256, 512, 1024, 2048,
    128, 128, 192, 256, 512, 1024, 3072, 4096,
    192, 192, 256, 512, 1024, 3072, 6144, 7168,
    256, 256, 512, 1024, 2048, 4096, 7168, 8192
};//strange unreadable values

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
const int m1 = 724; /*.707 (sqrt(2)/2) this is what the multiplier is naturally, instead we use 1448 which is sqrt(2)/2 << 10 or sqrt(2)/2 * 2^10*/;
const int m2 = 554; /*honeslty dont know what this one is equal to but its starts at .541 we expand it to 554 by >> 10 or .541 * 2^10*/
//m3 = m1
const int m4 = 669; /*same multiply by 2^9 to get 669 from 1.307*/
const int m5 = 392;/*.383 sin(pi/8), mulltiplied by 2^10 to get 392*/



int* dct_II_uint_8t_int_8x8(uint8_t *source_mcu)
{
    int* mcu = new int[64];
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
        

        //first stage
        f0 += f7;
        f1 += f6;
        f2 += f5;
        f3 += f4;
        
        f7 = f0 - (f7 << 1);
        f6 = f1 - (f6 << 1);
        f5 = f2 - (f5 << 1);
        f4 = -f3 + (f4 << 1);

        //second stage
        f3 = f0 - f3;
        f2 = f1 - f2;
        f1 = -f3 + (f0 << 1) + f2 - (f1 << 1);
        f2 += f3;
        f0 = -f1 - (f3 << 1) - (f0 << 2);

        f4 -= f5;
        f5 -= f6;
        f6 -= f7;

        //third stage - multiplication

        f2 = ((m1 * f2) >> 10) + f3;
        f3 = (f3 << 1) - f2;

        int special = ((f4 + f6) * m5) >> 10;

        f5 = f7 + ((f5 * m1) >> 10);
        f6 = ((f6 * m4) >> 9) - special;
        f4 = ((f4 * m2) >> 10) + special;
        f7 = -f5 + (f7 << 1);

        f5 += f6;
        f6 = f5 - (f6 << 1);
        f7 += f4;
        f4 = f7 - (f4 << 1);

        //fourth stage insert into array and bit shift multipliers and ending multipliers from aan algo away
        //notice the wonky index numbers, this is matching the chart provided at http://board.flatassembler.net/topic.php?p=204231

        mcu[i * 8] = f0 >> 3;
        mcu[i * 8 + 4] = f1 >> 4;
        mcu[i * 8 + 2] = f2 >> 4;
        mcu[i * 8 + 6] = f3 >> 4;
        mcu[i * 8 + 5] = f4 >> 4;
        mcu[i * 8 + 1] = f5 >> 4;
        mcu[i * 8 + 7] = f6 >> 4;
        mcu[i * 8 + 3] = f7 >> 4;
    }

    //column DCT only change is in source and output
    for (int i = 0; i < 8; i++)
    {

        //initialization
        int f0, f1, f2, f3, f4, f5, f6, f7;
        f0 = source_mcu[i];
        f1 = source_mcu[8 + i];
        f2 = source_mcu[16 + i];
        f3 = source_mcu[24 + i];
        f4 = source_mcu[32 + i];
        f5 = source_mcu[40 + i];
        f6 = source_mcu[48 + i];
        f7 = source_mcu[56 + i];


        //first stage
        f0 += f7;
        f1 += f6;
        f2 += f5;
        f3 += f4;

        f7 = f0 - (f7 << 1);
        f6 = f1 - (f6 << 1);
        f5 = f2 - (f5 << 1);
        f4 = -f3 + (f4 << 1);

        //second stage
        f3 = f0 - f3;
        f2 = f1 - f2;
        f1 = -f3 + (f0 << 1) + f2 - (f1 << 1);
        f2 += f3;
        f0 = -f1 - (f3 << 1) - (f0 << 2);

        f4 -= f5;
        f5 -= f6;
        f6 -= f7;

        //third stage - multiplication

        f2 = ((m1 * f2) >> 10) + f3;
        f3 = (f3 << 1) - f2;

        int special = ((f4 + f6) * m5) >> 10;

        f5 = f7 + ((f5 * m1) >> 10);
        f6 = ((f6 * m4) >> 9) - special;
        f4 = ((f4 * m2) >> 10) + special;
        f7 = -f5 + (f7 << 1);

        f5 += f6;
        f6 = f5 - (f6 << 1);
        f7 += f4;
        f4 = f7 - (f4 << 1);

        //fourth stage insert into array and bit shift multipliers

        mcu[i] = f0 >> 3;
        mcu[i + 32] = f1 >> 4;
        mcu[i + 16] = f2 >> 4;
        mcu[i + 48] = f3 >> 4;
        mcu[i + 40] = f4 >> 4;
        mcu[i + 8] = f5 >> 4;
        mcu[i + 56] = f6 >> 4;
        mcu[i + 24] = f7 >> 4;
    }

    return mcu;
}
const int m6 = 392;//cos(3pi/8) * 2^10
const int m7 = 946;//sin(3pi/8) * 2^10
const int m8 = 1448;//sqrt(2) * 2^10
int *dct_II_uint_8t_int_4x4(uint8_t* source_mcu)
{
    int* mcu = new int[16];

    //row DCT
    for (int i = 0; i < 4; i++)
    {
        int f0, f1, f2, f3;

        //insert stage

        f0 = source_mcu[i * 4];
        f1 = source_mcu[i * 4 + 1];
        f2 = source_mcu[i * 4 + 2];
        f3 = source_mcu[i * 4 + 3];

        //addition pre multipliers

        f0 += f3;
        f1 += f2;
        f3 = f0 - (f3 << 1);
        f2 = f1 - (f2 << 1);
        
        //top row addition
        
        f0 += f1;
        f1 = f0 - (f1 << 1);

        //bottom row multipliers
        int f5 = f2;
        f2 = (f2 * m6) + (f3 * m7);
        f3 = (f3 * m6) - (f2 * m7);

        //insert and normalization and bit shift
        //again notice how insert is not at regular intervals,
        //refer to document
        mcu[i * 4] = f0 >> 1;
        mcu[i * 4 + 2] = f1 >> 1;
        mcu[i * 4 + 1] = f2 / m8;
        mcu[i * 4 + 3] = f3 / m8;

    }

    //column DCT
    for (int i = 0; i < 4; i++)
    {
        int f0, f1, f2, f3;

        //insert stage

        f0 = source_mcu[i];
        f1 = source_mcu[i + 4];
        f2 = source_mcu[i + 8];
        f3 = source_mcu[i + 12];

        //addition pre multipliers

        f0 += f3;
        f1 += f2;
        f3 = f0 - (f3 << 1);
        f2 = f1 - (f2 << 1);

        //top row addition

        f0 += f1;
        f1 = f0 - (f1 << 1);

        //bottom row multipliers
        int f5 = f2;
        f2 = (f2 * m6) + (f3 * m7);
        f3 = (f3 * m6) - (f2 * m7);

        //insert and normalization and bit shift
        //again notice how insert is not at regular intervals,
        //refer to document
        mcu[i] = f0 >> 1;
        mcu[i + 8] = f1 >> 1;
        mcu[i + 4] = f2 / m8;
        mcu[i + 12] = f3 / m8;

    }

}
//this is applied after the DCT
void quantizer_8x8_int(int *mcu, const int *table)
{
    for (int i = 0; i < 64; i++)
    {
        mcu[i] = mcu[i] / table[i];
    }
}
void quantizer_4x4_int(int* mcu, const int* table)
{
    for (int i = 0; i < 16; i++)
    {
        mcu[i] = mcu[i] / table[i];
    }
}
intMcus::intMcus(MCU uintMcu, dimensions dim)
{
    //y loop
    if (dim.Y == 8)
    {
        this->Y = dct_II_uint_8t_int_8x8(uintMcu.Y);
        quantizer_8x8_int(Y, lumTable);
    }
    else if (dim.Y == 4)
    {
        this->Y = dct_II_uint_8t_int_8x8(uintMcu.Y);
        quantizer_4x4_int(Y, lumTable);
    }
    //cb loop
    if(dim.Cb == 8)
    {
        this->Cb = dct_II_uint_8t_int_8x8(uintMcu.Cb);
        quantizer_8x8_int(Cb, chromaTable);
    }
    else if (dim.Cb == 4)
    {
        this->Cb = dct_II_uint_8t_int_4x4(uintMcu.Cb);
        quantizer_4x4_int(Cb, chromaTable);
    }
    //cr loop
    if(dim.Cr == 8)
    {
        this->Cr == dct_II_uint_8t_int_8x8(uintMcu.Cr);
        quantizer_8x8_int(Cr, chromaTable);
    }
    else if(dim.Cr == 4)
    {
        this->Cr == dct_II_uint_8t_int_4x4(uintMcu.Cr);
        quantizer_4x4_int(Cr, chromaTable);
    }
}
mcuHuffmanContainer::mcuHuffmanContainer(MCUS origin)
{
    this->dim = origin.retrieveDim();
    this->mcuHeight = origin.retrieveHeight();
    this->mcuLength = origin.retrieveLength();

    this->mcus = new intMcus[mcuHeight * mcuLength];

    for (int i = 0; i < mcuLength; i++) 
    {
        for (int j = 0; j < mcuHeight; j++)
        {
            int pos = i + j * mcuLength;
            mcus[pos] = intMcus(origin.mcuList[pos], origin.retrieveDim());
        }
    }
}