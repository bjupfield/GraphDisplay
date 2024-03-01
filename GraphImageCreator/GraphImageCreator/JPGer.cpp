#include <iostream>
#include <fstream>
#include "JPGer.hpp"
#include "DumbFuncsNClasses.hpp"

using namespace std;

//all tables are provided by the courtesy of Nicolas Robidoux
//at 
//https://legacy.imagemagick.org/discourse-server/viewtopic.php?t=20333

const uint8_t ff = 255;

const int zigging[64] = {
    0,   1,  8, 16,  9,  2,  3, 10,
    17, 24, 32, 25, 18, 11,  4,  5,
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13,  6,  7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63
};

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
static const int lumTable_2[16] =
{
    16, 12, 12, 15,
    12, 12, 14, 18, 
    12, 14, 16, 24, 
    15, 18, 24, 30, 
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
static const int chromaTable_2[16] =
{
    17, 18, 24, 47,
    18, 21, 26, 66,
    24, 26, 56, 99,
    47, 66, 99, 128,
};
static const int* quantTables8x8[3] = { lumTable, chromaTable, chromaTable };
static const int* quantTables4x4[3] = { lumTable_2, chromaTable_2, chromaTable_2 };


int intSorter(int a, int b);
int intReverseSorter(int a, int b);
int* dctQuantizer(uint8_t* table, int dim, int previousDC, int num);
int* huffmanCodeCountArray(int *a, int arraySize);
void huffManReferenceTable(fakeDictionary<int, int>& huffmanCodeLength, fakeDictionary<int, uint8_t>& target, bool b = false);
void SOI_APPO_M(byteWritter& bw, int densityY, int densityX, int density);
void DQT_M(byteWritter& bw, int*& table, int tableNum, int num);
void SOF_M(byteWritter& bw, int chromaExist, int ySamplingSize, int cSamplingSize, int pixelHeight, int pixelWidth);
void DHT_M(byteWritter& bw, huffmanTable& huff);

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



int* dct_II_uint_8t_int_8x8(uint8_t *source_mcu, int previousCoefficient)
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

    mcu[0] = mcu[0] - previousCoefficient;
    return mcu;
}
const int m6 = 392;//cos(3pi/8) * 2^10
const int m7 = 946;//sin(3pi/8) * 2^10
const int m8 = 1448;//sqrt(2) * 2^10
int *dct_II_uint_8t_int_4x4(uint8_t* source_mcu, int previousCoefficient)
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
        f3 = (f3 * m6) - (f5 * m7);

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
        f3 = (f3 * m6) - (f5 * m7);

        //insert and normalization and bit shift
        //again notice how insert is not at regular intervals,
        //refer to document
        mcu[i] = f0 >> 1;
        mcu[i + 8] = f1 >> 1;
        mcu[i + 4] = f2 / m8;
        mcu[i + 12] = f3 / m8;

    }

    mcu[0] = mcu[0] - previousCoefficient;
    return mcu;
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
typedef struct {
    int row0;
    int row1;
    int row2;
    int row3;
    int row4;
    int row5;
    int row6;
    int row7;
} dct_stage;

