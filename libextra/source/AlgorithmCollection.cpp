#include "AlgorithmCollection.h"


namespace PRImA
{

/*
 * Class CAlgorithmCollection
 *
 * An algorithm that can execute multiple child algorithms.
 * Child algorithms can either be added in advance or on demand using a CAlgorithmProvider
 *
 * CC 25/04/2016 - created
 * CC 17/01/2017 - added option to run multi-threaded
 */

/*
 * Constructor
 * 'runParallel' - If set to true, the child algorithms will be run in parallel. Otherwise, they will run in a sequence (same order as they were added).
 */
CAlgorithmCollection::CAlgorithmCollection(bool runParallel /*= false*/)
{
	m_RunParallel = runParallel;
	m_MaxThreads = 3;
	m_ChildAlgorithmProvider = NULL;
}

/*
 * Constructor
 * 'name' - Name of algorithm
 * 'description' - Description of algorithm
 * 'runParallel' - If set to true, the child algorithms will be run in parallel. Otherwise, they will run in a sequence (same order as they were added).
 */
CAlgorithmCollection::CAlgorithmCollection(CUniString name, CUniString description, bool runParallel /*= false*/)
	: CAlgorithm(name, description)
{
	m_RunParallel = runParallel;
	m_MaxThreads = 3;
	m_ChildAlgorithmProvider = NULL;
}

/*
 * Destructor
 */ 
CAlgorithmCollection::~CAlgorithmCollection(void)
{
	for (unsigned int i=0; i<m_Algorithms.size(); i++)
		delete m_Algorithms[i];
	delete m_ChildAlgorithmProvider;
}

/*
 * Runs all child algorithms and updates progress
 */
void CAlgorithmCollection::DoRun()
{
	//Create algorithm provider, if none was supplied
	if (m_ChildAlgorithmProvider == NULL)
	{
		m_ChildAlgorithmProvider = new CListAlgorithmProvider(m_Algorithms);
	}

	//Run parallel or sequential
	if (m_RunParallel)
		RunParallel();
	else
		RunSequential();
}

/*
 * Runs all child algorithms one after the other.
 */
void CAlgorithmCollection::RunSequential()
{
	double progress = 0;
	double progressStep = 100.0 / (double)m_ChildAlgorithmProvider->GetAlgorithmCount();

	//Run all, one after the other
	m_Success = true;
	CAlgorithm * currentAlgorithm = m_ChildAlgorithmProvider->GetNextAlgorithm();
	while (currentAlgorithm != NULL)
	{
		if (currentAlgorithm != NULL)
			currentAlgorithm->Run();

		if (!currentAlgorithm->IsSuccess()) //One fails -> Collection fails
			m_Success = false;

		m_ChildAlgorithmProvider->OnAlgorithmFinished(currentAlgorithm);

		progress += progressStep;
		SetProgress((int)progress);

		currentAlgorithm = m_ChildAlgorithmProvider->GetNextAlgorithm();
	}
}

/*
 * Runs all child algorithms, up to 'maxThreads' in parallel.
 */
void CAlgorithmCollection::RunParallel()
{
	double progress = 0;
	double progressStep = 100.0 / (double)m_ChildAlgorithmProvider->GetAlgorithmCount();

	do
	{
		if (CheckIfChildAlgorithmFinished())
		{
			progress += progressStep;
			SetProgress((int)progress);
		}

		//Is there a free slot?
		if (m_RunningAlgorithms.size() < m_MaxThreads)
		{
			//Start next algorithm
			CAlgorithm * nextAlgorithm = m_ChildAlgorithmProvider->GetNextAlgorithm();

			if (nextAlgorithm != NULL)
			{
				m_RunningAlgorithms.push_back(nextAlgorithm);
				nextAlgorithm->RunAsync();
			}
		}

		//Wait a little bit
		Sleep(100);

	} while (!m_RunningAlgorithms.empty()); //Loop while at least one child algorithm is still running?
}

/*
 * Checks if a child algorithm has finished and, if yes, removes it from the list of running algorithms.
 * Returns true, if an algorithm was removed from the list of running algorithms.
 */
bool CAlgorithmCollection::CheckIfChildAlgorithmFinished()
{
	for (unsigned int i = 0; i < m_RunningAlgorithms.size(); i++)
	{
		CAlgorithm * alg = m_RunningAlgorithms[i];

		if (alg != NULL && !alg->IsRunning())
		{
			//Remove from list
			m_RunningAlgorithms.erase(m_RunningAlgorithms.begin() + i);

			//Successful?
			if (!alg->IsSuccess()) //One fails -> Collection fails
				m_Success = false;

			m_ChildAlgorithmProvider->OnAlgorithmFinished(alg);

			return true;
		}
	}
	return false;
}

/*
 * Adds a child algorithm that is to be executed later
 */
void CAlgorithmCollection::AddChildAlgorithm(CAlgorithm * algorithm)
{
	m_Algorithms.push_back(algorithm);
}

/*
 * Adds an algorithm provider that creates new child algorithms on demand (until the provider returns NULL).
 * This is an alternative to adding all child algorithms in advance before executing this algorithm collection.
 */
void CAlgorithmCollection::SetChildAlgorithmProvider(CAlgorithmProvider * algorithmProvider)
{
	m_ChildAlgorithmProvider = algorithmProvider;
}

/*
 * Returns the algorithm provider that creates new child algorithms on demand (until the provider returns NULL).
 * This is an alternative to adding all child algorithms in advance before executing this algorithm collection.
 */
CAlgorithmProvider * CAlgorithmCollection::GetChildAlgorithmProvider()
{
	return m_ChildAlgorithmProvider;
}

/*
 * Returns the number of child algorithms
 */
int CAlgorithmCollection::GetSize()
{
	return (int)m_Algorithms.size();
}

/*
 * Returns the child algorithm with the given index
 */
CAlgorithm * CAlgorithmCollection::GetChildAlgorithm(int index)
{
	return m_Algorithms[index];
}

/*
 * Sets the maximum number of threads (used when running parallel).
 */
void CAlgorithmCollection::SetMaxThreads(int maxThreads)
{
	m_MaxThreads = maxThreads;
}


/*
 * Class CListAlgorithmProvider
 *
 * Implementation of an algorithm provider that uses a list of algorithms
 *
 * CC 17/01/2017 - created
 */

/*
 * Constructor
 */
CListAlgorithmProvider::CListAlgorithmProvider(vector<CAlgorithm*> & algorithms)
{
	m_Algorithms = algorithms;
	m_Index = 0;
}

/*
 * Destructor
 */
CListAlgorithmProvider::~CListAlgorithmProvider()
{
}

/*
 * Returns the next algorithm in the list. If the end of the list is reached, NULL is returned.
 */
CAlgorithm * CListAlgorithmProvider::GetNextAlgorithm()
{
	if (m_Index < m_Algorithms.size()) 
	{
		CAlgorithm * ret = m_Algorithms[m_Index];
		m_Index++;
		return ret;
	}
	return NULL;
}

/*
 * Returns the size of the algorithm list
 */
int CListAlgorithmProvider::GetAlgorithmCount()
{
	return (int)m_Algorithms.size();
}

void CListAlgorithmProvider::OnAlgorithmFinished(CAlgorithm * algorithm)
{
	//Nothing to do
}


} //end namespace