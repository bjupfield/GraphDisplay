// GraphImageCreator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <iomanip>
#include <random>
#include "RGBtoYCrCb.hpp"
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
    std::cout << "Hello World!\n";

    char fileName[] = "../../ImageTxtFiles/blank.txt";
    char fileName2[] = "../../ImageTxtFiles/Arrow.txt";
    //../../ImageTxtFiles/Arrow.txt
    //../../ImageTxtFiles/blank.txt
    // C:/Users/bjupf/Desktop/GraphDisplayHtml/ImageTxtFiles/blank.txt
    // C:/Users/bjupf/Desktop/GraphDisplayHtml/ImageTxtFiles/Arrow.txt
    //C:/Users/bjupf/JpgEncoder/ImageTxtFiles/blank.txt
    //C:/Users/bjupf/JpgEncoder/ImageTxtFiles/Arrow.txt

    //std::cout << "Enter File Path" << std::endl;

    //char buffer500[] = "";

    //cin >> buffer500;
    //if(buffer500 != "")
    //{
    //    accessImage::recieveOrCreate(buffer500);
    //}
    //else 
    //{
    //    accessImage::recieveOrCreate(fileName);
    //}

    //cout << "This is the 'Label': " << accessImage::retrieveLabel(fileName) << endl;
    //cout << "This is the 'Label': " << accessImage::retrieveLabel(buffer500) << endl;
    //cout << "This is the 'Label': " << accessImage::retrieveLabel(fileName) << endl;
    //cout << "This is the 'Label': " << accessImage::retrieveLabel(fileName) << endl;

    //graphMap myPath = graphMap(3, 2);
    //cout << "Graph Map Label: " << myPath.pathToLabel(buffer500) << endl;
    //cout << "Graph Map Label: " << myPath.pathToLabel(fileName) << endl;
    //myPath.labelMap(new int[6] {1, 1, 0, 1, 1, 0});
    //cout << "Graph MapMap: " << myPath.retrieveMap() << endl;
    //int* dimensions = myPath.retrieveDimensions();
    //cout << "Map Length: " << dimensions[0] << endl;
    //cout << "Map Height: " << dimensions[1] << endl;

    //graphPresets b = accessImage::recieveOrCreate(fileName);


    //seed random graphmap
    random_device rd;
    mt19937 rng(rd());
    uniform_int_distribution<int> distribution(1, 2);
    graphMap myGraph = graphMap(4, 6);
    int label1 = myGraph.pathToLabel(fileName);
    int label2 = myGraph.pathToLabel(fileName2);

    int* map = new int[4 * 6];

    for (int i = 0; i < 4 * 6; i++)
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
    //decoment below to do anything

    myGraph.labelMap(map);

    //send randomseeded graphmap to jpger

    MCUS mine = graphPresetToMcus(myGraph);
    printMcus(mine);
    mcuHuffmanContainer mine2 = mcuHuffmanContainer(mine);



    hInfoStruct hINFO = { mine.retrieveHeight() * 8, mine.retrieveLength() * 8, 2, 2, 0, 1, 1, 64, 64 };

    char fileNAMES[] = "../../Funny.jpg";

    mine2.actualJpg(hINFO, mine2, fileNAMES);

    std::cout << "BlockHeight: " << mine.retrieveHeight() << " || BlockWdith: " << mine.retrieveLength() << " || Size: " << mine2.size() << std::endl;
    for (int k = 0; k < 3; k++)
    {
        std::cout << (k == 0 ? "Y " : k == 1 ? "Cb " : "Cr ") << " Block: \n";
        for (int n = 380; n < 384; n++)
        {
            for (int i = 0; i < 8; i++)
            {
                for (int j = 0; j < 8; j++)
                {
                    std::cout << (int)mine2.mcus[n].yCbCr[k][i * 8 + j] << ", ";
                }
                std::cout << std::endl;
            }
            std::cout << n + 1 << ":" << std::endl;
        }
    }
    std::cout << "Source Mcus\n";
    for(int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            std::cout << (int)mine.mcuList[0].ycbcr.Cr[i * 8 + j] << ", ";
        }
        std::cout << "\n";
    }
    std::cout << "Source Mcus\n";
    


    
    //uncomment
    char file23[] = "../../newestCreatedText.txt";
    char file24[] = "../../Funny.jpg";
    //comment
    //char file23[] = "../../test.txt";
    //char file24[] = "../../test.jpg";

    //byteWritter myBits = byteWritter(file24);

    //std::cout << "Hmm: " << myBits.write((uint16_t)0, 2) << "\n";
    //std::cout << "Hmm: " << myBits.write((uint16_t)0, 2) << "\n";
    //std::cout << "Hmm: " << myBits.write((uint16_t)2, 2) << "\n";
    //std::cout << "Hmm: " << myBits.write((uint16_t)11, 4) << "\n";
    //for (int i = 0; i < 4096; i++)
    //{
    //    myBits.write(255);
    //}

    ifstream image;
    image.open(file24, ios::in | ios::binary | ios::ate);
    if (!image.is_open())
    {
        cout << "Failed To Open!\n" << endl;
    }
    else
    {
        int imageSize = image.tellg();
        image.seekg(0, ios::beg);
        std::cout << imageSize << "\n";
        int rowSize = 2;
        uint8_t* myimageArray = new uint8_t[imageSize];
        uint8_t* bufferArray = new uint8_t[rowSize];


        for (int i = 0; i < imageSize / rowSize; i++)
        {
            image.read(reinterpret_cast<char*>(bufferArray), sizeof(uint8_t) * rowSize);
            copyArray(bufferArray, myimageArray, i * rowSize, rowSize);
            //cout << i << " row: ";
            //for (int j = 0; j < rowSize; j++)
            //{
            //    cout << std::hex << static_cast<int>(myimageArray[i * rowSize + j]) << " ";
            //}
            //cout << std::dec << endl;
        }

        cout << "File Size: " << imageSize << " bytes!!" << endl;
        delete[] bufferArray;

        image.close();
        std::fstream outFile;
        outFile.open(file23, ios::out | ios::trunc);
        if (!outFile.is_open())
        {
            cout << "Not Open" << endl;
        }
        int bytes = 0;
        while (bytes < imageSize)
        {
            //cout << "HEY WHAT THE FUCK: " << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(myimageArray[bytes]) << endl;
            if (static_cast<int>(myimageArray[bytes]) == 255)
            {
                if (static_cast<int>(myimageArray[bytes + 1]) != 0)
                {
                    if (bytes != 0)
                    {
                        outFile << std::dec << endl;
                    }
                    outFile << std::dec << "New Section: ";
                }
            }

            outFile << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(myimageArray[bytes]) << " ";

            ++bytes;
        }
        outFile.close();

    }

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
