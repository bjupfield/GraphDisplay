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
int fakeDictionary<Key, Term>::sortByTerm(CompFunc func)
{
	int bologoSort = 0;
	
	Term* newTerms = (Term*)malloc(sizeof(Term) * count);
	Key* newKeys = (Key*)malloc(sizeof(Key) * count);

	newTerms[this->count - 1] = this->terms[bologoSort];
	newKeys[this->count - 1] = this->keys[bologoSort];
	++bologoSort;
	
	std::cout << "Count Before Sort: " << this->count << std::endl;

	for(bologoSort; bologoSort < /*this->count*/ 3; bologoSort++)
	{
		int i = bologoSort / 2;
		std::cout << "Bologsort Term: " << this->terms[bologoSort] << "I Term: " << newTerms[count - i - 1] << std::endl;
		std::cout << "OG I: " << i  << " || Bologo: " << bologoSort << std::endl;
		while (i < bologoSort && !(((i == bologoSort || i == 0) ? true : func(newTerms[count - i - 2], this->terms[bologoSort]) == 1) && func(newTerms[count - i - 1], this->terms[bologoSort]) == 0))
		{
			int adder = (i / 2);
			if (func(newTerms[count - i - 1], this->terms[bologoSort]) == 0) i -= i > 0 ? i : 1;
			else i += i > 0 ? i : 1;
			std::cout << "New I: " << i << std::endl;
		}
		std::cout << "Bologo Before Sort: " << bologoSort << std::endl;
		std::cout << "I Before Sort: " << i << std::endl;
		copyArray(newTerms, newTerms, count - bologoSort - 1, bologoSort - i, count - bologoSort);
		newTerms[count - i - 1] = this->terms[bologoSort];
		std::cout << "Keys SORT" << std::endl;
		copyArray(newKeys, newKeys, count - bologoSort - 1, bologoSort - i, count - bologoSort);
		newKeys[count - i - 1] = this->keys[bologoSort];

	}
	
	delete this->keys;
	delete this->terms;
	this->keys = (Key*)newKeys;
	this->terms = (Term*)newTerms;
	
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