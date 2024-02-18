#ifndef DUMBFUNCSNCLASSES_HPP
#define DUMBFUNCSNCLASSES_HPP

template <typename Key, typename Term>
class fakeDictionary
{
private:
	Key* keys;
	Term* terms;
	int count;
	int pos;
public:
	//fakeDictionary() : keys(nullptr), terms(nullptr), count(0), pos(0) {}
	fakeDictionary();
	int addPair(Key key, Term term);
	int changeTerm(Key key, Term term);
	int removePair(Key key, Term term);
	Term retrieveTerm(Key key);
	Key* retrieveKeys(Term term);
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

template <typename T>
void copyArray(T* source, T* destination, int startIndex, int num)
{
	for (int b = startIndex; b < startIndex + num; b++)
	{
		destination[b] = source[b - startIndex];
	}
}



#endif