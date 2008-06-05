//////////////////////////////////////////////////////////////////////////////
//
//  Hamiltonian closure testing by Filip Jurcicek
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __PARSE
#define __PARSE

#include "GlobalDefine.h"

class Parse
{
public:
  // remove white space
  bool RemoveWS(string & str); 
  // split in to the two parts by delimeter
  bool Split(string str, string sDelimeter, string & sBegin, string & sRest, bool bTest = false);
  // split in to the two parts by delimeters
  bool Split(string str, set<string> ssDelimeters, string & sBegin, string & sRest, string & sUsedDelimeter);
  // remove surrounding brackets
  bool RemoveAmbientBrackets(string & str, char chBracket1, char chBracket2);
  // function parametr extraction
  bool ExtractParameterFromFunction(string str, string sFunction, 
    char chBracket1, char chBracket2, string & sParameter);

  bool Successor(string str, string & sNode);

  bool Predecessor(string str, string & sNode);
};

#endif
