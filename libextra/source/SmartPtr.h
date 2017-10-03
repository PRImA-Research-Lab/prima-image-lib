#pragma once

#include <typeinfo.h>

//TODO Use this as compiler switch
//#define DONT_SYNCHRONIZE_SMART_POINTER

#ifndef DllExport
 #ifndef PRIMA_DLL_IMPORT
  #define DllExport __declspec( dllexport )
 #else
  #define DllExport __declspec( dllimport )
 #endif
#endif

#ifndef DONT_SYNCHRONIZE_SMART_POINTER
	#include <afxmt.h>
#endif

namespace PRImA 
{

/*
 * Template Class SmartPtr
 *
 * The SmartPtr template class is a replacement for conventional c pointers.
 * It uses reference counting to release pointed objects automatically if
 * they are not referenced anymore.
 * 
 *
 * Thread safety:
 *    The smart pointer class is by default thread safe.
 *    For single threaded applications the precompiler
 *    definition DONT_SYNCHRONIZE_SMART_POINTER may be
 *    used to disable synchronization.
 *
 * Performance:
 *    Without synchronization (not thread safe):  About double the time of normal pointers.
 *    Synchronized (thread safe):                 About five times slower than normal pointers.
 *
 * Christian Clausner - 2010
 * The SmartPtr class is based on the 'Smart Pointer' articles by Scott Meyers.
 * See www.aristeia.com/Papers/C++ReportColumns/
 */

template<class T>							// Template for smart
class DllExport SmartPtr {							// pointer objects

//Methods:
public:
	SmartPtr(T * realPointer = NULL);		// Create a smart ptr to an
											// obj given a dumb ptr to
											// it; uninitialized ptrs
											// default to 0 (null)

	SmartPtr(const SmartPtr& otherPointer); // Copy a smart ptr

	~SmartPtr();							// Destroy a smart ptr

	// Make an assignment to a smart ptr
	SmartPtr& operator=(const SmartPtr& otherPointer);

	void Assign(T * realPointer);

	T* operator->() const;	// Dereference a smart ptr
							// to get at a member of
							// what it points to

	T& operator*() const;	// Defererence a smart ptr

	bool operator==(const SmartPtr& otherPointer) const;	// Checks if pointers are equal
	bool operator!=(const SmartPtr& otherPointer) const;	// Checks if pointers are not equal

	bool IsNull();											// Test if pointee is NULL

	bool IsTypeOf(const type_info & typeInfo);				// Checks if the pointee is of the specified type

	template<class newType>									// Template function for
		operator SmartPtr<newType>()						// implicit conversion ops.
		{
			//Lock
			#ifndef DONT_SYNCHRONIZE_SMART_POINTER
				CSingleLock singleLock(m_CriticalSect);
				singleLock.Lock();
			#endif

			//New pointer
			SmartPtr<newType> ret(m_Pointee);

			ret._UnsafeSetReferenceCount(m_ReferenceCount);
			#ifndef DONT_SYNCHRONIZE_SMART_POINTER
				ret._UnsafeSetCriticalSection(m_CriticalSect);		//For synchronization
			#endif

			//Increase reference count
			(*m_ReferenceCount)++;

			//Unlock
			#ifndef DONT_SYNCHRONIZE_SMART_POINTER
				singleLock.Unlock();
			#endif

			return ret;
		}

