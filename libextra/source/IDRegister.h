#pragma once
#ifndef IDREGISTER_H
#define IDREGISTER_H

#include <limits.h>
#include <map>
#include "ExtraString.h"

#ifndef DllExport
 #ifndef PRIMA_DLL_IMPORT
  #define DllExport __declspec( dllexport )
 #else
  #define DllExport __declspec( dllimport )
 #endif
#endif

using namespace std;

namespace PRImA 
{

/*
 * Class for registering layout regions.
 *
 * CC 20.10.2009 - changed from integer IDs to string IDs
 * CC 30.10.2009 - switched from set<string> to map<string,CLayoutObject> to be able to find the region for a given id
 * CC 19.02.2010 - converted to tempate
 */

template <class T>
class DllExport CIdRegister
{
	// CONSTRUCTION
public:
	CIdRegister();
	~CIdRegister();

	// METHODS
public:
	CUniString	CreateId(T region, wchar_t prefix = NULL);
	CUniString	RegisterId(CUniString id,  T region);
	void		UnregisterId(CUniString id);
	T			GetRegion(CUniString id);
	inline bool	HasCollisions() { return !m_Collisions.IsEmpty(); };
	CUniString	GetCollisions() { return m_Collisions; }
	bool		HasId(CUniString & id);
	bool		IsIdValid(CUniString & id);
	CUniString  GetValidId(wchar_t prefix = NULL);
	CIdRegister<T> * Clone();
	void		Reset();
	
private:
	//CUniString		i2string(int i);

	// DATA ITEMS
private:
	map<CUniString,T> * m_Map;
	int m_Num;
	CUniString m_Collisions;
};

#ifndef PRIMA_DLL_IMPORT

/*
 * Constructor
 */ 
template <class T>
CIdRegister<T>::CIdRegister()
{
	m_Map = new map<CUniString,T>();
	m_Num = 0;
}

/*
 * Destructor
 */
template <class T>
CIdRegister<T>::~CIdRegister()
{
	while (!m_Map->empty())
	{
		this->UnregisterId((*(m_Map->begin())).first);
	}

	delete m_Map;
}

/*
 * Unregisteres all IDs
 */
template <class T>
void CIdRegister<T>::Reset()
{
	m_Map->clear();
	m_Num = 0;
	m_Collisions = CUniString();
}

/*
 * Creates an unique ID.
 * 'prefix' (optional) - Prefix character to use for the ID (e.g. 'w')
 */
template <class T>
CUniString CIdRegister<T>::CreateId(T region, wchar_t prefix /*= NULL*/)
{
	CUniString id = GetValidId(prefix);
	return this->RegisterId(id, region);
}

/*
 *
 * 'prefix' (optional) - Prefix character to use for the ID (default is 'r')
 */
template <class T>
CUniString CIdRegister<T>::GetValidId(wchar_t prefix /*= NULL*/)
{
	CUniString id;

	//Prefix
	if (prefix == NULL)
		id.Append(L"r");
	else
		id.Append(prefix);

	//Number
	id.Append(m_Num);

	while(m_Map->find(id) != m_Map->end())
	{
		m_Num++;
		id.Clear();
		if (prefix == NULL)
			id.Append(L"r");
		else
			id.Append(prefix);
		id.Append(m_Num);
	} 
	m_Num++;
	return id;
}

/*
 * Tries to register the given ID. If the ID already exists, a new
 * ID is returned.
 */
template <class T>
CUniString CIdRegister<T>::RegisterId(CUniString id, T region)
{
	if (id.IsEmpty()) //Empty strings will not be registered
		return id;

	map<CUniString,T>::iterator it = m_Map->find(id);
	if (it == m_Map->end()) //Not yet registered
	{
		m_Map->insert(std::pair<CUniString,T>(id,region));
		return id;
	}
	//Already registered 
	// Same object?
	if (region != NULL && (*it).second == region)
		return id;

	// Not same object -> create a new ID

	// Get prefix
	wchar_t prefix = 'r';
	if (!id.IsEmpty())
		prefix = id.GetAt(0);

	CUniString newId = this->CreateId(region, prefix);

	//Log collision
	m_Collisions.Append(id);
	m_Collisions.Append(L" changed to ");
	m_Collisions.Append(newId);
	m_Collisions.Append(L"\n");

	return newId;
}

/*
 * Removes the given ID from the register.
 */
template <class T>
void CIdRegister<T>::UnregisterId(CUniString id)
{
	map<CUniString,T>::iterator it;
	it = m_Map->find(id);
	if (it != m_Map->end())
	{
		m_Map->erase(it); //also deletes the string
	}
}

/*
 * Returns the object associated with the given ID (or NULL if not found)
 */
template <class T>
T CIdRegister<T>::GetRegion(CUniString id)
{
	if (!HasId(id))
		return NULL;
	return (*(m_Map->find(id))).second;
}

/*
 * Checks if the given ID has been registered
 */
template <class T>
bool CIdRegister<T>::HasId(CUniString & id)
{
	return !(m_Map->find(id) == m_Map->end());
}

/*
 * Basic check for validity (ID must not start with a digit) 
 */
template <class T>
bool CIdRegister<T>::IsIdValid(CUniString & id)
{
	bool valid = true;

	if (!id.IsEmpty())
	{
		wchar_t * buffer = id.GetBuffer();
		if (buffer[0] >= '0' && buffer[0] <= '9')
			valid = false;
	}

	return valid;
}

/*
 * Creates a deep copy of this ID register.
 * Note: Does not clone the objects associated with the IDs
 */
template <class T>
CIdRegister<T> *  CIdRegister<T>::Clone()
{
	CIdRegister<T> * copy = new CIdRegister<T>();

	map<CUniString,T>::iterator it = m_Map->begin();
	while (it != m_Map->end())
	{
		copy->RegisterId((*it).first, (*it).second);
		it++;
	}

	return copy;
}

#endif // PRIMA_DLL_IMPORT

} //end namespace

#endif // IDREGISTER_H

