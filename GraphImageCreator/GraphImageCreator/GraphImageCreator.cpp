// GraphImageCreator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <iomanip>
#include <random>
#include "GraphPresets.hpp"
#include "JPGer.hpp"
#include "Mcuer.hpp"
#include "DumbFuncsNClasses.hpp"

using namespace std;
int bitength(int bits)//gives the position of the most significant bit
{
    unsigned bitDestroyer = bits;
    unsigned bitLength;
    for (bitLength = 0; bitDestroyer != 0; ++bitLength) bitDestroyer >>= 1;
    return bitLength;
}
void stringToInt(char* entry, int* output);
void errorString(char* adjust, const char* entry);

int main(int stC, char* st[])
{
    if (stC == 0 || st[0][0] == 'C')
    {

        char fileName[] = "../../ImageTxtFiles/blank.txt";
        char fileName2[] = "../../ImageTxtFiles/Arrow.txt";


        //seed random graphmap
        random_device rd;
        mt19937 rng(rd());
        uniform_int_distribution<int> distribution(1, 2);
        graphMap myGraph = graphMap(12, 18);
        int label1 = myGraph.pathToLabel(fileName);
        int label2 = myGraph.pathToLabel(fileName2);

        int* map = new int[12 * 18];

        for (int i = 0; i < 12 * 18; i++)
        {
            if (distribution(rng) == 1)
            {
                map[i] = label1;
            }
            else
            {
                map[i] = label2;
            }
        }

        myGraph.labelMap(map);

        //send randomseeded graphmap to jpger

        MCUS mine = graphPresetToMcus(myGraph);
        mcuHuffmanContainer mine2 = mcuHuffmanContainer(mine);



        hInfoStruct hINFO = { mine.retrieveHeight() * 8, mine.retrieveLength() * 8, 2, 2, 0, 1, 1, 64, 64 };

        char fileNAMES[] = "../../Funny.jpg";

        mine2.actualJpg(hINFO, mine2, fileNAMES);

        char file23[] = "../../newestCreatedText.txt";
        char file24[] = "../../Funny.jpg";

        exit(2);
    }
    else
    {
        bool formatingError = false;
        char formatingErrorString[150] = {' '};
        char* curr = st[0];
        if (stC == 1)
        {
            formatingError = true;
            errorString(formatingErrorString, "Error No Entry");
        }
        else 
        {
            curr = st[1];
            int height = 0, length = 0;
            stringToInt(st[1], &height);
            
            if (height != -1)//graphmap being used
            {
                if (stC > 6)//should be 6 change later
                {
                    stringToInt(st[2], &length);
                    int fileCount = 0;
                    stringToInt(st[3], &fileCount);
                    
                    if (fileCount != -1 && length != -1)
                    {
                        graphMap graph = graphMap(length, height);
                        int* labels = new int[fileCount];
                        int i = 0;
                        for (i; i < fileCount; i++)
                        {
                            labels[i] = graph.pathToLabel(st[i + 4]);
                            std::cout << "Label is: " << labels[i] << "\n";
                            if (labels[i] == -1)
                            {
                                break;
                            }
                        }
                        if (i != fileCount)//file not found
                        {
                            formatingError = true;
                            errorString(formatingErrorString, "File Not Found");
                            if (formatingError) std::cout << "Enter Arguments: Look at Documentation for Format\n" << "Error: " << formatingErrorString << "\nFile: " << st[4 + i];
                            exit(2);
                        }
                        else//all files found and labels and maps created now just to seeed the map...
                        {
                            std::cout << "Here";

                            int* map = new int[height * length];
                            curr = st[fileCount + 4];
                            
                            i = 0;
                            int j = 0;
                            for (i; i < height * length; ++i)
                            {
                                int label = 0;
                                while (curr[j] != '\n' && curr[j] != ',')
                                {
                                    int convert = (int)curr[j] - 48;
                                    if (convert > 9 || convert < 0)
                                    {
                                        label = -1;
                                        break;
                                    }
                                    label *= 10;
                                    label += convert;
                                    j++;
                                }
                                if (label != -1 && label < fileCount)//checks if label exist or if the label is not a number
                                {
                                    map[i] = label;
                                }
                                else//label failed
                                {
                                    i = -1;
                                    break; 
                                }
                                ++j;
                            }
                            if (i == -1)
                            {
                                formatingError = true;
                                errorString(formatingErrorString, "Label Map Exceeds Bounds");
                            }
                            else
                            {//succes?
                                
                                graph.labelMap(map);

                                MCUS toMCUS = graphPresetToMcus(graph);
                                mcuHuffmanContainer toHuffman = mcuHuffmanContainer(toMCUS);

                                hInfoStruct hINFO = { toMCUS.retrieveHeight() * 8, toMCUS.retrieveLength() * 8, 2, 2, 0, 1, 1, 64, 64 };

                                if (stC == fileCount + 6)
                                {
                                    toHuffman.actualJpg(hINFO, toHuffman, st[fileCount + 5]);
                                }
                                else
                                {//either output file name is not given or there are strings after it is given, both bad
                                    errorString(formatingErrorString, "Incorrect File Assigment");
                                    formatingError = true;
                                }
                            }
                            
                        }

                    }
                    else 
                    {
                        formatingError = true;
                        errorString(formatingErrorString, "Length or FileCount not Specified");
                    }
                }
                else
                {
                    formatingError = true;
                    errorString(formatingErrorString, "Incorrect String Count");
                }
            }
            else//this we will use to encode bitmaps and other file types
            {
            
            }
            
        }
        if (formatingError) std::cout << "Enter Arguments: Look at Documentation for Format\n" << "Error: " << formatingErrorString;
    }
}

void stringToInt(char* entry, int* output)//-1 if failed or not number, only does unsigned ints
{
    int i = 0;
    while (entry[i] != '\n')
    {
        int convert = (int)entry[i] - 48;
        if (convert < 0 || convert > 9)
        {
            if(convert != -48) i = -1;
            break;
        }
        *output *= 10;
        *output += convert;
        ++i;
    }
    if (i == -1) *output = -1;
}
void errorString(char* adjust, const char* entry)
{
    for (int i = 0; i < 150; i++)
    {
        adjust[i] = ' ';
    }
    int i = 0;
    while(entry[i] != '\n')
    {
        adjust[i] = entry[i];
        ++i;
    }
}
