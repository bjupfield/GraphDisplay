#ifndef DUMBFUNCSNCLASSES_HPP
#define DUMBFUNCSNCLASSES_HPP
#include <cstdlib>

template <typename Key, typename Term>
class fakeDictionary
{
private:
	Key* keys;
	Term* terms;
	int count;
	int pos;
public:
	fakeDictionary();
	int addPair(Key key, Term term);
	int changeTerm(Key key, Term term);
	int removePair(Key key, Term term);
	Term retrieveTerm(Key key);
	Key* retrieveKeys(Term term);
}
;
template <typename T>
int searchArray(T exist, T* in)
{
	int pos = 0;
	for (T& item : in)
	{
		if (exist == item) {
			return pos;
		}
		++pos;
	}
	return -1;
}

//lol first int will be size of array heheheheheheheheeheheh evil evil
template <typename T>
int* searchArrayAll(T exist, T* in)
{
	int* evilCommited = malloc(sizeof(int));
	evilCommited[0] = 0;
	int pos = 0;
	for (T& item : in)
	{
		if (exist == item) {
			evilCommited[0] += 1;
			evilCommited = realloc(evilCommited, sizeof(int) * evilCommited[0]);
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