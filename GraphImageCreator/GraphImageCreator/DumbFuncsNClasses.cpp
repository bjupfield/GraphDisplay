#include "DumbFuncsNClasses.hpp"
#include <cstdlib>
#include <iostream>

using namespace std;

template <typename Key, typename Term>
fakeDictionary<Key,Term>::fakeDictionary()
{
	this->pos = 0;
	this->count = 0;
	this->keys = (Key*)malloc(sizeof(Key) * 10);
	this->terms = (Term*)malloc(sizeof(Term) * 10);
}
template <typename Key, typename Term>
fakeDictionary<Key, Term>::~fakeDictionary()
{
	if (this->keys != nullptr) free(this->keys);

	if (this->terms != nullptr) free(this->terms);
}
template <typename Key, typename Term>
int fakeDictionary<Key,Term>::addPair(Key key, Term term)
{

	if (this->count == 0) 
	{
		this->count = 1;

		keys[count - 1] = key;
		terms[count - 1] = term;

		return count;
	}
	else if (searchArray(key, this->keys, this->count) == -1)
	{
		this->count += 1;
		this->keys = (Key*)realloc(this->keys, sizeof(Key) * count);
		this->terms = (Term*)realloc(this->terms, sizeof(Term) * count);

		keys[count - 1] = key;
		terms[count - 1] = term;
	}

	return -1;

}
template <typename Key, typename Term>
int fakeDictionary<Key,Term>::changeTerm(Key key, Term term)
{
	int position = searchArray(key, this->keys, this->count);
	if (position != -1)
	{
		terms[position] = term;

		return position;
	}
	return -1;
}
template <typename Key, typename Term>
int fakeDictionary<Key,Term>::removePair(Key key)
{
	int position = searchArray(key, this->keys, this->count);
	if(position != -1)
	{
		this->count -= 1;
		Term* newTerms = (Term*)malloc(sizeof(Term) * count);
		Key* newKeys = (Key*)malloc(sizeof(Key) * count);

		copyArray(this->terms, newTerms, 0, position - 1);
		copyArray(this->terms, newTerms, position + 1, count + 1);

		free(this->terms);
		free(this->keys);

		this->terms = newTerms;
		this->keys = newKeys;

		return position;
	}
	return -1;
}
template <typename Key, typename Term>
int fakeDictionary<Key, Term>::sortByTerm(CompFunc func)
{
	int bologoSort = 0;
	
	Term bufferTerm;	
	Key bufferKey;
	++bologoSort;

	for(bologoSort; bologoSort < this->count; bologoSort++)
	{
		int i = bologoSort >= 2 ? bologoSort / 2 : 1;
		while (i <= bologoSort && !((func(this->terms[count - i], this->terms[0]) == 1) && (i == 1 ? true : func(this->terms[count - i + 1], this->terms[0]) == 0)))
		{
			int adder = (i / 2);
			if (func(this->terms[count - i], this->terms[0]) == 1) 
			{
				if (i != 1)i -= (adder > 0 && i - adder < 1) ? adder : 1;
				else break;
			}
			else i += (adder > 0 && adder + i > bologoSort) ? adder : 1;
		}


		if(i > bologoSort)
		{
			if (i > bologoSort + 1) i = bologoSort + 1;
			bufferTerm = this->terms[count - i];
			this->terms[count - i] = this->terms[0];
			this->terms[0] = bufferTerm;

			bufferKey = this->keys[count - i];
			this->keys[count - i] = this->keys[0];
			this->keys[0] = bufferKey;
		}
		else
		{
			for (i; i <= bologoSort + 1; i++)
			{
				bufferTerm = this->terms[count - i];
				this->terms[count - i] = this->terms[0];
				this->terms[0] = bufferTerm;

				bufferKey = this->keys[count - i];
				this->keys[count - i] = this->keys[0];
				this->keys[0] = bufferKey;
			}
		}


	}
	

	
	return 1;
}
template <typename Key, typename Term>
Term fakeDictionary<Key,Term>::retrieveTerm(Key key)
{
	int position = searchArray(key, this->keys, this->count);
	
	if(position != -1)
	{
		return this->terms[position];
	}
	
	return NULL;
}
template <typename Key, typename Term>
Key* fakeDictionary<Key,Term>::retrieveKeys(Term term)
{
	int* posArray = searchArrayAll(term, this->terms, this->count);

	if(posArray[0] != 0)
	{
		Key* myKeys = (Key*)malloc(sizeof(Key) * posArray[0]);
		
		for(int i = 1; i <= posArray[0]; i++)
		{
			myKeys[i - 1] = this->keys[posArray[i]];
		}

		free(posArray);

		return myKeys;
	}

	free(posArray);
	return NULL;
}
template <typename Key, typename Term>
int fakeDictionary<Key, Term>::retrieveTermCount(Term term)
{
	int* posArray = searchArrayAll(term, this->terms, this->count);

	return posArray[0];
}
template <typename Key, typename Term>
Key* fakeDictionary<Key,Term>::retrieveAllKeys()
{
	return this->keys;
}
template <typename Key, typename Term>
Term* fakeDictionary<Key, Term>::retrieveAllTerms()
{
	return this->terms;
}
template <typename Key, typename Term>
int fakeDictionary<Key,Term>::returnCount()
{
	return this->count;
}
template class fakeDictionary<int, int>;
template class fakeDictionary<int, uint8_t>;
byteWritter::byteWritter(const char* fileName)
{
	this->byte = 0;
	this->currentBit = 8;
	this->outFile.open(fileName, std::ios::out);
	if(!this->outFile.is_open())
	{
		delete this;
	}
}
int byteWritter::write(uint8_t bits, uint8_t bitLength)
{
	if (bitLength <= 0 || bitLength > 255)//return -1 if bitlength is greater than possible bit length or zero
	{
		return -1;
	}
	if ((bits >> bitLength) > 0)//bits recieved are larger than the bitlength
	{
		return -2;
	}

	//find how many bits are 0 at the start, as we are reading from a uint8_t it could give us a value of 1
	//equal to 001 or a value of 1 equal to 01, and the only way to tell is the bitlength
	//so to solve this we bit shift the bits to the right starting from bit length - 1 untill we find a value greater than 1, meaning that there is a non zero value bit stored
	int leadingZeroBits = 0;
	if (bits == 0) leadingZeroBits = bitLength;
	else
	{
		while ((bits >> leadingZeroBits) > 0 && bitLength > leadingZeroBits)
		{
			leadingZeroBits++;
		}
	}
	leadingZeroBits = bitLength - leadingZeroBits;
	//find if bits given will exceed current byte if added to current byte
	int hangingBits = bitLength - currentBit;
	if (hangingBits < 0) hangingBits = 0;
	int nonHangingBits = bitLength - hangingBits;//just using this to make it more clear

	//adjust leading zero to account for hanging bits
	if (leadingZeroBits > nonHangingBits) {
		leadingZeroBits = nonHangingBits;
		if (inScan) std::cout << "HMMMM\n";
	}

	//write bits to byte
	uint8_t bitsToAdd = bits >> hangingBits;//right-bit-shift to remove hangingbits
	this->byte <<= nonHangingBits;//move byte to the left by how many bits were adidng
	//this->byte = this->byte << leadingZeroBits;//first add leading zero bits
	this->byte += bitsToAdd;//than add bits
	this->currentBit -= nonHangingBits;
	if (inScan && ((int)bits == 18 || (int)bits == 3 || true))
	{
		std::cout << "In ByteWritter: || CurrentBit: " << currentBit << "" << " || Non-HangingBits: " << (int)nonHangingBits << " ||LeadingZeroes: " << leadingZeroBits << " || bitsToAdd: " << (int)bitsToAdd << " || BitLength: " << (int)bitLength << " || Bits: " << (int)bits << " || Finalized Byte" << (int)byte << std::endl;
	}

	if(currentBit == 0)
	{
		//turn byte to char
		char buffer = this->byte;
		this->outFile.write(&buffer, 1);
		if (inScan) std::cout << "Sent Value: " << (int)byte << std::endl;
		if(inScan && buffer == char(255))//funny condition needed to prevent ff in scan for jpg, writes 00 after any ff that appears in scan to prevent marker readings
		{
			char buffer = (char)0;
			this->outFile.write(&buffer, 1);
		}
		//reset byte
		this->byte = 0;
		this->currentBit = 8;
		if(hangingBits > 0)
		{
			//bit mask and recursion
			bitsToAdd = 1;
			for (nonHangingBits = 1; nonHangingBits < hangingBits; nonHangingBits++) bitsToAdd = bitsToAdd << 1 | 1;
			return this->write(bits & bitsToAdd, hangingBits);
		}
		return 0;
	}
	else if (currentBit > 0 && currentBit <= 8) 
	{
		return 0;
	}
	else if(currentBit > 8)
	{
		return -3;
	}
	else if (currentBit < 0)
	{
		return -4;
	}
}
int byteWritter::write(uint8_t bits)//this func writes a single byte, no matter the size, it also finishes the current byte
{
	char buffer;
	if (currentBit != 8)
	{
		this->byte << (currentBit);
		buffer = this->byte;
		this->outFile.write(&buffer, 1);
	}
	buffer = bits;
	this->outFile.write(&buffer, 1);
	return 0;
}
bool byteWritter::open()
{
	if (this)
	{
		return true;
	}
	return false;
}
void byteWritter::inScanFlip()
{
	this->inScan = !this->inScan;
}
int byteWritter::bitPosition()
{
	return this->currentBit;
}