void dct_aan(int*, int*);
void dct_aan(int* input, int* output) {

    int iter;
    int temp_buff;

    dct_stage stage1;
    dct_stage stage2;
    dct_stage stage3;
    dct_stage stage4;
    dct_stage stage5;

    //stage 1
    stage1.row0 = input[0] + input[6];
    stage1.row1 = input[1] + input[5];
    stage1.row2 = input[2] + input[4];
    stage1.row3 = input[3];
    stage1.row4 = input[2] - input[4];
    stage1.row5 = input[1] - input[5];
    stage1.row6 = input[0] - input[6];
    stage1.row7 = input[7];


    //stage 2
    stage2.row0 = stage1.row0 + stage1.row3;
    stage2.row1 = stage1.row1 + stage1.row2;
    stage2.row2 = stage1.row1 - stage1.row2;
    stage2.row3 = stage1.row0 - stage1.row3;
    stage2.row4 = stage1.row4 + stage1.row5;
    stage2.row5 = stage1.row5 + stage1.row6;
    stage2.row6 = stage1.row6 + stage1.row7;
    stage2.row7 = stage1.row7;

    //stage 3
    stage3.row0 = stage2.row0 + stage2.row1;
    stage3.row1 = stage2.row0 - stage2.row1;
    stage3.row2 = stage2.row2 + stage2.row3;
    stage3.row3 = stage2.row3;
    stage3.row4 = stage2.row4;
    stage3.row5 = stage2.row5;
    stage3.row6 = stage2.row6;
    stage3.row7 = stage2.row7;

    //stage 4 
    // a1 = 0.707 0.10110101
    // a2 = 0.541 0.10001010
    // a3 = 0.707 0.10110101
    // a4 = 1.307 1.01001111
    // a5 = 0.383 0.01100010
    stage4.row0 = stage3.row0;
    stage4.row1 = stage3.row1;
    stage4.row2 = (stage3.row2 >> 1) + (stage3.row2 >> 3) + (stage3.row2 >> 4) + ((stage3.row2 + 32) >> 6) + ((stage3.row2 + 128) >> 8);
    stage4.row3 = stage3.row3;

    temp_buff = stage3.row4 + stage3.row6;
    temp_buff = (temp_buff >> 2) + (temp_buff >> 3) + ((temp_buff + 64) >> 7);
    stage4.row4 = (stage3.row4 >> 1) + ((stage3.row4 + 16) >> 5) + ((stage3.row4 + 64) >> 7) - temp_buff;
    stage4.row5 = (stage3.row5 >> 1) + (stage3.row5 >> 3) + (stage3.row5 >> 4) + ((stage3.row5 + 32) >> 6) + ((stage3.row5 + 128) >> 8);
    stage4.row6 = stage3.row6 + (stage3.row6 >> 2) + (stage3.row6 >> 5) + (stage3.row6 >> 6) + (stage3.row6 >> 7) + (stage3.row6 >> 8) - temp_buff;
    stage4.row7 = stage3.row7;

    //stage 5
    stage5.row0 = stage4.row0;
    stage5.row1 = stage4.row1;
    stage5.row2 = stage4.row3 + stage4.row2;
    stage5.row3 = stage4.row3 - stage4.row2;
    stage5.row4 = stage4.row4;
    stage5.row5 = stage4.row5 + stage4.row7;
    stage5.row6 = stage4.row6;
    stage5.row7 = stage4.row7 - stage4.row5;

    //stage 6 Output
    output[0] = stage5.row0;
    output[4] = stage5.row1;
    output[2] = stage5.row2;
    output[6] = stage5.row3;
    output[5] = stage5.row4 + stage5.row7;
    output[1] = stage5.row5 + stage5.row6;
    output[7] = stage5.row5 - stage5.row6;
    output[3] = stage5.row4 + stage5.row7;

}
int* test_8x8_int_DCT(uint8_t *source_mcu, int previousCoefficient)
{
    int* mcu = new int[64];

    for(int i = 0; i < 8; i++)
    {
        int* row = new int[8];
        row[0] = source_mcu[i * 8];
        row[1] = source_mcu[i * 8 + 1];
        row[2] = source_mcu[i * 8 + 2];
        row[3] = source_mcu[i * 8 + 3];
        row[4] = source_mcu[i * 8 + 4];
        row[5] = source_mcu[i * 8 + 5];
        row[6] = source_mcu[i * 8 + 6];
        row[7] = source_mcu[i * 8 + 7];

        int* rowStore = new int[8];
        dct_aan(row, rowStore);

        mcu[i * 8] = rowStore[0];
        mcu[i * 8 + 1] = rowStore[1];
        mcu[i * 8 + 2] = rowStore[2];
        mcu[i * 8 + 3] = rowStore[3];
        mcu[i * 8 + 4] = rowStore[4];
        mcu[i * 8 + 5] = rowStore[5];
        mcu[i * 8 + 6] = rowStore[6];
        mcu[i * 8 + 7] = rowStore[7];
    }
    for (int i = 0; i < 8; i++)
    {
        int* column = new int[8];
        column[0] = source_mcu[i];
        column[1] = source_mcu[i + 8];
        column[2] = source_mcu[i + 16];
        column[3] = source_mcu[i + 24];
        column[4] = source_mcu[i + 32];
        column[5] = source_mcu[i + 40];
        column[6] = source_mcu[i + 48];
        column[7] = source_mcu[i + 56];

        int* rowStore = new int[8];
        dct_aan(column, rowStore);

        mcu[i] = rowStore[0];
        mcu[i + 8] = rowStore[1];
        mcu[i + 16] = rowStore[2];
        mcu[i + 24] = rowStore[3];
        mcu[i + 32] = rowStore[4];
        mcu[i + 40] = rowStore[5];
        mcu[i + 48] = rowStore[6];
        mcu[i + 56] = rowStore[7];

    }

    mcu[0] = mcu[0] - previousCoefficient;
    return mcu;
}
int* test2_dct(uint8_t* source_mcu, int previousCoefficient)
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

        //second stage
        int s0, s1, s2, s3, s4, s5, s6, s7;

        s0 = f0 + f7;
        s1 = f1 + f6;
        s2 = f2 + f5;
        s3 = f3 + f4;
        s4 = f3 - f4;
        s5 = f2 - f5;
        s6 = f1 - f6;
        s7 = f0 - f7;
        
        //third stage
        int t0, t1, t2, t3, t4, t5, t6, t7;

        t0 = s0 + s3;
        t1 = s1 + s2;
        t2 = s1 - s2;
        t3 = s0 - s3;

        t4 = -s4 - s5;
        t5 = s5 + s6;
        t6 = s6 + s7;
        t7 = s7;

        //fourth stage
        int q0, q1, q2, q3, q4, q5, q6, q7;

        q0 = t0 + t1;
        q1 = t0 - t1;
        q2 = t2 + t3;
        q3 = t3;

        q4 = t4;
        q5 = t5;
        q6 = t6;
        q7 = t7;

        //mulitplication stage, fifth stage
        float special;
        float r0, r1, r2, r3, r4, r5, r6, r7;

        special = (float)(q4 + q6) * 0.383f;

        r0 = (float)q0;
        r1 = (float)q1;
        r2 = (float)q2 * 0.707f;
        r3 = (float)q3;

        r4 = -(float)q4 * 0.541f - special;
        r5 = (float)q5 * 0.707f;
        r6 = (float)q6 * 1.307f - special;
        r7 = (float)q7;

        //sixth stage;
        float x0, x1, x2, x3, x4, x5, x6, x7;

        x0 = r0;
        x1 = r1;
        x2 = r2 + r3;
        x3 = r3 - r2;

        x4 = r4;
        x5 = r5 + r7;
        x6 = r6;
        x7 = r7 - r5;

        //seventh final stage
        float v0, v1, v2, v3, v4, v5, v6, v7;

        v0 = x0;
        v1 = x1;
        v2 = x2;
        v3 = x3;
        
        v4 = x4 + x7;
        v5 = x5 + x6;
        v6 = x5 - x6;
        v7 = x7 - x4;

        //insert and bit shift
        mcu[i * 8] = (int)v0 >> 3;
        mcu[i * 8 + 4] = (int)v1 >> 4;
        mcu[i * 8 + 2] = (int)v2 >> 4;
        mcu[i * 8 + 6] = (int)v3 >> 4;
        mcu[i * 8 + 5] = (int)v4 >> 4;
        mcu[i * 8 + 1] = (int)v5 >> 4;
        mcu[i * 8 + 7] = (int)v6 >> 4;
        mcu[i * 8 + 3] = (int)v7 >> 4;
    }
    //column DCT
    for (int i = 0; i < 8; i++)
    {

        //initialization
        int f0, f1, f2, f3, f4, f5, f6, f7;
        f0 = source_mcu[i];
        f1 = source_mcu[i + 8];
        f2 = source_mcu[i + 16];
        f3 = source_mcu[i + 24];
        f4 = source_mcu[i + 32];
        f5 = source_mcu[i + 40];
        f6 = source_mcu[6 + 48];
        f7 = source_mcu[7 + 56];

        //second stage
        int s0, s1, s2, s3, s4, s5, s6, s7;

        s0 = f0 + f7;
        s1 = f1 + f6;
        s2 = f2 + f5;
        s3 = f3 + f4;
        s4 = f3 - f4;
        s5 = f2 - f5;
        s6 = f1 - f6;
        s7 = f0 - f7;

        //third stage
        int t0, t1, t2, t3, t4, t5, t6, t7;

        t0 = s0 + s3;
        t1 = s1 + s2;
        t2 = s1 - s2;
        t3 = s0 - s3;

        t4 = -s4 - s5;
        t5 = s5 + s6;
        t6 = s6 + s7;
        t7 = s7;

        //fourth stage
        int q0, q1, q2, q3, q4, q5, q6, q7;

        q0 = t0 + t1;
        q1 = t0 - t1;
        q2 = t2 + t3;
        q3 = t3;

        q4 = t4;
        q5 = t5;
        q6 = t6;
        q7 = t7;

        //mulitplication stage, fifth stage
        float special;
        float r0, r1, r2, r3, r4, r5, r6, r7;

        special = (float)(q4 + q6) * 0.383f;

        r0 = (float)q0;
        r1 = (float)q1;
        r2 = (float)q2 * 0.707f;
        r3 = (float)q3;

        r4 = -(float)q4 * 0.541f - special;
        r5 = (float)q5 * 0.707f;
        r6 = (float)q6 * 1.307f - special;
        r7 = (float)q7;

        //sixth stage;
        float x0, x1, x2, x3, x4, x5, x6, x7;

        x0 = r0;
        x1 = r1;
        x2 = r2 + r3;
        x3 = r3 - r2;

        x4 = r4;
        x5 = r5 + r7;
        x6 = r6;
        x7 = r7 - r5;

        //seventh final stage
        float v0, v1, v2, v3, v4, v5, v6, v7;

        v0 = x0;
        v1 = x1;
        v2 = x2;
        v3 = x3;

        v4 = x4 + x7;
        v5 = x5 + x6;
        v6 = x5 - x6;
        v7 = x7 - x4;

        //insert and bit shift
        mcu[i] = (int)v0 >> 3;
        mcu[i + 32] = (int)v1 >> 4;
        mcu[i + 16] = (int)v2 >> 4;
        mcu[i + 48] = (int)v3 >> 4;
        mcu[i + 40] = (int)v4 >> 4;
        mcu[i + 8] = (int)v5 >> 4;
        mcu[i + 56] = (int)v6 >> 4;
        mcu[i + 24] = (int)v7 >> 4;
    }
    
    mcu[0] = mcu[0] - previousCoefficient;
    return mcu;
}
int* dct_II_uint_8t_int_4x4_TEST(uint8_t* source_mcu, int previousCoefficient)
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

        //first stage

        int x0, x1, x2, x3;

        x0 = f0 + f3;
        x1 = f1 + f2;
        x2 = f1 - f2;;
        x3 = f0 - f3;;

        //second stage

        int s0, s1, s2, s3;

        s0 = x0 + x1;
        s1 = x0 - x1;
        s2 = (x2 * m6) + (x3 * m7);
        s3 = (x3 * m6) + (x2 * m7);

        //bit shift and divide

        mcu[i * 4] = s0 >> 1;
        mcu[i * 4 + 2] = s1 >> 1;
        mcu[i * 4 + 1] = s2 / m8;
        mcu[i * 4 + 3] = s3 / m8;

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

        //first stage

        int x0, x1, x2, x3;

        x0 = f0 + f3;
        x1 = f1 + f2;
        x2 = f1 - f2;;
        x3 = f0 - f3;;

        //second stage

        int s0, s1, s2, s3;

        s0 = x0 + x1;
        s1 = x0 - x1;
        s2 = (x2 * m6) + (x3 * m7);
        s3 = (x3 * m6) + (x2 * m7);

        //bit shift and divide

        mcu[i] = f0 >> 1;
        mcu[i + 8] = f1 >> 1;
        mcu[i + 4] = f2 / m8;
        mcu[i + 12] = f3 / m8;

    }

    mcu[0] = mcu[0] - previousCoefficient;
    return mcu;
}
void actualJpg(hInfoStruct hInfo, mcuHuffmanContainer mcuHuffman, char*& fileName)
{
    byteWritter bW = byteWritter(fileName);

    if (bW.open())
    {
        //write jpg...
        SOI_APPO_M(bW, hInfo.densityY, hInfo.densityX, hInfo.density);
        DQT_M(bW, hInfo.lumaTable, hInfo.lumaTableType, 0);//luma table will be used if monochrome
        if (hInfo.chromaTableType != 0) DQT_M(bW, hInfo.chromaTable, hInfo.chromaTableType, 1);//check if chromatable exist (will not if monochrome image)



    }
}
mcuHuffmanContainer::mcuHuffmanContainer(MCUS origin)
{
    this->dim = origin.retrieveDim();
    this->mcuHeight = origin.retrieveHeight();
    this->mcuLength = origin.retrieveLength();

    this->mcus = new intMcus[mcuHeight * mcuLength];

    this->yCHuffman = new huffmanTable[2];
    /*this->yCHuffman[0] = huffmanTable();
    this->yCHuffman[1] = huffmanTable();*/


    for (int i = 0; i < mcuHeight; i++) 
    {
        for (int j = 0; j < mcuLength; j++)
        {
            int pos = i * mcuLength + j;
            mcus[pos] = intMcus();
            for(int i = 0; i < 3; i++)//if we ever want to make it monochrome...
            {
                mcus[pos].yCbCr(i) = dctQuantizer(&origin.mcuList[pos].ycbcr[i], this->dim[i], ((pos == 0) ? 0 : mcus[pos - 1].yCbCr[i][0]), i);
                yCHuffman[i < 1 ? i : 1].frequency(this->dim[i], this->mcus[pos].yCbCr[i]);
            }
            //mcus[pos - 1].Y[0]
            //im throwing the previous mcus DC coefficient, as this is what jpg does. Jpg standard says that DC coefficient are dependent on previous DC cocefficients, where the DC Coefficient = current coefficient - previous coefficient
            //https://en.wikipedia.org/wiki/JPEG under entropy coding section
        }
    }
    
    for (int n = 0; n < 20; n++)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                std::cout << this->mcus[n].yCbCr[1][j + i * 4] << ", ";
            }
            std::cout << std::endl;
        }
    }
    yCHuffman[0].huffmanCodes();
    yCHuffman[1].huffmanCodes();
}
void testIntMcus(mcuHuffmanContainer mine, int num)
{
    for(int i = 0; i < mine.dim.Y; i++)
    {
        for(int j = 0; j < mine.dim.Y; j++)
        {
            std::cout << mine.mcus[num].yCbCr[0][j + i * mine.dim.Y] << " ";
        }
        std::cout << std::endl;
    }
}
int* dctQuantizer(uint8_t* table,int dim, int previousDC, int num) 
{
    int* returnTable = (int*)malloc(dim * dim * sizeof(int));
    if (dim == 8)
    {
        //this->Y = dct_II_uint_8t_int_8x8(uintMcu.Y);
        //testing
        //this->Y = test_8x8_int_DCT(uintMcu.Y, previousCoefficient);
        returnTable = test2_dct(table, previousDC);
        quantizer_8x8_int(returnTable, quantTables8x8[num]);
    }
    else if (dim == 4)
    {
        //returnTable = dct_II_uint_8t_int_4x4(table, previousDC);
        returnTable = dct_II_uint_8t_int_4x4_TEST(table, previousDC);
        quantizer_4x4_int(returnTable, quantTables4x4[num]);
    }
    return returnTable;
}
void huffmanTable::frequency(int dim, int* table)
{
    //dc
    if (table[0] != 0)
        if (this->DCcodeLength.addPair(table[0], 1) == -1) DCcodeLength.changeTerm(table[0], DCcodeLength.retrieveTerm(table[0]) + 1);

    //ac

    for (int i = 1; i < dim * dim; i++)
    {
        if (table[i] != 0)
            if (ACcodeLength.addPair(table[i], 1) == -1)
            {
                ACcodeLength.changeTerm(table[i], ACcodeLength.retrieveTerm(table[i]) + 1);
            }
    }
}
void huffmanTable::huffmanCodes()
{
    this->DCcodeLength.sortByTerm(intSorter);
    this->ACcodeLength.sortByTerm(intSorter);

    int* huffmanDC = huffmanCodeCountArray(this->DCcodeLength.retrieveAllTerms(), this->DCcodeLength.returnCount());
    int* huffmanAC = huffmanCodeCountArray(this->ACcodeLength.retrieveAllTerms(), this->ACcodeLength.returnCount());

    int* keysDC = this->DCcodeLength.retrieveAllKeys();
    int* keysAC = this->ACcodeLength.retrieveAllKeys();
    for (int i = 0; i < this->DCcodeLength.returnCount(); i++)
    {
        this->DCcodeLength.changeTerm(keysDC[i], huffmanDC[i]);
    }
    
    for (int i = 0; i < this->ACcodeLength.returnCount(); i++)
    {
        this->ACcodeLength.changeTerm(keysAC[i], huffmanAC[i]);
    }

    this->DCcodeLength.sortByTerm(intReverseSorter);
    this->ACcodeLength.sortByTerm(intReverseSorter);

    huffManReferenceTable(this->DCcodeLength, this->DCcode);
    huffManReferenceTable(this->ACcodeLength, this->ACcode, true);

}
int* huffmanCodeCountArray(int* a, int arraySize) //okay this function accepts a frequenc array and returns a huffman tree arrray. The array it returns is equal in size to the array it enters and has the huffman positions of the frequencies stored where the frequencies were.
//the huffman positions means the huffman code length, the position in the tree that would increase the binary code by 1 digit, such as 01 to 010. What I store is the number of digits it takes, the code length, which in the case of 01 is 2 and 010 is 3.
{
    if(arraySize > 1)
    {
        //find two mininum values
        int pos1 = 0, pos1Min = a[0];

        for (int i = 1; i < arraySize; i++)
        {
            if (pos1Min > a[i])
            {
                pos1Min = a[i];
                pos1 = i;
            }
        }

        int pos2 = pos1 == 0 ? 1 : 0, pos2Min = pos1 == 0 ? a[1] : a[0];

        for (int i = 1; i < arraySize; i++)
        {
            if(pos2Min > a[i] && i != pos1)
            {
                pos2Min = a[i];
                pos2 = i;
            }
        }
        
        int* b = new int[arraySize - 1] {0};
        newCopyArray(a, b, 0, 0, pos2);//copy all values before pos2 into array
        newCopyArray(a, b, pos2 + 1, pos2, arraySize - 1 - pos2);//copy all values after pos2 into array
        b[(pos1 > pos2) ? pos1 - 1 : pos1] = pos2Min + pos1Min;//combine the two minimum values into one frequency in the new array

        int* c = huffmanCodeCountArray(b, arraySize - 1);//recursively call the function with the new frequencies

        int* d = new int[arraySize] {0};
        newCopyArray(c, d, 0, 0, pos2);//reverse the shrink
        newCopyArray(c, d, pos2, pos2 + 1, arraySize - 1 - pos2);


        d[pos1] += 1;//add 1 code length to the minimum value that was shrunk
        d[pos2] = d[pos1];//readd the minimum value back into the array

        delete[] c;
        delete[] b;

        return d;//return the array
    }
    else
    {
        return new int[1] {1};//all codes have a minimum code length of 1
    }
}
void huffManReferenceTable(fakeDictionary<int, int>& huffmanCodeLength, fakeDictionary<int, uint8_t> &target,bool b)//this creates a dictionary for referencing the code values later
{
    int count = huffmanCodeLength.returnCount();

    if (count > 170 || count < 1)//if above 170 or below 1 value cannot store as huffman table
    {
        std::cout << "Failure" << std::endl;
    }

    int* funnyPtr = huffmanCodeLength.retrieveAllKeys();
    int* funnyPtr2 = huffmanCodeLength.retrieveAllTerms();

    //code starts at zero, add 1 for every iteration
    //when code lenght changes bit shift to the left by newcodelength - curcodelength 
    uint8_t code = 0;
    uint8_t codeLength = funnyPtr2[0];
    for (int i = 0; i < count; i++)
    {
        if (i != 0 && codeLength != funnyPtr2[i])
        {
            code = code << funnyPtr2[i] - codeLength;
            codeLength = funnyPtr2[i];
        }
        target.addPair(funnyPtr[i], code);
        ++code;
    }
}
int intSorter(int a, int b)
{
    if (a < b) return 0;
    return 1;
}
int intReverseSorter(int a, int b)
{
    if (a >= b) return 0;
    return 1;
}
void SOI_APPO_M(byteWritter& bw, int densityY, int densityX, int density)
{
    bw.write(ff); bw.write(216);//SOI Marker, START OF IMAGE MARKER
    bw.write(ff); bw.write(224);//APPO Marker, application date marker
    bw.write(0); bw.write(16);//APPO segment size, because we are not using the thumbnail this will always be the same size
    bw.write(74); bw.write(70); bw.write(73); bw.write(70); bw.write(0);//JFIF indentifier string, this constant identifies the jpeg as a jfif type, which is standard
    bw.write((uint8_t)density);//these next three are the resoultuion density of the image. say that the resolution is 3 by 2, so every inch there is three pixels y and 2 pixels x
    bw.write((uint8_t)densityY, 16);//this means we would set density to 3
    bw.write((uint8_t)densityX, 16);//and y density to 2 x density to 2...... however it is almost always zero density and 1, 1 for y density and x density, because no one uses different density images
    bw.write(0);//thumbnailsize
    bw.write(0);//thumbnailsizey
}
void DQT_M(byteWritter& bw, int*& table, int tableNum, int num)
{
    bw.write(ff); bw.write(219);//DQT header, Data Quantization Table Header
    bw.write(tableNum + 3, 16);//Header length, table length + 3 bits written here not including marker
    bw.write(0, 4); bw.write(num, 4);// this byte here contains two bits (lol not actually bits) of information, the table integer size either 8 bit or 16 bit and the table id number. We will only be using 8 bit which is 0000 (16 is 0001)m abd the second bit contains the id, possible nums 0-3
    for (int i = 0; i < tableNum; i++)
    {
        bw.write(table[zigging[i]]);
    }
}
void SOF_M(byteWritter& bw, int chromaExist, int ySamplingSize, int cSamplingSize, int pixelHeight, int pixelWidth) 
{
    bw.write(ff); bw.write(192);//S0F header, start of frame header type 0
    bw.write(chromaExist != 0 ? 17 : 11);//length of segment, if monochrome only 11 bytes if not 17 
    bw.write(8);//jpg pixel precision, jpg only supports 8bit precision but it must be included anyway lol
    bw.write(pixelHeight, 16);//jpg pixel height, stored in two bits maxinum size of 256 x 256
    bw.write(pixelWidth, 16);//jpg pixel length
    bw.write(1); bw.write(ySamplingSize == 2 ? 34 : 17); bw.write(0);//y component or monochrome component data, sampling size will either by 1 or 2, should almost always be two for the y component
    if (chromaExist != 0)
    {
        bw.write(2); bw.write(ySamplingSize == 2 ? 34 : 17); bw.write(1);//cb component, sampling size should almost always be one and therefor 0x11, also quant tables are at the end, chroma uses quant table one luma uses quant table 0
        bw.write(3); bw.write(ySamplingSize == 2 ? 34 : 17); bw.write(1);
    }

}