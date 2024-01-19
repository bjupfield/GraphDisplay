#include <iostream>
#include "RGBtoYCrCb.hpp"
using namespace std;

	int luminance(int red, int green, int blue)
	{
		return min(255, max(0, (int)floorf(red * .299f + green * .587f + blue * .114f)));
	}
	int chrominanceRed(int red, int green, int blue)
	{
		return min(255, max(0, 128 + (int)floorf(red * .701f - green * .587f - blue * .114f)));
	}
	int chrominanceBlue(int red, int green, int blue)
	{
		return min(255, max(0, 128 + (int)floorf(red * .299f - green * .587f + blue * .886f)));
	}