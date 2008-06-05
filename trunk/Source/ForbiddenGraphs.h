//////////////////////////////////////////////////////////////////////////////
//
//  Forbidden graphs
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __FORBIDDEN_GRAPHS
#define __FORBIDDEN_GRAPHS

#include "GlobalDefine.h"
#include "ConditionSet.h"
#include "Parse.h"
#include "Graph.h"

class ForbiddenGraphs: public Parse
{
  bool m_bUseUV;
  short m_iDegree;
  short m_iMinNumOfConfigurationNodes;

  set<string> m_ssNodes;
  // in this set are only nodes which are not equal to m_sNodeU and m_sNodeV
  set<string> m_ssCoNodes; 
  string m_sNodeU;
  string m_sNodeV;
  ConditionSet m_ConditionSet;

public: 
  ForbiddenGraphs(void);
  ~ForbiddenGraphs(void);

  short Read(string sFileName);
  short Print(void);
  short WriteConditionSet(string sFileName);

  short ParseClosure(void);
  short GenerateGraphs(string sFileName);
private:

  void CombineGraphStates(short * piSelectedParts, short iMaxPartsNum);
  bool PermuteCoNodes(vector<string> * pvsPermutedCoNodes);
  void SetPermutedNodeNames(Graph *pG, vector<string> * pvsPermutedCoNodes,
                            short * piSelectedParts, short iMaxPartsNum);
};

#endif