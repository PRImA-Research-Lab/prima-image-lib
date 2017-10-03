#ifndef ARRAY_H
#define ARRAY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef DllExport
 #ifndef PRIMA_DLL_IMPORT
  #define DllExport __declspec( dllexport )
 #else
  #define DllExport __declspec( dllimport )
 #endif
#endif

namespace PRImA 
{

/*
 * Class template CExtraArray
 *
 * Dynamic array with some additional features such as sorting and removing duplicates
 */

template <class T>
class DllExport CExtraArray
{
	// CONSTRUCTION
public:
	CExtraArray();
	~CExtraArray();

	// METHODS
public:
	bool AddItem(T Item);
	bool AddItemBefore(int Index, T Item);
	void DeleteAll();
	void DeleteItem(int Index);
	T & GetItem(int Index);
	int GetItemIndex(T Item);
	int GetItemIndex(T * Item);
	T * GetItemP(int Index);
	int GetNoItems();
	T & operator[](int Index);
	void operator=(CExtraArray<T> & Orig);
	void QSort(int (*fncompare)(const void *, const void *));
	void RemoveDupes();
	void SetIncrement(const int NewIncrement);
	void SetItem(const int Index, const T NewItem);
	bool SetNoItems(const int Number);
	bool SwapItems(int A, int B);
	CExtraArray<T> * Clone();

