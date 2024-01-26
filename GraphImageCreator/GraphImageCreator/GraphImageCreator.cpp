// GraphImageCreator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <iomanip>
#include "RGBtoYCrCb.hpp"
#include "GraphPresets.hpp"

using namespace std;
template <typename T>
void copyBufferArray(T* source, T* destination, int startIndex, int num);

int main()
{
    std::cout << "Hello World!\n";
    //ifstream image;
    //image.open("C:/Users/bjupf/Desktop/GraphDisplayHtml/s.jpg", ios::in | ios::binary | ios::ate);
    //if(!image.is_open())
    //{
    //    cout << "Failed To Open!\n" << endl;
    //    exit(0);
    //}

    //int imageSize = image.tellg();
    //image.seekg(0, ios::beg);
    //
    //int rowSize = 40;
    //uint8_t* myimageArray = new uint8_t[imageSize];
    //uint8_t* bufferArray = new uint8_t[rowSize];


    //for (int i = 0; i < imageSize / rowSize; i++) 
    //{
    //    image.read(reinterpret_cast<char*>(bufferArray), sizeof(uint8_t) * rowSize);
    //    copyBufferArray(bufferArray, myimageArray, i * rowSize, rowSize);
    //    cout << i << " row: ";
    //    for (int j = 0; j < rowSize; j++) 
    //    {
    //        cout << std::hex << static_cast<int>(myimageArray[i * rowSize + j]) << " ";
    //    }
    //    cout << std::dec << endl;
    //}

    //cout << "File Size: " << imageSize << " bytes!!" << endl;
    //delete[] bufferArray;
    //
    //image.close();
    //std::fstream outFile;
    //outFile.open("C:/Users/bjupf/Desktop/GraphDisplayHtml/newCreatedText.txt", ios::out | ios::trunc);
    //if (!outFile.is_open())
    //{
    //    cout << "Not Open" << endl;
    //}
    //int bytes = 0;
    //while(bytes < imageSize)
    //{
    //    if (static_cast<int>(myimageArray[bytes]) == 255) 
    //    {
    //        if(static_cast<int>(myimageArray[bytes + 1]) != 0)
    //        {
    //            if (bytes != 0) 
    //            {
    //                outFile << std::dec << endl;
    //            }
    //            outFile << std::dec << "New Section: ";
    //        }
    //    }

    //    outFile << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(myimageArray[bytes]) << " ";

    //    ++bytes;
    //}
    //outFile.close();
    
    char fileName[] = "C:/Users/bjupf/JpgEncoder/ImageTxtFiles/Arrow.txt";
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
    


    ofstream myJpg;
    myJpg.open("C:/Users/bjupf/JpgEncoder/mybad.jpg");

    if(myJpg.is_open())
    {
        //header and appo file write, standard information does not change
        myJpg << (uint8_t)255;
        myJpg << (uint8_t)216;
        myJpg << (uint8_t)255;
        myJpg << (uint8_t)224;
        myJpg << (uint8_t)0;
        myJpg << (uint8_t)16;
        myJpg << (uint8_t)74;
        myJpg << (uint8_t)70;
        myJpg << (uint8_t)73;
        myJpg << (uint8_t)70;
        myJpg << (uint8_t)0;
        const uint8_t* affoHeader = new uint8_t[9]{ (uint8_t)255, (uint8_t)224, (uint8_t)0, (uint8_t)16, (uint8_t)74, (uint8_t)70, (uint8_t)73, (uint8_t)70, (uint8_t)0};
        myJpg << affoHeader;
        myJpg.close();
    }


   // delete[] myimageArray;
    exit(2);
}
template <typename T>
void copyBufferArray(T* source, T* destination, int startIndex, int num) 
{
    for (int b = startIndex; b < startIndex + num; b++) 
    {
        destination[b] = source[b - startIndex];
    }
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
