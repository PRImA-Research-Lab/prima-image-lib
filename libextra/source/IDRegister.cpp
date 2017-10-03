#include "IDRegister.h"
#include <sstream>

/*
 * Class for registering layout region.
 *
 * CC 20.10.2009 - changed from integer IDs to string IDs
 * CC 30.10.2009 - switched from set<string> to map<string,CLayoutObject> to be able to find the region for a given id
 */

//CC moved the definitons to the header file, because templates don't work otherwise

