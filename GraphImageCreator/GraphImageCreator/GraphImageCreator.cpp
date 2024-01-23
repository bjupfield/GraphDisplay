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
    
    char fileName[] = "C:/Users/bjupf/Desktop/GraphDisplayHtml/ImageTxtFiles/Arrow.txt";
    // C:/Users/bjupf/Desktop/GraphDisplayHtml/ImageTxtFiles/blank.txt
    // C:/Users/bjupf/Desktop/GraphDisplayHtml/ImageTxtFiles/Arrow.txt

    std::cout << "Enter File Path" << std::endl;

    char buffer500[] = "";

    cin >> buffer500;
    if(buffer500 != "")
    {
        accessImage::recieveOrCreate(buffer500);
    }
    else 
    {
        accessImage::recieveOrCreate(fileName);
    }

    cout << "This is the 'Label': " << accessImage::retrieveLabel(fileName) << endl;
    //graphPresets b = accessImage::recieveOrCreate(fileName);

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
