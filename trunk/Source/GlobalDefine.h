//////////////////////////////////////////////////////////////////////////////
//
//  Hamiltonian closure testing by Filip Jurcicek
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __GLOBAL_DEFINE
#define __GLOBAL_DEFINE

#pragma warning(disable:4786)

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <strstream>
#include <algorithm>

#include <set>
#include <vector>
#include <list>


using namespace std;

#include "assert.h"

#define RETURN_OK                     1
#define RETURN_FAIL                   2
#define RETURN_BAD_NODE               3
#define RETURN_EXIST                  4
#define RETURN_STRUCTURE_CORRUPTED    5


#ifdef _DEBUG
  #define ASSERT(BOOLEAN_EXP) { assert(BOOLEAN_EXP); }
#else 
  #define ASSERT(BOOLEAN_EXP) { }
#endif // _DEBUG

#endif