	// DATA ITEMS
private:
	int m_nItems;
	int m_nAlloc;
	int m_nIncr;
	T * m_pItems;
};

#ifndef PRIMA_DLL_IMPORT

template <class T>
CExtraArray<T>::CExtraArray()
{
//	printf("Start of Array Constructor\n");
	m_nItems = 0;
	m_nAlloc = 0;
	m_nIncr  = 100;
	m_pItems = NULL;
//	printf("End of Array Constructor\n");
}

template <class T>
CExtraArray<T>::~CExtraArray()
{
//	printf("Start of Array DESTRUCTOR\n");
	delete [] m_pItems;
//	printf("End of Array DESTRUCTOR\n");
}

template <class T>
int CExtraArray<T>::GetNoItems()
{
	return m_nItems;
}

template <class T>
T & CExtraArray<T>::GetItem(int Index)
{
	return m_pItems[Index];
}

template <class T>
T * CExtraArray<T>::GetItemP(int Index)
{
	return &(m_pItems[Index]);
}

template <class T>
bool CExtraArray<T>::AddItem(T Item)
{
	if(m_nItems == m_nAlloc)
	{
		T * temp = new T[m_nItems + m_nIncr];
		if(temp == NULL)
		{
			fprintf(stderr,"AddItem: Couldn't Allocate Memory\n");
			return false;
		}

		for(int i = 0; i < m_nItems; i++)
			temp[i] = m_pItems[i];

		delete [] m_pItems;
		m_pItems = temp;
		m_nAlloc += m_nIncr;
	}
	
	m_pItems[m_nItems] = Item;
	m_nItems++;
	return true;
}

template <class T>
bool CExtraArray<T>::AddItemBefore(int Index, T Item)
{
	int i;
	T * temp;
	
	if(m_nItems == m_nAlloc)
	{
		temp = new T[m_nAlloc + m_nIncr];
		m_nAlloc += m_nIncr;
	}
	else
	{
		temp = new T[m_nAlloc];
	}
		
	if(temp == NULL)
	{
		fprintf(stderr,"AddItem: Couldn't Allocate Memory\n");
		return false;
	}

	for(i = 0; i < Index; i++)
		temp[i] = m_pItems[i];
	temp[Index] = Item;
	for(i = Index; i < m_nItems; i++)
		temp[i + 1] = m_pItems[i];

//	memcpy(temp, m_pItems, sizeof(T) * (Index));
//	memcpy(&(temp[Index + 1]), &(m_pItems[Index]), sizeof(T) * (m_nItems - Index));
	delete [] m_pItems;
//	delete m_pItems;
	m_pItems = temp;
	
//	m_pItems[Index] = Item;
	m_nItems++;
	return true;
}

template <class T>
void CExtraArray<T>::DeleteAll()
{
	delete [] m_pItems;
	m_pItems = NULL;
	m_nItems = 0;
	m_nAlloc = 0;
}

template <class T>
void CExtraArray<T>::DeleteItem(int Index)
{
	int i;

	for(i = Index + 1; i < m_nItems; i++)
	{
		memcpy(&(m_pItems[i - 1]), &(m_pItems[i]), sizeof(T));
	}

	m_nItems--;
}

template <class T>
int CExtraArray<T>::GetItemIndex(T Item)
{
	return GetItemIndex(&Item);
}

template <class T>
int CExtraArray<T>::GetItemIndex(T * Item)
{
	for(int i = 0; i < m_nItems; i++)
	{
		if(*Item == m_pItems[i])
			return i;
	}

	return -1;
}

template <class T>
T & CExtraArray<T>::operator[](int Index)
{
	return m_pItems[Index];
}

template <class T>
void CExtraArray<T>::operator=(CExtraArray<T> & Orig)
{
	printf("CExtraArray::operator=\n");
	delete [] m_pItems;
	m_nIncr  = Orig.m_nIncr;

	m_pItems = new T[Orig.m_nItems];

	if(m_pItems != NULL)
	{
		m_nItems = Orig.m_nItems;
		m_nAlloc = Orig.m_nAlloc;

		for(int i = 0; i < m_nItems; i++)
			m_pItems[i] = Orig.m_pItems[i];
	}
	else
	{
		m_nItems = 0;
		m_nAlloc = 0;
	}
}

template <class T>
void CExtraArray<T>::QSort(int (*fncompare)(const void *, const void *))
{
	qsort(m_pItems,m_nItems,sizeof(T),fncompare);
}

// Remove Dupes works correctly only on the sorted array for speed



template <class T>
void CExtraArray<T>::RemoveDupes()
{
	int i,j;
	int LastOrig = 0;
	int NewCount = m_nItems;

	for(i = 1; i < m_nItems; i++)
	{
		if(m_pItems[LastOrig] != m_pItems[i])
		{
			LastOrig++;
			m_pItems[LastOrig] = m_pItems[i];
		}
		else
			NewCount--;
	}

	m_nItems = NewCount;
}

template <class T>
void CExtraArray<T>::SetIncrement(const int NewIncrement)
{
	m_nIncr = NewIncrement;
}

template <class T>
void CExtraArray<T>::SetItem(const int Index, const T NewItem)
{
	m_pItems[Index] = NewItem;
}

template <class T>
bool CExtraArray<T>::SetNoItems(const int Number)
{
	delete [] m_pItems;
	m_pItems = new T[Number];
	if(m_pItems == NULL)
	{
		m_nItems = 0;
		return false;
	}
	m_nItems = Number;
	return true;
}

template <class T>
bool CExtraArray<T>::SwapItems(int A, int B)
{
	T * temp = new T[1];
	if(temp == NULL)
	{
		fprintf(stderr, "Error: SwapItems failed to allocate memory\n");
		return false;
	}
	memcpy(temp, &(m_pItems[A]), sizeof(T));
	memcpy(&(m_pItems[A]), &(m_pItems[B]), sizeof(T));
	memcpy(&(m_pItems[B]), temp, sizeof(T));
	delete [] temp;
	return true;
}

//CC 10.03.2010
template <class T>
CExtraArray<T> * CExtraArray<T>::Clone()
{
	CExtraArray<T> * copy = new CExtraArray<T>();
	for (int i=0; i<this->GetNoItems(); i++)
		copy->AddItem(this->GetItem(i));
	return copy;
}

#endif //PRIMA_DLL_IMPORT

}

#endif // ARRAY_H
