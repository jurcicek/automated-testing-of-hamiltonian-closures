//////////////////////////////////////////////////////////////////////////////
//
//  Hamiltonian closure testing by Filip Jurcicek
//
//////////////////////////////////////////////////////////////////////////////

#include "GlobalDefine.h"
#include "Parse.h"

//////////////////////////////////////////////////////////////////////////////
//
//  bool Parse::RemoveWS(string & str)
//
//////////////////////////////////////////////////////////////////////////////

bool Parse::RemoveWS(string & str)
{
  short iI;
  bool bRet = false;

  for(iI = 0; iI < str.length(); iI++)
  {
    switch(str[iI])
    {
      case '\n':
      case '\r':
      case '\t':
      case ' ' :
      {
        str.erase(iI, 1);
        iI--;
        bRet = true;
        break;
      }
    }
  }

  return bRet;
}

//////////////////////////////////////////////////////////////////////////////
//
//  bool Parse::Split(string str, string sDelimeter, string & sBegin, 
//                    string & sRest, bool bTest)
//
//////////////////////////////////////////////////////////////////////////////

bool Parse::Split(string str, string sDelimeter, string & sBegin, string & sRest, bool bTest)
{
  short iNumOfBracket = 0;
  short iI;

  for(iI = 0; iI < str.length(); iI++)
  {
    switch(str[iI])
    {
      case '{':
      case '(':
      case '[':
      {
        iNumOfBracket++;
        break;
      }
      case '}':
      case ')':
      case ']':
      {
        iNumOfBracket--;
        break;
      }
    }

    ASSERT(iNumOfBracket >= 0);

    if( (iNumOfBracket == 0) && (str.substr(iI, sDelimeter.length()) == sDelimeter) )
    {
      ASSERT(iI > 0);

      if(!bTest)
      {
        sBegin = str.substr(0, iI);
        sRest = str.substr(iI + sDelimeter.length());
      }
      
      return true;
    }
  }

  if(!bTest)
  {
    sBegin = str;
    sRest = "";
  }

  return false;
} 

//////////////////////////////////////////////////////////////////////////////
//
//  bool Parse::Split(string str, set<string> ssDelimeters, string & sBegin, 
//    string & sRest, string & sUsedDelimeter)
//
//////////////////////////////////////////////////////////////////////////////

bool Parse::Split(string str, set<string> ssDelimeters, string & sBegin, 
                  string & sRest, string & sUsedDelimeter)
{
  short iNumOfBracket = 0;
  short iI;
  set<string>::iterator ssIt;

  for(iI = 0; iI < str.length(); iI++)
  {
    switch(str[iI])
    {
      case '{':
      case '(':
      case '[':
      {
        iNumOfBracket++;
        break;
      }
      case '}':
      case ')':
      case ']':
      {
        iNumOfBracket--;
        break;
      }
    }

    ASSERT(iNumOfBracket >= 0);


    if(iNumOfBracket == 0) 
    {
      for(ssIt = ssDelimeters.begin(); ssIt != ssDelimeters.end(); ssIt++)
        if(str.substr(iI, ssIt->length()) == *ssIt )
        {
          ASSERT(iI > 0);

          sBegin = str.substr(0, iI);
          sRest = str.substr(iI + ssIt->length());
          sUsedDelimeter = *ssIt;

          return true;
        }
    }
  }

  return false;
} 

//////////////////////////////////////////////////////////////////////////////
//
//  bool Parse::RemoveAmbientBrackets(string & str, char chBracket1, char chBracket2)
//
//////////////////////////////////////////////////////////////////////////////

bool Parse::RemoveAmbientBrackets(string & str, char chBracket1, char chBracket2)
{
  bool bRet = false;

  while(true)
  {
    if( (str[0]                == chBracket1) && 
        (str[str.length() - 1] == chBracket2)    )
    {
      str.erase(0, 1);
      str.erase(str.length() - 1, 1);

      bRet = true;
    }
    else
      break;
  }

  return bRet;
}

//////////////////////////////////////////////////////////////////////////////
//
//  bool Parse::ExtractParameterFromFunction(string str, string sFunction, 
//       char chBracket1, char chBracket2, string & sParameter)
//
//////////////////////////////////////////////////////////////////////////////

bool Parse::ExtractParameterFromFunction(string str, string sFunction, 
                                         char chBracket1, char chBracket2, 
                                         string & sParameter)
{
  bool bRet = false;
  short iI;
  short iNumOfBracket = 0;

  // name of function is up to first left bracket
  for(iI = 0; iI < str.length(); iI++)
    if(str[iI] == chBracket1) 
      if(iI == sFunction.length())
        break;
      else
        return false;
  
  if(str.substr(0, sFunction.length()) == sFunction)
  {
    str.erase(0, sFunction.length());

    for(iI = 0; iI < str.length(); iI++)
    {
      if(str[iI] == chBracket1) 
        iNumOfBracket++;

      if(str[iI] == chBracket2)
        iNumOfBracket--;

      if(!iNumOfBracket)
        break;
    }     

    if(iI != str.length() - 1)
      return false;

    sParameter = str.substr(1, str.length() - 2);

    bRet = true;
  }

  return bRet;
}

//////////////////////////////////////////////////////////////////////////////
//
//  bool Parse::Successor(string str, string & sNode)
//
//////////////////////////////////////////////////////////////////////////////

bool Parse::Successor(string str, string & sNode)
{
  if(str[str.length() - 1] == '+')
  {
    sNode = str.substr(0, str.length() - 1);

    return true;
  }

  return false;
}

//////////////////////////////////////////////////////////////////////////////
//
//  bool Parse::Predecessor(string str, string & sNode)
//
//////////////////////////////////////////////////////////////////////////////

bool Parse::Predecessor(string str, string & sNode)
{
  if(str[str.length() - 1] == '-')
  {
    sNode = str.substr(0, str.length() - 1);

    return true;
  }

  return false;
}
