//////////////////////////////////////////////////////////////////////////////
//
//  Hamiltonian closure testing by Filip Jurcicek
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __CLOSURE_SET
#define __CLOSURE_SET

#include "GlobalDefine.h"
#include "Parse.h"
#include "Graph.h"

class Condition
{
public:
  string m_sLeftSide;
  string m_sRightSide;
};

class ConditionSet: public Parse
{
  list<Condition> m_lConditions;
  set<string> * m_pssNodes;
  string * m_psNodeU;
  string * m_psNodeV;
  
public: 
  ConditionSet(set<string> * pssNodes, string * psNodeU, string * psNodeV);
  ConditionSet(ConditionSet & conSet);
  ~ConditionSet(void);

  void AddCondition(string sLeftSide, string sRightSide)
  {
    Condition con;

    con.m_sLeftSide = sLeftSide;
    con.m_sRightSide = sRightSide;

    m_lConditions.push_back(con);
  }

  void ClearConditions(void)
  {
    m_lConditions.clear();
  }

  bool Expand(void);
  short AddRemovableEdges(Graph *pG, bool bTestCorruption = false);
  short AddEdges(Graph *pG, list<Graph *> & lGraph);

  void Print(void);
  void Write(string sFileName);
  
private:

  inline bool IsNode(string sNodeName);
  inline bool IsNodeU(string sNodeName);
  inline bool IsNodeV(string sNodeName);
};

#endif
