#include <iostream>
#include <fstream>
#include "JPGer.hpp"
#include "DumbFuncsNClasses.hpp"

using namespace std;

//all tables are provided by the courtesy of Nicolas Robidoux
//at 
//https://legacy.imagemagick.org/discourse-server/viewtopic.php?t=20333

const uint8_t ff = 255;
const uint8_t f0 = 240;
const uint8_t x00 = 0;

bool testVar = false;

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
const int zigging2[16] = {
    0,  1,  4,  8,
    5,  2,  3,  6,
    9, 12, 13, 10,
    7, 11, 14, 15
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
int customSorter(uint8_t a, uint8_t b);
int bitLength(int bits);
int bitsAllOne(uint16_t bits, int length);
int* dctQuantizer(int* table, int dim, int previousDC, int num);
int* huffmanCodeCountArray(int *a, int arraySize);
void huffManReferenceTable(fakeDictionary<int, int>& huffmanCodeLength, fakeDictionary<int, uint16_t>& target, bool b = false);
void SOI_APPO_M(byteWritter& bw, int densityY, int densityX, int density);
void DQT_M(byteWritter& bw, const int* table, int tableNum, int num);
void SOF_M(byteWritter& bw, int chromaExist, int ySamplingSize, int cSamplingSize, int pixelHeight, int pixelWidth);
void DHT_M(byteWritter& bw, int ACDC, int tableNum, fakeDictionary<int, int>& codeFreq);
void SOS_M(byteWritter& bw, int components, int firstComponentLength);
void MCU_W(byteWritter& bw, fakeDictionary<int, uint16_t>& huffmanDcValueCodes, fakeDictionary<int, int>& huffmanDcValueLength, fakeDictionary<int, uint16_t>& huffmanAcValueCodes, fakeDictionary<int, int>& huffmanAcValueLength, int dim, int* table);
void EOI_M(byteWritter& bw);

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



int* dct_II_int_8t_int_8x8(int *source_mcu)
{
    int* mcu = new int[65];
    for (int i = 0; i < 64; i++) mcu[i] = (int)source_mcu[i];
    //row DCT
    for (int i = 0; i < 8; i++)
    {

        //initialization
        int f0, f1, f2, f3, f4, f5, f6, f7;
        f0 = mcu[i * 8];
        f1 = mcu[1 + i * 8];
        f2 = mcu[2 + i * 8];
        f3 = mcu[3 + i * 8];
        f4 = mcu[4 + i * 8];
        f5 = mcu[5 + i * 8];
        f6 = mcu[6 + i * 8];
        f7 = mcu[7 + i * 8];
        

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
        f0 = mcu[i];
        f1 = mcu[8 + i];
        f2 = mcu[16 + i];
        f3 = mcu[24 + i];
        f4 = mcu[32 + i];
        f5 = mcu[40 + i];
        f6 = mcu[48 + i];
        f7 = mcu[56 + i];


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
int *dct_II_int_8t_int_4x4(int* source_mcu)
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
int* test_8x8_int_DCT(int *source_mcu)
{
    int* mcu = new int[65];
    for (int i = 0; i < 64; i++) mcu[i] = (int)source_mcu[i];

    for(int i = 0; i < 8; i++)
    {
        int* row = new int[8];
        row[0] = mcu[i * 8];
        row[1] = mcu[i * 8 + 1];
        row[2] = mcu[i * 8 + 2];
        row[3] = mcu[i * 8 + 3];
        row[4] = mcu[i * 8 + 4];
        row[5] = mcu[i * 8 + 5];
        row[6] = mcu[i * 8 + 6];
        row[7] = mcu[i * 8 + 7];

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
        column[0] = mcu[i];
        column[1] = mcu[i + 8];
        column[2] = mcu[i + 16];
        column[3] = mcu[i + 24];
        column[4] = mcu[i + 32];
        column[5] = mcu[i + 40];
        column[6] = mcu[i + 48];
        column[7] = mcu[i + 56];

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

    return mcu;
}
int* test2_dct(int* source_mcu)
{
    int* mcu = new int[65];
    for (int i = 0; i < 64; i++) mcu[i] = (int)source_mcu[i];
    //row DCT
    for (int i = 0; i < 8; i++)
    {

        //initialization
        int f0, f1, f2, f3, f4, f5, f6, f7;
        f0 = mcu[i * 8];
        f1 = mcu[1 + i * 8];
        f2 = mcu[2 + i * 8];
        f3 = mcu[3 + i * 8];
        f4 = mcu[4 + i * 8];
        f5 = mcu[5 + i * 8];
        f6 = mcu[6 + i * 8];
        f7 = mcu[7 + i * 8];

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
        f0 = mcu[i];
        f1 = mcu[i + 8];
        f2 = mcu[i + 16];
        f3 = mcu[i + 24];
        f4 = mcu[i + 32];
        f5 = mcu[i + 40];
        f6 = mcu[i + 48];
        f7 = mcu[i + 56];

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
    return mcu;
}
static const float mp0 = 0.7071;
static const float mp1 = 0.9239;
static const float mp2 = 0.3827;
static const float mp3 = 1.4142;
int* dct_II_int_8t_int_8x8_Version3(int* source_mcu)
{
    int* mcu = new int[65];
    for (int i = 0; i < 64; i++) mcu[i] = (int)source_mcu[i];
    //column DCT
    for (int i = 0; i < 8; i++)
    {

        //initialization
        int a0, a1, a2, a3, a4, a5, a6, a7;
        a0 = mcu[i];
        a1 = mcu[i + 8];
        a2 = mcu[i + 16];
        a3 = mcu[i + 24];
        a4 = mcu[i + 32];
        a5 = mcu[i + 40];
        a6 = mcu[i + 48];
        a7 = mcu[i + 56];

        int b0, b1, b2, b3, b4, b5, b6, b7;
        b0 = a0 + a7;
        b1 = a1 + a6;
        b2 = a2 + a5;
        b3 = a3 + a4;

        b4 = a3 - a4;
        b5 = a2 - a5;
        b6 = a1 - a6;
        b7 = a0 - a7;

        int c0, c1, c2, c3, c4, c5, c6, c7;
        c0 = b0 + b3;
        c1 = b1 + b2;
        c2 = b1 - b2;
        c3 = b0 - b3;

        c4 = -b4 - b5;
        c5 = b5 + b6;
        c6 = b6 + b7;
        c7 = b7;

        float d0, d1, d2, d3, d4, d5, d6, d7;
        d0 = c0 + c1;
        d1 = c0 - c1;
        d2 = (float)(c2 + c3) * mp0;
        d3 = c3;

        d4 = ((float)c4 * mp1) + ((float)c6 * mp2);
        d5 = (float)c5 * mp0;
        d6 = ((float)c6 * mp1) - ((float)c4 * mp2);
        d7 = c7;

        float e0, e1, e2, e3, e4, e5, e6, e7;
        e0 = d0;
        e1 = d1;
        e2 = d2 + d3;
        e3 = d3 - d2;

        e4 = -d4;
        e5 = d5 + d7;
        e6 = d6;
        e7 = d7 - d5;

        float f0, f1, f2, f3, f4, f5, f6, f7;
        f0 = e0 / mp3;
        f1 = e1 / mp3;
        f2 = e2;
        f3 = e3;

        f4 = e4 + e7;
        f5 = e5 + e6;
        f6 = e5 - e6;
        f7 = e7 - e4;

        mcu[i] = ((int)f0);
        mcu[i + 32] = ((int)f1);
        mcu[i + 16] = ((int)f2) >> 2;
        mcu[i + 48] = ((int)f3) >> 2;
        mcu[i + 40] = ((int)f4) >> 2;
        mcu[i + 8] = ((int)f5) >> 2;
        mcu[i + 56] = ((int)f6) >> 2;
        mcu[i + 24] = ((int)f7) >> 2;

    }
    
    //row DCT
    for (int i = 0; i < 8; i++)
    {

        //initialization
        int a0, a1, a2, a3, a4, a5, a6, a7;
        a0 = mcu[i * 8];
        a1 = mcu[1 + i * 8];
        a2 = mcu[2 + i * 8];
        a3 = mcu[3 + i * 8];
        a4 = mcu[4 + i * 8];
        a5 = mcu[5 + i * 8];
        a6 = mcu[6 + i * 8];
        a7 = mcu[7 + i * 8];

        int b0, b1, b2, b3, b4, b5, b6, b7;
        b0 = a0 + a7;
        b1 = a1 + a6;
        b2 = a2 + a5;
        b3 = a3 + a4;

        b4 = a3 - a4;
        b5 = a2 - a5;
        b6 = a1 - a6;
        b7 = a0 - a7;

        int c0, c1, c2, c3, c4, c5, c6, c7;
        c0 = b0 + b3;
        c1 = b1 + b2;
        c2 = b1 - b2;
        c3 = b0 - b3;

        c4 = -b4 - b5;
        c5 = b5 + b6;
        c6 = b6 + b7;
        c7 = b7;

        float d0, d1, d2, d3, d4, d5, d6, d7;
        d0 = c0 + c1;
        d1 = c0 - c1;
        d2 = (float)(c2 + c3) * mp0;
        d3 = c3;

        d4 = ((float)c4 * mp1) + ((float)c6 * mp2);
        d5 = (float)c5 * mp0;
        d6 = ((float)c6 * mp1) - ((float)c4 * mp2);
        d7 = c7;

        float e0, e1, e2, e3, e4, e5, e6, e7;
        e0 = d0;
        e1 = d1;
        e2 = d2 + d3;
        e3 = d3 - d2;

        e4 = -d4;
        e5 = d5 + d7;
        e6 = d6;
        e7 = d7 - d5;

        float f0, f1, f2, f3, f4, f5, f6, f7;
        f0 = e0 / mp3;
        f1 = e1 / mp3;
        f2 = e2;
        f3 = e3;

        f4 = e4 + e7;
        f5 = e5 + e6;
        f6 = e5 - e6;
        f7 = e7 - e4;

        mcu[i * 8] = ((int)f0);
        mcu[i * 8 + 4] = ((int)f1);
        mcu[i * 8 + 2] = ((int)f2) >> 2;
        mcu[i * 8 + 6] = ((int)f3) >> 2;
        mcu[i * 8 + 5] = ((int)f4) >> 2;
        mcu[i * 8 + 1] = ((int)f5) >> 2;
        mcu[i * 8 + 7] = ((int)f6) >> 2;
        mcu[i * 8 + 3] = ((int)f7) >> 2;

    }
    return mcu;
}
const float m00 = 2.0 * std::cos(1.0 / 16.0 * 2.0 * 3.14);
const float m10 = 2.0 * std::cos(2.0 / 16.0 * 2.0 * 3.14);
const float m30 = 2.0 * std::cos(2.0 / 16.0 * 2.0 * 3.14);
const float m50 = 2.0 * std::cos(3.0 / 16.0 * 2.0 * 3.14);
const float m20 = m00 - m50;
const float m40 = m00 + m50;

const float s0 = std::cos(0.0 / 16.0 * 3.14) / std::sqrt(8);
const float s1 = std::cos(1.0 / 16.0 * 3.14) / 2.0;
const float s2 = std::cos(2.0 / 16.0 * 3.14) / 2.0;
const float s3 = std::cos(3.0 / 16.0 * 3.14) / 2.0;
const float s4 = std::cos(4.0 / 16.0 * 3.14) / 2.0;
const float s5 = std::cos(5.0 / 16.0 * 3.14) / 2.0;
const float s6 = std::cos(6.0 / 16.0 * 3.14) / 2.0;
const float s7 = std::cos(7.0 / 16.0 * 3.14) / 2.0;
int* straightCopyDct(int* source_mcu) {

    int* mcu = new int[65];
    for (int i = 0; i < 64; i++) mcu[i] = (int)source_mcu[i];

    for (uint8_t i = 0; i < 8; ++i) {
        const float a0 = mcu[0 * 8 + i];
        const float a1 = mcu[1 * 8 + i];
        const float a2 = mcu[2 * 8 + i];
        const float a3 = mcu[3 * 8 + i];
        const float a4 = mcu[4 * 8 + i];
        const float a5 = mcu[5 * 8 + i];
        const float a6 = mcu[6 * 8 + i];
        const float a7 = mcu[7 * 8 + i];

        const float b0 = a0 + a7;
        const float b1 = a1 + a6;
        const float b2 = a2 + a5;
        const float b3 = a3 + a4;
        const float b4 = a3 - a4;
        const float b5 = a2 - a5;
        const float b6 = a1 - a6;
        const float b7 = a0 - a7;

        const float c0 = b0 + b3;
        const float c1 = b1 + b2;
        const float c2 = b1 - b2;
        const float c3 = b0 - b3;
        const float c4 = b4;
        const float c5 = b5 - b4;
        const float c6 = b6 - c5;
        const float c7 = b7 - b6;

        const float d0 = c0 + c1;
        const float d1 = c0 - c1;
        const float d2 = c2;
        const float d3 = c3 - c2;
        const float d4 = c4;
        const float d5 = c5;
        const float d6 = c6;
        const float d7 = c5 + c7;
        const float d8 = c4 - c6;

        const float e0 = d0;
        const float e1 = d1;
        const float e2 = d2 * m10;
        const float e3 = d3;
        const float e4 = d4 * m20;
        const float e5 = d5 * m30;
        const float e6 = d6 * m40;
        const float e7 = d7;
        const float e8 = d8 * m50;

        const float f0 = e0;
        const float f1 = e1;
        const float f2 = e2 + e3;
        const float f3 = e3 - e2;
        const float f4 = e4 + e8;
        const float f5 = e5 + e7;
        const float f6 = e6 + e8;
        const float f7 = e7 - e5;

        const float g0 = f0;
        const float g1 = f1;
        const float g2 = f2;
        const float g3 = f3;
        const float g4 = f4 + f7;
        const float g5 = f5 + f6;
        const float g6 = f5 - f6;
        const float g7 = f7 - f4;

        mcu[0 * 8 + i] = g0 * s0;
        mcu[4 * 8 + i] = g1 * s4;
        mcu[2 * 8 + i] = g2 * s2;
        mcu[6 * 8 + i] = g3 * s6;
        mcu[5 * 8 + i] = g4 * s5;
        mcu[1 * 8 + i] = g5 * s1;
        mcu[7 * 8 + i] = g6 * s7;
        mcu[3 * 8 + i] = g7 * s3;
    }
    for (uint8_t i = 0; i < 8; ++i) {
        const float a0 = mcu[i * 8 + 0];
        const float a1 = mcu[i * 8 + 1];
        const float a2 = mcu[i * 8 + 2];
        const float a3 = mcu[i * 8 + 3];
        const float a4 = mcu[i * 8 + 4];
        const float a5 = mcu[i * 8 + 5];
        const float a6 = mcu[i * 8 + 6];
        const float a7 = mcu[i * 8 + 7];

        const float b0 = a0 + a7;
        const float b1 = a1 + a6;
        const float b2 = a2 + a5;
        const float b3 = a3 + a4;
        const float b4 = a3 - a4;
        const float b5 = a2 - a5;
        const float b6 = a1 - a6;
        const float b7 = a0 - a7;

        const float c0 = b0 + b3;
        const float c1 = b1 + b2;
        const float c2 = b1 - b2;
        const float c3 = b0 - b3;
        const float c4 = b4;
        const float c5 = b5 - b4;
        const float c6 = b6 - c5;
        const float c7 = b7 - b6;

        const float d0 = c0 + c1;
        const float d1 = c0 - c1;
        const float d2 = c2;
        const float d3 = c3 - c2;
        const float d4 = c4;
        const float d5 = c5;
        const float d6 = c6;
        const float d7 = c5 + c7;
        const float d8 = c4 - c6;

        const float e0 = d0;
        const float e1 = d1;
        const float e2 = d2 * m10;
        const float e3 = d3;
        const float e4 = d4 * m20;
        const float e5 = d5 * m30;
        const float e6 = d6 * m40;
        const float e7 = d7;
        const float e8 = d8 * m50;

        const float f0 = e0;
        const float f1 = e1;
        const float f2 = e2 + e3;
        const float f3 = e3 - e2;
        const float f4 = e4 + e8;
        const float f5 = e5 + e7;
        const float f6 = e6 + e8;
        const float f7 = e7 - e5;

        const float g0 = f0;
        const float g1 = f1;
        const float g2 = f2;
        const float g3 = f3;
        const float g4 = f4 + f7;
        const float g5 = f5 + f6;
        const float g6 = f5 - f6;
        const float g7 = f7 - f4;

        mcu[i * 8 + 0] = g0 * s0;
        mcu[i * 8 + 4] = g1 * s4;
        mcu[i * 8 + 2] = g2 * s2;
        mcu[i * 8 + 6] = g3 * s6;
        mcu[i * 8 + 5] = g4 * s5;
        mcu[i * 8 + 1] = g5 * s1;
        mcu[i * 8 + 7] = g6 * s7;
        mcu[i * 8 + 3] = g7 * s3;
    }
    return mcu;
}
const float multA1 = 0.707;
const float multA2 = 0.541;
const float multA3 = 0.707;
const float multA4 = 1.307;
const float multA5 = 0.383;
int* dct_attempt5Billion(int* source_mcu)
{
    int* mcu = new int[65];
    for (int i = 0; i < 64; i++) mcu[i] = source_mcu[i];
    
    //row dct
    for (int i = 0; i < 8; ++i) {
        int f0, f1, f2, f3, f4, f5, f6, f7;
        f0 = mcu[i];
        f1 = mcu[8 + i];
        f2 = mcu[16 + i];
        f3 = mcu[24 + i];
        f4 = mcu[32 + i];
        f5 = mcu[40 + i];
        f6 = mcu[48 + i];
        f7 = mcu[56 + i];

        //step one
        int a0, a1, a2, a3, a4, a5, a6, a7;

        a0 = f0 + f7;
        a1 = f1 + f6;
        a2 = f2 + f5;
        a3 = f3 + f4;

        a4 = f3 - f4;
        a5 = f2 - f5;
        a6 = f1 - f6;
        a7 = f0 - f7;

        //step two
        int b0, b1, b2, b3, b4, b5, b6, b7;

        b0 = a0 + a3;
        b1 = a1 + a2;
        b2 = a1 - a2;
        b3 = a0 - a3;

        b4 = -a4 - a5;
        b5 = a5 + a6;
        b6 = a6 + a7;
        b7 = a7;

        //step three
        int c0, c1, c2, c3, c4, c5, c6, c7;

        c0 = b0 + b1;
        c1 = b0 - b1;
        c2 = b2 + b3;
        c3 = b3;

        c4 = b4;
        c5 = b5;
        c6 = b6;
        c7 = b7;

        //step four: multiplication
        float d0, d1, d2, d3, d4, d5, d6, d7, d8;

        d0 = c0;
        d1 = c1;
        d2 = (float)c2 * multA1;
        d3 = c3;

        d8 = -(float)(c4 + c6) * multA5;//special term

        d4 = d8 - ((float)c4 * multA2);
        d5 = (float)c5 * multA3;
        d6 = d8 - ((float)c6 * multA4);
        d7 = c7;

        //step five
        float e0, e1, e2, e3, e4, e5, e6, e7;

        e0 = d0;
        e1 = d1;
        e2 = d2 + d3;
        e3 = d3 - d2;

        e4 = d4;
        e5 = d5 + d7;
        e6 = d6;
        e7 = d7 - d5;

        //final step
        int F0, F1, F2, F3, F4, F5, F6, F7;
        
        F0 = (int)e0 / 8;
        F4 = (int)e1 / 16;
        F2 = (int)e2 / 16;
        F6 = (int)e3 / 16;

        F5 = (int)(e4 + e7) / 16;
        F1 = (int)(e5 + e6) / 16;
        F7 = (int)(e5 - e6) / 16;
        F3 = (int)(e7 - e4) / 16;

        //insert
        
        mcu[i] = F0;
        mcu[8 + i] = F1;
        mcu[16 + i] = F2;
        mcu[24 + i] = F3;
        mcu[32 + i] = F4;
        mcu[40 + i] = F5;
        mcu[48 + i] = F6;
        mcu[56 + i] = F7;
    
    }
    
    //column dct
    for (int i = 0; i < 8; ++i) 
    {
        int f0, f1, f2, f3, f4, f5, f6, f7;
        f0 = mcu[i * 8 ];
        f1 = mcu[i * 8 + 1];
        f2 = mcu[i * 8 + 2];
        f3 = mcu[i * 8 + 3];
        f4 = mcu[i * 8 + 4];
        f5 = mcu[i * 8 + 5];
        f6 = mcu[i * 8 + 6];
        f7 = mcu[i * 8 + 7];

        //step one
        int a0, a1, a2, a3, a4, a5, a6, a7;

        a0 = f0 + f7;
        a1 = f1 + f6;
        a2 = f2 + f5;
        a3 = f3 + f4;

        a4 = f3 - f4;
        a5 = f2 - f5;
        a6 = f1 - f6;
        a7 = f0 - f7;

        //step two
        int b0, b1, b2, b3, b4, b5, b6, b7;

        b0 = a0 + a3;
        b1 = a1 + a2;
        b2 = a1 - a2;
        b3 = a0 - a3;

        b4 = -a4 - a5;
        b5 = a5 + a6;
        b6 = a6 + a7;
        b7 = a7;

        //step three
        int c0, c1, c2, c3, c4, c5, c6, c7;

        c0 = b0 + b1;
        c1 = b0 - b1;
        c2 = b2 + b3;
        c3 = b3;

        c4 = b4;
        c5 = b5;
        c6 = b6;
        c7 = b7;

        //step four: multiplication
        float d0, d1, d2, d3, d4, d5, d6, d7, d8;

        d0 = c0;
        d1 = c1;
        d2 = (float)c2 * multA1;
        d3 = c3;

        d8 = -(float)(c4 + c6) * multA5;//special term

        d4 = d8 - ((float)c4 * multA2);
        d5 = (float)c5 * multA3;
        d6 = d8 - ((float)c6 * multA4);
        d7 = c7;

        //step five
        float e0, e1, e2, e3, e4, e5, e6, e7;

        e0 = d0;
        e1 = d1;
        e2 = d2 + d3;
        e3 = d3 - d2;

        e4 = d4;
        e5 = d5 + d7;
        e6 = d6;
        e7 = d7 - d5;

        //final step
        int F0, F1, F2, F3, F4, F5, F6, F7;

        F0 = (int)e0 / 8;
        F4 = (int)e1 / 16;
        F2 = (int)e2 / 16;
        F6 = (int)e3 / 16;

        F5 = (int)(e4 + e7) / 16;
        F1 = (int)(e5 + e6) / 16;
        F7 = (int)(e5 - e6) / 16;
        F3 = (int)(e7 - e4) / 16;

        //insert

        mcu[i * 8] = F0;
        mcu[i * 8 + 1] = F1;
        mcu[i * 8 + 2] = F2;
        mcu[i * 8 + 3] = F3;
        mcu[i * 8 + 4] = F4;
        mcu[i * 8 + 5] = F5;
        mcu[i * 8 + 6] = F6;
        mcu[i * 8 + 7] = F7;
    }
    return mcu;
    
}
int* dctATTEMPTTRILLION(int* source_mcu)
{
        float tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
        float tmp10, tmp11, tmp12, tmp13;
        float z1, z2, z3, z4, z5, z11, z13;
        int* mcu = new int[65];
        for (int i = 0; i < 64; i++) mcu[i] = source_mcu[i];
        /* Pass 1: process rows. */
        for (int i = 0; i < 8; i++) {
            tmp0 = mcu[i * 8] + mcu[i * 8 + 7];
            tmp7 = mcu[i * 8] - mcu[i * 8 + 7];
            tmp1 = mcu[i * 8 + 1] + mcu[i * 8 + 6];
            tmp6 = mcu[i * 8 + 1] - mcu[i * 8 + 6];
            tmp2 = mcu[i * 8 + 2] + mcu[i * 8 + 5];
            tmp5 = mcu[i * 8 + 2] - mcu[i * 8 + 5];
            tmp3 = mcu[i * 8 + 3] + mcu[i * 8 + 4];
            tmp4 = mcu[i * 8 + 3] - mcu[i * 8 + 4];

            /* Even part */

            tmp10 = tmp0 + tmp3;        /* phase 2 */
            tmp13 = tmp0 - tmp3;
            tmp11 = tmp1 + tmp2;
            tmp12 = tmp1 - tmp2;

            mcu[i * 8] = (tmp10 + tmp11) / 2; /* phase 3 */
            mcu[i * 8 + 4] = (tmp10 - tmp11) / 4;

            z1 = (tmp12 + tmp13) * ((float)0.707106781); /* c4 */
            mcu[i * 8 + 2] = (tmp13 + z1) / 4;    /* phase 5 */
            mcu[i * 8 + 6] = (tmp13 - z1) / 4;

            /* Odd part */

            tmp10 = tmp4 + tmp5;        /* phase 2 */
            tmp11 = tmp5 + tmp6;
            tmp12 = tmp6 + tmp7;

            /* The rotator is modified from fig 4-8 to avoid extra negations. */
            z5 = (tmp10 - tmp12) * ((float)0.382683433); /* c6 */
            z2 = ((float)0.541196100) * tmp10 + z5; /* c2-c6 */
            z4 = ((float)1.306562965) * tmp12 + z5; /* c2+c6 */
            z3 = tmp11 * ((float)0.707106781); /* c4 */

            z11 = tmp7 + z3;            /* phase 5 */
            z13 = tmp7 - z3;

            mcu[i * 8 + 5] = (z13 + z2) / 4;      /* phase 6 */
            mcu[i * 8 + 3] = (z13 - z2) / 4;
            mcu[i * 8 + 1] = (z11 + z4) / 4;
            mcu[i * 8 + 7] = (z11 - z4) / 4;

        }

        /* Pass 2: process columns. */

        for (int i = 0; i < 8; i++) {
            tmp0 = mcu[i] + mcu[i + 56];
            tmp7 = mcu[i] - mcu[i + 56];
            tmp1 = mcu[i + 8] + mcu[i + 48];
            tmp6 = mcu[i + 8] - mcu[i + 48];
            tmp2 = mcu[i + 16] + mcu[i + 40];
            tmp5 = mcu[i + 16] - mcu[i + 40];
            tmp3 = mcu[i + 24] + mcu[i + 32];
            tmp4 = mcu[i + 24] - mcu[i + 32];

            /* Even part */

            tmp10 = tmp0 + tmp3;        /* phase 2 */
            tmp13 = tmp0 - tmp3;
            tmp11 = tmp1 + tmp2;
            tmp12 = tmp1 - tmp2;

            mcu[i] = (tmp10 + tmp11) / 2; /* phase 3 */
            mcu[i + 32] = (tmp10 - tmp11) / 4;

            z1 = (tmp12 + tmp13) * ((float)0.707106781); /* c4 */
            mcu[i + 16] = (tmp13 + z1) / 4; /* phase 5 */
            mcu[i + 48] = (tmp13 - z1) / 4;

            /* Odd part */

            tmp10 = tmp4 + tmp5;        /* phase 2 */
            tmp11 = tmp5 + tmp6;
            tmp12 = tmp6 + tmp7;

            /* The rotator is modified from fig 4-8 to avoid extra negations. */
            z5 = (tmp10 - tmp12) * ((float)0.382683433); /* c6 */
            z2 = ((float)0.541196100) * tmp10 + z5; /* c2-c6 */
            z4 = ((float)1.306562965) * tmp12 + z5; /* c2+c6 */
            z3 = tmp11 * ((float)0.707106781); /* c4 */

            z11 = tmp7 + z3;            /* phase 5 */
            z13 = tmp7 - z3;

            mcu[i + 40] = (z13 + z2) / 4; /* phase 6 */
            mcu[i + 24] = (z13 - z2) / 4;
            mcu[i + 8] = (z11 + z4) / 4;
            mcu[i + 56] = (z11 - z4) / 4;
        }
        return mcu;
}
int* dct_II_int_8t_int_4x4_TEST(int* source_mcu)
{
    int* mcu = new int[17];

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

    return mcu;
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
            for(int n = 0; n < 3; n++)//if we ever want to make it monochrome...
            {
                mcus[pos].yCbCr(n) = dctQuantizer(origin.mcuList[pos].ycbcr[n], this->dim[n], ((pos == 0) ? 0 : mcus[pos - 1].yCbCr[n][(dim[n] == 8 ? 64 : 16)]), n);
                yCHuffman[n == 0 ? 0 : 1].frequency(this->dim[n], mcus[pos].yCbCr[n]);
            }
            
        }
    }
    
    yCHuffman[0].huffmanCodes();
    yCHuffman[1].huffmanCodes();
}
int mcuHuffmanContainer::size()
{
    return this->mcuHeight * this->mcuLength;
}
void mcuHuffmanContainer::actualJpg(hInfoStruct hInfo, mcuHuffmanContainer mcuHuffman, char* fileName)
{
    byteWritter bW = byteWritter(fileName);

    if (bW.open())
    {
        //write jpg...
        SOI_APPO_M(bW, hInfo.densityY, hInfo.densityX, hInfo.density);
        DQT_M(bW, (hInfo.lumaTableType == 64 ? lumTable : lumTable_2), hInfo.lumaTableType, 0);//luma table will be used if monochrome
        if (hInfo.chromaTableType != 0) DQT_M(bW, hInfo.chromaTableType == 64 ? chromaTable : chromaTable, 64, 1);//check if chromatable exist (will not if monochrome image)
        SOF_M(bW, hInfo.chromaTableType, hInfo.samplingY, hInfo.samplingC, hInfo.pixelHeight, hInfo.pixelLength);
        DHT_M(bW, 0, 0, mcuHuffman.yCHuffman[0].DCcodeLength);
        DHT_M(bW, 1, 0, mcuHuffman.yCHuffman[0].ACcodeLength);
        if (hInfo.chromaTableType != 0)
        {
            DHT_M(bW, 0, 1, mcuHuffman.yCHuffman[1].DCcodeLength);
            DHT_M(bW, 1, 1, mcuHuffman.yCHuffman[1].ACcodeLength);
        }
        int chromad = hInfo.chromaTableType != 0 ? 3 : 1;
        SOS_M(bW, chromad, hInfo.lumaTableType);
        bW.inScanFlip(); //decoment this after testing
        for (int i = 0; i < mcuHuffman.size(); i++)
        {
            if (i == 24)testVar = true;
            MCU_W(bW, mcuHuffman.yCHuffman[0].DCcode, mcuHuffman.yCHuffman[0].DCcodeLength, mcuHuffman.yCHuffman[0].ACcode, mcuHuffman.yCHuffman[0].ACcodeLength, hInfo.lumaTableType, mcuHuffman.mcus[i].yCbCr[0]);
            testVar = false;
            if (chromad == 3)
            {
                MCU_W(bW, mcuHuffman.yCHuffman[1].DCcode, mcuHuffman.yCHuffman[1].DCcodeLength, mcuHuffman.yCHuffman[1].ACcode, mcuHuffman.yCHuffman[1].ACcodeLength, hInfo.chromaTableType, mcuHuffman.mcus[i].yCbCr[1]);
                MCU_W(bW, mcuHuffman.yCHuffman[1].DCcode, mcuHuffman.yCHuffman[1].DCcodeLength, mcuHuffman.yCHuffman[1].ACcode, mcuHuffman.yCHuffman[1].ACcodeLength, hInfo.chromaTableType, mcuHuffman.mcus[i].yCbCr[2]);
            }
        }
        bW.inScanFlip(); //decoment this after testing
        EOI_M(bW);
    }
}
int* dctQuantizer(int* table,int dim, int previousDC, int num) 
{
    int* returnTable;

    //int* returnTable = (int*)malloc((dim * dim + 1) * sizeof(int));
    if (dim == 8)
    {
        //returnTable = dct_II_int_8t_int_8x8(table);
        //testing
        //returnTable = test_8x8_int_DCT(table);
        //returnTable = test2_dct(table);
        //returnTable = dct_II_int_8t_int_8x8_Version3(table);
        returnTable = straightCopyDct(table);
        //returnTable = dct_attempt5Billion(table);
        //returnTable = dctATTEMPTTRILLION(table);
        quantizer_8x8_int(returnTable, quantTables8x8[num]);

    }
    else
    {
        exit(0);
        //returnTable = dct_II_uint_8t_int_4x4(table, previousDC);
        returnTable = dct_II_int_8t_int_4x4_TEST(table);
        quantizer_4x4_int(returnTable, quantTables4x4[num]);
    }
    returnTable[dim * dim] = returnTable[0] + 0;
    returnTable[0] = returnTable[0] - previousDC;
    return returnTable;
}
void huffmanTable::frequency(int dim, int* table)
{
    //dc
    //std::cout << "\n << " << table[0];
    int coeffLength = bitLength(table[0] > 0 ? table[0] : -table[0]);
    if (DCcodeLength.addPair(coeffLength, 1) == -1) 
    {
        DCcodeLength.changeTerm(coeffLength, DCcodeLength.retrieveTerm(coeffLength) + 1);
    }

    //ac
    int zeroes = 0;
    for (int i = 1; i < dim * dim; i++, zeroes++)
    {
        int coeff = table[dim == 8 ? zigging[i] : zigging2[i]];
        if (coeff != 0)
        {//if the coeff is zero we dont add, because the huffman values have coeff and previous values
            if (zeroes > 15)
            {
                while (zeroes > 15)
                {//if the coeff has more than 15 zeroes we need to add the special code f0 which means 16 zeroes
                    if (ACcodeLength.addPair(f0, 1) == -1)ACcodeLength.changeTerm(f0, ACcodeLength.retrieveTerm(f0) + 1);
                    zeroes -= 16;
                }
            }
            coeffLength = bitLength(coeff > 0 ? coeff : -coeff);
            if (bitLength(coeffLength) > 4) {
                std::cout << "error coeff length greater than 4: " << coeffLength << " || coeff: " << coeff << " || I: " << i;
                exit(3);
            }
            coeffLength += (zeroes << 4);//the zeroes value are stored as the frist 4 bits and the coeff length is stored as the last 4 bits, if all goes well in the dct the coeffient should never exceed 4 bit length so this should work
            if (ACcodeLength.addPair(coeffLength, 1) == -1) 
            { 
                ACcodeLength.changeTerm(coeffLength, ACcodeLength.retrieveTerm(coeffLength) + 1); 
            }
            zeroes = -1;//evil
        }
        else if (i == ((dim * dim) - 1))
        {
            if (ACcodeLength.addPair(x00, 1) == -1)ACcodeLength.changeTerm(x00, ACcodeLength.retrieveTerm(x00) + 1);
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
    
    //if there is a 1 key it messses up the entire thing... so just add 1 to it and think it works, at least im pretty sure thats what the itu is saying... also if there are just two just add 1 to the second one instead... for obvious reasons
    //also it messes it up because it is illegal to have values of only 1 bits, and if it starts at 1 I think it necessarily will have values of 1 bits only at a length past like 5
    //if(keysDC[0] == 1)
    //{
    //    if (keysDC[1] == 1) keysDC[1] += 1;
    //    else keysDC[0] +=1;
    //}
    //if (keysAC[0] == 1)
    //{
    //    if (keysAC[1] == 1) keysAC[1] += 1;
    //    else keysAC[0] += 1;
    //}

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
    //doesnt work sometimes just do it eight times lol

    huffManReferenceTable(this->DCcodeLength, this->DCcode);
    huffManReferenceTable(this->ACcodeLength, this->ACcode, true);

    uint16_t* dcs = this->DCcode.retrieveAllTerms();
    uint16_t* acs = this->ACcode.retrieveAllTerms();

    int* acsL = this->ACcodeLength.retrieveAllTerms();
    int* dcsL = this->DCcodeLength.retrieveAllTerms();

    int acC = this->ACcodeLength.returnCount() - 1;
    int dcC = this->DCcodeLength.returnCount() - 1;


    //okay I think the bits being all equal to 1 only occurs in the final code... or at least I think thats how it works... I think?... I don't know we'll see
    if(bitsAllOne(dcs[dcC], dcsL[dcC]) == 1)
    {
        dcs[dcC] <<= 1;
        dcsL[dcC] += 1;
    }
    if (bitsAllOne(acs[acC], acsL[acC]) == 1)
    {
        acs[acC] <<= 1;
        acsL[acC] += 1;
    }
    

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
        return new int[1] {0};//all codes have a minimum code length of 1, but the start node is 0
    }
}
void huffManReferenceTable(fakeDictionary<int, int>& huffmanCodeLength, fakeDictionary<int, uint16_t> &target,bool b)//this creates a dictionary for referencing the code values later
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
    uint16_t code = 0;
    uint8_t codeLength = funnyPtr2[0];
    for (int i = 0; i < count; i++)
    {
        if (i != 0 && codeLength != funnyPtr2[i])
        {
            code = code << (funnyPtr2[i] - codeLength);
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
int customSorter(uint8_t a, uint8_t b)
{
    if (a >= b)return 0;
    return 1;
}
int bitLength(int bits)//gives the position of the most significant bit
{
    unsigned bitDestroyer = bits;
    unsigned bitLength;
    for (bitLength = 0; bitDestroyer != 0; ++bitLength) bitDestroyer >>= 1;
    return bitLength;
}
int bitsAllOne(uint16_t bits, int length)//if bits are all 1 return 1, if not return 0
{
    uint16_t bitMx = 1;
    for (int i = 0; i < length; i++)
    {
        bitMx *= 2;
    }
    if (bitMx - 1 == bits)
    {
        return 1;
    }
    return 0;
}
void SOI_APPO_M(byteWritter& bw, int densityY, int densityX, int density)
{
    bw.write(ff); bw.write(216);//SOI Marker, START OF IMAGE MARKER
    bw.write(ff); bw.write(224);//APPO Marker, application date marker
    bw.write(0); bw.write(16);//APPO segment size, because we are not using the thumbnail this will always be the same size
    bw.write(74); bw.write(70); bw.write(73); bw.write(70); bw.write(0);//JFIF indentifier string, this constant identifies the jpeg as a jfif type, which is standard
    bw.write(1); bw.write(1);//JFIF version, 1.01
    bw.write((uint8_t)density);//these next three are the resoultuion density of the image. say that the resolution is 3 by 2, so every inch there is three pixels y and 2 pixels x
    bw.write((uint16_t)densityY, 16);//this means we would set density to 3
    bw.write((uint16_t)densityX, 16);//and y density to 2 x density to 2...... however it is almost always zero density and 1, 1 for y density and x density, because no one uses different density images
    bw.write(0);//thumbnailsize
    bw.write(0);//thumbnailsizey
}
void DQT_M(byteWritter& bw, const int* table, int tableNum, int num)
{
    bw.write(ff); bw.write(219);//DQT header, Data Quantization Table Header
    bw.write((uint16_t)(tableNum + 3), 16);//Header length, table length + 3 bits written here not including marker
    bw.write((uint8_t)0, 4); bw.write((uint8_t)num, 4);// this byte here contains two bits (lol not actually bits) of information, the table integer size either 8 bit or 16 bit and the table id number. We will only be using 8 bit which is 0000 (16 is 0001)m abd the second bit contains the id, possible nums 0-3
    for (int i = 0; i < tableNum; i++)
    {
        bw.write(table[tableNum == 64 ? zigging[i] : zigging2[i]] > 254 ? 254 : table[tableNum == 64 ? zigging[i] : zigging2[i]]);
    }
}
void SOF_M(byteWritter& bw, int chromaExist, int ySamplingSize, int cSamplingSize, int pixelHeight, int pixelWidth) 
{
    bw.write(ff); bw.write(192);//S0F header, start of frame header type 0
    bw.write((uint8_t)(chromaExist != 0 ? 17 : 11), 16);//length of segment, if monochrome only 11 bytes if not 17 
    bw.write(8);//jpg pixel precision, jpg only supports 8bit precision but it must be included anyway lol
    bw.write((uint16_t)pixelHeight, 16);//jpg pixel height, stored in two bits maxinum size of 256 x 256
    bw.write((uint16_t)pixelWidth, 16);//jpg pixel length
    bw.write((chromaExist ? 3 : 1));
    bw.write(1); bw.write(ySamplingSize == 1 ? 34 : 17); bw.write(0);//y component or monochrome component data, sampling size will either by 1 or 2, should almost always be 1 for the y component
    if (chromaExist != 0)
    {
        bw.write(2); bw.write(cSamplingSize == 1 ? 34 : 17); bw.write(1);//cb component, sampling size should almost always be 2 and therefor 0x11, also quant tables are at the end, chroma uses quant table one luma uses quant table 0
        bw.write(3); bw.write(cSamplingSize == 1 ? 34 : 17); bw.write(1);
    }

}
void DHT_M(byteWritter& bw, int ACDC /*1 = AC 0 = DC*/, int tableNum /*tablenum 0-3*/, fakeDictionary<int, int>& codeFreq)
{
    bw.write(ff); bw.write(196);//DHT Header, define huffman table header
    bw.write((uint16_t)(19 + codeFreq.returnCount()), 16);//code length, codelength (2) + tableinfo(1) + number codeLengths(16) + symbols(variable)
    bw.write(16 * ACDC + tableNum);//table info, first 4 bits are 0 if dc 1 is ac, last 4 bits are tablenum 0 is going to be y and 1 is going to be the chrominance
    //below is the # of code lengths, which is stored instead ofthe codes as you can just reconstruct the codes...
    for (int i = 1; i < 17; i++) {
        bw.write(codeFreq.retrieveTermCount(i));
    }
    //below are the symbols, our dictionary is already sorted into the proper format so we just use that
    //the way this works is the symbols below are tied to the code lengths above, the first symbol below is tied to the first code length above and so on
    int* huffValues = codeFreq.retrieveAllKeys();
    for (int i = 0; i < codeFreq.returnCount(); i++)
    {
        bw.write((uint8_t)huffValues[i]);
    }
}
void SOS_M(byteWritter& bw, int components, int firstComponentLength)
{
    bw.write(ff); bw.write(218);//SOS marker, start of scan marker
    bw.write((uint16_t)(components * 2 + 6), 16);//length of marker, variable
    bw.write(components);
    for (int i = 0; i < 3; i++)
    {
        bw.write(i + 1);//component
        bw.write(i == 0 ? 0 : 17);//component dc/ac table, first 4 bits are dc table id last 4 bits ac table
        //because we are using standard sequential y and chrominance tables we know the first component y will have the table ids of 0 and the second and third components cb and cr will be 1
    }
    bw.write(0);//start of mcus
    bw.write(firstComponentLength - 1);//length of mcus...
    bw.write(0);//hurrr
}
void MCU_W(byteWritter& bw, fakeDictionary<int, uint16_t>& huffmanDcValueCodes, fakeDictionary<int, int>& huffmanDcValueLength, fakeDictionary<int, uint16_t>& huffmanAcValueCodes, fakeDictionary<int, int>& huffmanAcValueLength, int dim, int* table)
{
    //write dc
    int coeffLength = bitLength(table[0] > 0 ? table[0] : -table[0]);
    bw.write(huffmanDcValueCodes.retrieveTerm(coeffLength), huffmanDcValueLength.retrieveTerm(coeffLength));//write dchuffman value

    unsigned funny = 1;
    for (int two = 0; two < coeffLength; two++)
    {
        funny <<= 1;
    }
    if (funny > 65536) funny = 65536;
    bw.write((uint16_t)(table[0] > 0 ? (uint16_t)table[0] : ((~((uint16_t)(-table[0]))) & (funny - 1))), coeffLength);//flip bits if negative, write coefficient value

    //write ac
    int zeroes = 0;

    for (int i = 1; i < dim; i++, zeroes++)
    {
        int coeff = table[dim == 64 ? zigging[i] : zigging2[i]];
        if (coeff != 0)
        {//if the coeff is zero we dont add, because the huffman values have coeff and previous values
            if (zeroes > 15)
            {
                while (zeroes > 15)
                {//if the coeff has more than 15 zeroes we need to add the special code f0 which means 16 zeroes
                    bw.write(huffmanAcValueCodes.retrieveTerm(f0), huffmanAcValueLength.retrieveTerm(f0));
                    zeroes -= 16;
                }
            }
            coeffLength = bitLength(coeff > 0 ? coeff : -coeff);
            if (bitLength(coeffLength) > 4) {
                std::cout << "error coeff length greater than 4: " << coeffLength << " || coeff: " << coeff << " || I: " << i;
                exit(3);
            }
            int huffValue = (zeroes << 4) + coeffLength;

            bw.write(huffmanAcValueCodes.retrieveTerm(huffValue), huffmanAcValueLength.retrieveTerm(huffValue));
            unsigned funny = 1;//used for bitwise and operator to mask out the negative values
            for (int two = 0; two < coeffLength; two++) 
            {
                funny <<= 1;
            }
            if (funny > 65536) funny = 65536;
            if (bw.write((uint16_t)(coeff > 0 ? (uint16_t)coeff : ((~((uint16_t)(-coeff))) & (funny - 1))), coeffLength) == -4)
            {
                std::cout << "Weird Occurence\n";
            }
            zeroes = -1;
        }
        else if (i == 63)
        {
            bw.write(huffmanAcValueCodes.retrieveTerm(x00), huffmanAcValueLength.retrieveTerm(x00));
        }
    }
}
void EOI_M(byteWritter& bw)
{
    if(bw.bitPosition() != 8)//stuff remaining values with 1 if the byte is not complete and than do the end of image marker
    {
        bw.write((uint8_t)((uint8_t)255 >> (8 - bw.bitPosition())), bw.bitPosition());
    }
    bw.write(ff); bw.write(217);//EOI marker, end of image marker
}