//////////////////////////////////////////////////////////////////////////////
//
//  Hamiltonian cycle closure testing by Filip Jurcicek
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __CLOSURE_HAM_CYLCE
#define __CLOSURE_HAM_CYCLE

#include "GlobalDefine.h"
#include "ConditionSet.h"
#include "Parse.h"
#include "Graph.h"

class ClosureHamCycle: public Parse
{
  set<string> m_ssNodes;
  // in this set are only nodesm whic are not equal to m_sNodeU and m_sNodeV
  set<string> m_ssCoNodes; 
  string m_sNodeU;
  string m_sNodeV;
  ConditionSet m_ConditionSet;

public: 
  ClosureHamCycle(void);
  ~ClosureHamCycle(void);

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