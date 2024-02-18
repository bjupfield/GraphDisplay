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
int fakeDictionary<Key,Term>::addPair(Key key, Term term)
{

	if (this->count == 0 || searchArray(key, this->keys, this->count) == -1) 
	{
		this->count += 1;
		std::cout << "Counters" << this->count << std::endl;
		this->keys = (Key*)realloc(this->keys, sizeof(Key) * count);
		this->terms = (Term*)realloc(this->terms, sizeof(Term) * count);

		keys[count - 1] = key;
		terms[count - 1] = term;

		return count;
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
int fakeDictionary<Key,Term>::removePair(Key key, Term term)
{
	int position = searchArray(key, this->keys, this->count);
	if(position != -1)
	{
		this->count -= 1;
		Term* newTerms = (Term*)malloc(sizeof(Term) * count);
		Key* newKeys = (Key*)malloc(sizeof(Key) * count);

		copyArray(this->terms, newTerms, 0, position - 1);
		copyArray(this->terms, newTerms, position + 1, count + 1);

		delete this->terms;
		delete this->keys;

		this->terms = newTerms;
		this->keys = newKeys;

		return position;
	}
	return -1;
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

		delete posArray;

		return myKeys;
	}

	delete posArray;
	return NULL;
}
template <typename Key, typename Term>
Key* fakeDictionary<Key, Term>::retrieveAllKeys()
{
	return this->keys;
}
template <typename Key, typename Term>
int fakeDictionary<Key, Term>::returnCount()
{
	return this->count;
}
template class fakeDictionary<int, int>;