	template<class newType>										// Template function for
		SmartPtr<T> Cast(SmartPtr<newType> other)				// explicit conversion ops.
		{
			//This function does almost the same as the assignment operator.
			//It just adds an explicit cast to the pointee assignment.

			//Lock
			#ifndef DONT_SYNCHRONIZE_SMART_POINTER
				CSingleLock singleLock(m_CriticalSect);
				singleLock.Lock();
			#endif

			//Decrease reference count
			(*m_ReferenceCount)--;

			// Remove old pointee
			if ((*m_ReferenceCount) == 0) //No more references
			{
				//Delete the original pointer
				delete m_Pointee;
				m_Pointee = NULL;

				delete m_ReferenceCount;
				m_ReferenceCount = 0;

				#ifndef DONT_SYNCHRONIZE_SMART_POINTER
					//Unlock
					singleLock.Unlock();
					delete m_CriticalSect;
					m_CriticalSect = NULL;
				#endif
			}
			else
			{
				//Unlock
				#ifndef DONT_SYNCHRONIZE_SMART_POINTER
					singleLock.Unlock();
				#endif
			}

			//Assign new pointee
			// Lock
			#ifndef DONT_SYNCHRONIZE_SMART_POINTER
				m_CriticalSect = other._UnsafeGetCriticalSection();
				CSingleLock singleLock2(m_CriticalSect);
				singleLock2.Lock();
			#endif
			m_Pointee = (T*)other._UnsafeGetConventionalPointer();				//This is the actual cast
			m_ReferenceCount = other._UnsafeGetReferenceCount();

			(*m_ReferenceCount)++;

			//Unlock
			#ifndef DONT_SYNCHRONIZE_SMART_POINTER
				singleLock2.Unlock();
			#endif

			return *this;
		}

	template<class newType>										// Template function for
		SmartPtr<T> DynamicCast(SmartPtr<newType> other)		// explicit conversion ops.
		{
			//This function does almost the same as the assignment operator.
			//It just adds an explicit dynamic cast to the pointee assignment.

			//Lock
			#ifndef DONT_SYNCHRONIZE_SMART_POINTER
				CSingleLock singleLock(m_CriticalSect);
				singleLock.Lock();
			#endif

			//Decrease reference count
			(*m_ReferenceCount)--;

			// Remove old pointee
			if ((*m_ReferenceCount) == 0) //No more references
			{
				//Delete the original pointer
				delete m_Pointee;
				m_Pointee = NULL;

				delete m_ReferenceCount;
				m_ReferenceCount = 0;

				#ifndef DONT_SYNCHRONIZE_SMART_POINTER
					//Unlock
					singleLock.Unlock();
					delete m_CriticalSect;
					m_CriticalSect = NULL;
				#endif
			}
			else
			{
				//Unlock
				#ifndef DONT_SYNCHRONIZE_SMART_POINTER
					singleLock.Unlock();
				#endif
			}

			//Assign new pointee
			// Lock
			#ifndef DONT_SYNCHRONIZE_SMART_POINTER
				m_CriticalSect = other._UnsafeGetCriticalSection();
				CSingleLock singleLock2(m_CriticalSect);
				singleLock2.Lock();
			#endif

			m_Pointee = dynamic_cast<T*>(other._UnsafeGetConventionalPointer());				//This is the actual cast

			//if (m_Pointee != NULL)
			//{
				m_ReferenceCount = other._UnsafeGetReferenceCount();
				(*m_ReferenceCount)++;
			//}
			//else
			//{
			//	m_ReferenceCount = new int;
			//	(*m_ReferenceCount) = 1;
			//}

			//Unlock
			#ifndef DONT_SYNCHRONIZE_SMART_POINTER
				singleLock2.Unlock();
			#endif

			return *this;
		}

	//These methods are needed for the explicit type cast and should not be used from the outside.
	T		*	_UnsafeGetConventionalPointer();
	int		*	_UnsafeGetReferenceCount();
	void		_UnsafeSetReferenceCount(int * count);
	void		_UnsafeIncreaseReferenceCount();
	#ifndef DONT_SYNCHRONIZE_SMART_POINTER
		CCriticalSection *	_UnsafeGetCriticalSection();		
		void _UnsafeSetCriticalSection(CCriticalSection * sect);		
	#endif

// Data members
private:
	T * m_Pointee;				//What the smart ptr points to

	int * m_ReferenceCount;		//How many pointer instances are referencing the pointee

