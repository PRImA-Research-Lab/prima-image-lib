#pragma once

#include "algorithm.h"
#include <afxmt.h>

namespace PRImA
{

/*
 * Abstract Class CAlgorithmProvider
 *
 * Factory interface for a limited set of algorithms that are created on demand.
 *
 * CC 17/01/2017 - created
 */
class CAlgorithmProvider
{
public:
	virtual CAlgorithm * GetNextAlgorithm() = 0;

	virtual int GetAlgorithmCount() = 0;

	virtual void OnAlgorithmFinished(CAlgorithm * algorithm) = 0;

};


/*
 * Class CListAlgorithmProvider
 *
 * Implementation of an algorithm provider that uses a list of algorithms
 *
 * CC 17/01/2017 - created
 */
class CListAlgorithmProvider : public CAlgorithmProvider
{
public:
	CListAlgorithmProvider(vector<CAlgorithm*> & algorithms);
	~CListAlgorithmProvider();

	virtual CAlgorithm * GetNextAlgorithm();

	virtual int GetAlgorithmCount();

	virtual void OnAlgorithmFinished(CAlgorithm * algorithm);

private:
	vector<CAlgorithm*> m_Algorithms;
	int m_Index;
};


/*
 * Class CAlgorithmCollection
 *
 * An algorithm that can execute multiple child algorithms.
 * Child algorithms can either be added in advance or on demand using a CAlgorithmProvider
 *
 * CC 25/04/2016 - created
 * CC 17/01/2017 - added option to run multi-threaded
 */
class CAlgorithmCollection : public CAlgorithm
{
public:
	CAlgorithmCollection(bool runParallel = false);
	CAlgorithmCollection(CUniString name, CUniString description, bool runParallel = false);
	~CAlgorithmCollection(void);

	void AddChildAlgorithm(CAlgorithm * algorithm);

	void SetChildAlgorithmProvider(CAlgorithmProvider * algorithmProvider);
	CAlgorithmProvider * GetChildAlgorithmProvider();

	int GetSize();

	CAlgorithm * GetChildAlgorithm(int index);

	void SetMaxThreads(int maxThreads);

private:
	void DoRun();
	void RunSequential();
	void RunParallel();
	bool CheckIfChildAlgorithmFinished();

private:
	vector<CAlgorithm*> m_Algorithms;
	vector<CAlgorithm*> m_RunningAlgorithms;
	CAlgorithmProvider * m_ChildAlgorithmProvider;
	bool				m_RunParallel;
	int					m_MaxThreads;
	CCriticalSection	m_CriticalSect;
};


} //end namespace