#pragma once

namespace PRImA 
{

/*
 * Class CAlgorithmListener
 *
 * Listener interface for algorithm events
 *
 * CC 29/08/2017 - created
 */
class CAlgorithmListener
{
public:
	CAlgorithmListener();
	virtual ~CAlgorithmListener();

	virtual void AlgorithmFinished(void * alg) = 0;

};

} //end namespace