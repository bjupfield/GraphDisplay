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


int main()
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
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