	#ifndef DONT_SYNCHRONIZE_SMART_POINTER
		CCriticalSection *	m_CriticalSect;		//For synchronization
	#endif

};	


//Definition:

#ifndef PRIMA_DLL_IMPORT

/*
 * Constructor
 *
 * 'realPointer' - Conventional pointer to the object the smart pointer should point to.
 */
template<class T>
SmartPtr<T>::SmartPtr(T * realPointer /* = NULL */)
{
	m_Pointee = realPointer;

	//Initialize the reference counter with 1
	m_ReferenceCount = new int;
	(*m_ReferenceCount) = 1;

	#ifndef DONT_SYNCHRONIZE_SMART_POINTER
		m_CriticalSect = new CCriticalSection();
	#endif
}

/*
 * Copy Constructor
 */
template<class T>
SmartPtr<T>::SmartPtr(const SmartPtr<T>& otherPointer)
{
	//Lock
	#ifndef DONT_SYNCHRONIZE_SMART_POINTER
		CSingleLock singleLock(otherPointer.m_CriticalSect);
		m_CriticalSect = otherPointer.m_CriticalSect;

		singleLock.Lock();
	#endif

	m_Pointee = otherPointer.m_Pointee; 
	m_ReferenceCount = otherPointer.m_ReferenceCount;

	//Increase reference count
	(*m_ReferenceCount)++;

	//Unlock
	#ifndef DONT_SYNCHRONIZE_SMART_POINTER
		singleLock.Unlock();
	#endif
}

/*
 * Destructor
 *
 * Decreases the reference count and deletes the pointed object,
 * if the reference count reaches zero.
 */
template<class T>
SmartPtr<T>::~SmartPtr()
{
	//Lock
	#ifndef DONT_SYNCHRONIZE_SMART_POINTER
		CSingleLock singleLock(m_CriticalSect);
		singleLock.Lock();
	#endif

	//Decrease reference count
	(*m_ReferenceCount)--;

	if ((*m_ReferenceCount) == 0) //No more references
	{
		//Delete the original pointer
		delete m_Pointee;
		m_Pointee = NULL;

		delete m_ReferenceCount;
		m_ReferenceCount = 0;

		#ifndef DONT_SYNCHRONIZE_SMART_POINTER
			//Unlock
			singleLock.Unlock();
			delete m_CriticalSect;
			m_CriticalSect = NULL;
		#endif
	}
	else
	{
		//Unlock
		#ifndef DONT_SYNCHRONIZE_SMART_POINTER
			singleLock.Unlock();
		#endif
	}
}

/*
 * Make an assignment to a smart ptr
 */
template<class T>
SmartPtr<T>& SmartPtr<T>::operator=(const SmartPtr<T>& otherPointer)
{
	//Lock
	#ifndef DONT_SYNCHRONIZE_SMART_POINTER
		CSingleLock singleLock(m_CriticalSect);
		singleLock.Lock();
	#endif

	//Decrease reference count
	(*m_ReferenceCount)--;

	// Remove old pointee
	if ((*m_ReferenceCount) == 0) //No more references
	{
		//Delete the original pointer
		delete m_Pointee;
		m_Pointee = NULL;

		delete m_ReferenceCount;
		m_ReferenceCount = 0;

		#ifndef DONT_SYNCHRONIZE_SMART_POINTER
			//Unlock
			singleLock.Unlock();
			delete m_CriticalSect;
			m_CriticalSect = NULL;
		#endif
	}
	else
	{
		//Unlock
		#ifndef DONT_SYNCHRONIZE_SMART_POINTER
			singleLock.Unlock();
		#endif
	}

	//Assign new pointee
	// Lock
	#ifndef DONT_SYNCHRONIZE_SMART_POINTER
		m_CriticalSect = otherPointer.m_CriticalSect;
		CSingleLock singleLock2(m_CriticalSect);
		singleLock2.Lock();
	#endif
	m_Pointee = otherPointer.m_Pointee;
	m_ReferenceCount = otherPointer.m_ReferenceCount;

	(*m_ReferenceCount)++;

	//Unlock
	#ifndef DONT_SYNCHRONIZE_SMART_POINTER
		singleLock2.Unlock();
	#endif

	return *this;
}

/*
 * Explicitly assign a conventional pointer. This smart pointer takes the responsibility 
 * to release the pointed object.
 * This method is a little bit faster than the implicit assign with the = operator.
 */
template<class T>
void SmartPtr<T>::Assign(T * realPointer)
{
	//Lock
	#ifndef DONT_SYNCHRONIZE_SMART_POINTER
		CSingleLock singleLock(m_CriticalSect);
		singleLock.Lock();
	#endif

	//Decrease reference count
	(*m_ReferenceCount)--;

	// Remove old pointee
	if ((*m_ReferenceCount) == 0) //No more references
	{
		//Delete the original pointer
		delete m_Pointee;
		m_Pointee = NULL;
	}

	//Assign new pointee
	m_Pointee = realPointer;

	(*m_ReferenceCount) = 1;

	//Unlock
	#ifndef DONT_SYNCHRONIZE_SMART_POINTER
		singleLock.Unlock();
	#endif
}

/*
 * Equals
 */
template<class T>
bool SmartPtr<T>::operator==(const SmartPtr<T>& otherPointer) const
{
	return otherPointer.m_Pointee == this->m_Pointee;
}

/*
 * Not Equals
 */
template<class T>
bool SmartPtr<T>::operator!=(const SmartPtr<T>& otherPointer) const
{
	return otherPointer.m_Pointee != this->m_Pointee;
}

/*
 * Dereference a smart ptr to get at a member of what it points to
 */
template<class T>
T* SmartPtr<T>::operator->() const
{
	return m_Pointee;
}

/*
 * Defererence a smart ptr
 */
template<class T>
T& SmartPtr<T>::operator*() const
{
	return *m_Pointee;
}

/*
 * Test if pointee is NULL.
 */
template<class T>
bool SmartPtr<T>::IsNull() 
{
	return m_Pointee == NULL;
}

/*
 * Checks if the pointee is of the specified type
 *
 * Usage: bool result = smartPointerObject.IsTypeOf(typeid(CAnyClass));
 */
template<class T>
bool SmartPtr<T>::IsTypeOf(const type_info & typeInfo)
{
	if (m_Pointee == NULL)
		return false;
	return typeid(*m_Pointee) == typeInfo;
}

/*
 * Should not be used from the outside.
 * We needed this for the explixit type cast.
 */
template<class T>
T * SmartPtr<T>::_UnsafeGetConventionalPointer()
{
	return m_Pointee;
}

/*
 * Should not be used from the outside.
 * We needed this for the explixit type cast.
 */
template<class T>
int	* SmartPtr<T>::_UnsafeGetReferenceCount()
{
	return m_ReferenceCount;
}

/*
 * Should not be used from the outside.
 * We needed this for the implicit type conversion.
 */
template<class T>
void SmartPtr<T>::_UnsafeSetReferenceCount(int * count)
{
	delete m_ReferenceCount;
	m_ReferenceCount = count;
}

/*
 * Use only if absolutley necessary.
 */
template<class T>
void SmartPtr<T>::_UnsafeIncreaseReferenceCount()
{
	(*m_ReferenceCount)++;
}

/*
 * Should not be used from the outside.
 * We needed this for the explixit type cast.
 */
#ifndef DONT_SYNCHRONIZE_SMART_POINTER
	template<class T>
	CCriticalSection * SmartPtr<T>::_UnsafeGetCriticalSection()
	{
		return m_CriticalSect;
	}
#endif

/*
 * Should not be used from the outside.
 * We needed this for the implicit type conversion.
 */
#ifndef DONT_SYNCHRONIZE_SMART_POINTER
	template<class T>
	void SmartPtr<T>::_UnsafeSetCriticalSection(CCriticalSection * sect)
	{
		delete m_CriticalSect;
		m_CriticalSect = sect;
	}
#endif


/*
 * Class SmartContainer
 *
 * Simple container template that holds exactly one object.
 */

template<class T>			
class SmartContainer {		

public:
	SmartContainer(T obj);
	T GetObject();

private:
	T m_Object;
};

/*
 * Constructor
 */
template<class T>
SmartContainer<T>::SmartContainer(T obj)
{
	m_Object = obj;
}

template<class T>
T SmartContainer<T>::GetObject()
{
	return m_Object;
}

#endif

}