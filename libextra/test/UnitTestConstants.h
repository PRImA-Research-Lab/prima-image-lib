#pragma once

namespace libextra
{

/*
 * Class CUnitTestConstants
 *
 * Constants for unit tests (e.g. file paths)
 *
 * CC 09/05/14
 */
class CUnitTestConstants
{
public:
	static const wchar_t * TESTING_FOLDER;
	CUnitTestConstants(void);
	~CUnitTestConstants(void);
};

}