#ifndef DUMBFUNCSNCLASSES_HPP
#define DUMBFUNCSNCLASSES_HPP

template <typename Key, typename Term>
class fakeDictionary
{
private:
	typedef int (*CompFunc)(Term, Term);
	Key* keys;
	Term* terms;
	int count;
	int pos;
public:
	//fakeDictionary() : keys(nullptr), terms(nullptr), count(0), pos(0) {}
	fakeDictionary();
	int addPair(Key key, Term term);//returns -1 if key already exist
	int changeTerm(Key key, Term term);//returns -1 if key does not exist
	int removePair(Key key);//returns -1 if key does not exist
	int sortByTerm(CompFunc func);
	//funcs return should be 0 if less than 1 if greater than or equal too to sort from greatest to least, opposite to sort from least to greatest
	Term retrieveTerm(Key key);//returns null if key does not exist
	Key* retrieveKeys(Term term);//returns null if term does not exist
	Key* retrieveAllKeys();
	int returnCount();
}
;
template <typename T>
int searchArray(T exist, T* in, int length)
{
	for (int i = 0; i < length; i++)
	{
		if (exist == in[i]) {
			return i;
		}
	}
	return -1;
}

#include <cstdlib>
//lol first int will be size of array heheheheheheheheeheheh evil evil
template <typename T>
int* searchArrayAll(T exist, T* in, int length)
{
	int* evilCommited = (int*)malloc(sizeof(int));
	evilCommited[0] = 0;
	int pos = 0;
	for (int i = 0; i < length; i++)
	{
		if (exist == in[i]) {
			evilCommited[0] += 1;
			evilCommited = (int*)realloc(evilCommited, sizeof(int) * evilCommited[0]);
			evilCommited[evilCommited[0]] = pos;
		}
		++pos;
	}
	return evilCommited;
}
#include <iostream>
template <typename T>
void copyArray(T* source, T* destination, int destinationStartIndex, int num, int sourceStartIndex = 0)
{
	for (int b = destinationStartIndex; b < destinationStartIndex + num; b++)
	{

		destination[b] = source[b - destinationStartIndex + sourceStartIndex];
	}

}
template <typename T>
void newCopyArray(T* source, T* destination, int sourceStartIndex = 0, int destinationStartIndex, int num)
{
	for (int b = 0; b < num; b++)
	{
		destination[b + destinationStartIndex] = &source[b + sourceStartIndex];
	}

}


#endif