//////////////////////////////////////////////////////////////////////////////
//
//  Hamiltonian closure testing by Filip Jurcicek
//
//////////////////////////////////////////////////////////////////////////////

#include "GlobalDefine.h"
#include "ConditionSet.h"

//#define DEBUG_PARSE

//////////////////////////////////////////////////////////////////////////////
//
//  ConditionSet::ConditionSet(set<string> & ssNodes, string * psNodeU, 
//                             string * psNodeV)
//
//////////////////////////////////////////////////////////////////////////////

ConditionSet::ConditionSet(set<string> * pssNodes, string * psNodeU, string * psNodeV)
{
  m_pssNodes = pssNodes;
  m_psNodeU = psNodeU;
  m_psNodeV = psNodeV;
}

//////////////////////////////////////////////////////////////////////////////
//
//  ConditionSet::ConditionSet(ConditionSet & conSet)
//
//////////////////////////////////////////////////////////////////////////////

ConditionSet::ConditionSet(ConditionSet & conSet)
{
  m_pssNodes = conSet.m_pssNodes;
  m_psNodeU = conSet.m_psNodeU;
  m_psNodeV = conSet.m_psNodeV;
  m_lConditions = conSet.m_lConditions;
}

//////////////////////////////////////////////////////////////////////////////
//
//  ConditionSet::~ConditionSet(void)
//
//////////////////////////////////////////////////////////////////////////////

ConditionSet::~ConditionSet(void)
{
}

//////////////////////////////////////////////////////////////////////////////
//
//  void ConditionSet::Print(void)
//
//////////////////////////////////////////////////////////////////////////////

void ConditionSet::Print(void)
{
  list<Condition>::iterator i;

  for(i = m_lConditions.begin(); i != m_lConditions.end(); i++)
  {
    cout << "condition: " << i->m_sLeftSide << " <= " << i->m_sRightSide << endl;
  }
}

//////////////////////////////////////////////////////////////////////////////
//
//  ConditionSet::Write(string sFileName)
//
//////////////////////////////////////////////////////////////////////////////

void ConditionSet::Write(string sFileName)
{
  fstream fout(sFileName.c_str(), ios::out);
  list<Condition>::iterator i;

  for(i = m_lConditions.begin(); i != m_lConditions.end(); i++)
  {
    fout << "condition: " << i->m_sLeftSide << " <= " << i->m_sRightSide << endl;
  }

  fout.close();
}

//////////////////////////////////////////////////////////////////////////////
//
//  bool ConditionSet::Expand(void)
//
//  Returns true if wasn't perfomed any expansion.
//  
//////////////////////////////////////////////////////////////////////////////

bool ConditionSet::Expand(void)
{
  bool bRet = true; 
  list<Condition>::iterator i, j;
  string sBegin, sRest, sParameter;

  //==========================================================================
  //  rule 0:
  //
  //          (x)  <<=>>  x 
  //
  //==========================================================================

  for(i = m_lConditions.begin(); i != m_lConditions.end(); i++)
  {
    // simplify expresions
    RemoveAmbientBrackets(i->m_sLeftSide, '(', ')');
    RemoveAmbientBrackets(i->m_sRightSide, '(', ')');
  }

  //==========================================================================
  //  rule 1:  
  //
  //          x | y <= exp  <<=>>  x <= exp  
  //                               y <= exp  
  //
  //==========================================================================

  i = m_lConditions.begin();
  while(i != m_lConditions.end())
  {
    if(Parse::Split(i->m_sLeftSide, "|", sBegin, sRest))
    {
      RemoveAmbientBrackets(sBegin, '(', ')');
      RemoveAmbientBrackets(sRest, '(', ')');
      
      AddCondition(sBegin, i->m_sRightSide);

      while(!sRest.empty())
      {
        Parse::Split(sRest, "|", sBegin, sRest);
        RemoveAmbientBrackets(sBegin, '(', ')');
        RemoveAmbientBrackets(sRest, '(', ')');

        AddCondition(sBegin, i->m_sRightSide);
      }

      // remove substituted expresion
      i = m_lConditions.erase(i);

      bRet = false; 
    }
    else
      i++;
  }

#ifdef DEBUG_PARSE
  Write("rule1.txt");
#endif

  //==========================================================================
  //  rule 2:
  //
  //          exp <= x & y  <<=>>  exp <=  x 
  //                               exp <=  y 
  //
  //==========================================================================

  i = m_lConditions.begin();
  while(i != m_lConditions.end())
  {
    // check the priority (operator | has lower priority than &)
    if(Parse::Split(i->m_sRightSide, "|", sBegin, sRest, true))
    {
      i++;
      continue;
    }

    // no union, so do this
    if(Parse::Split(i->m_sRightSide, "&", sBegin, sRest))
    {
      RemoveAmbientBrackets(sBegin, '(', ')');
      RemoveAmbientBrackets(sRest, '(', ')');
      
      AddCondition(i->m_sLeftSide, sBegin);

      while(!sRest.empty())
      {
        Parse::Split(sRest, "&", sBegin, sRest);
        RemoveAmbientBrackets(sBegin, '(', ')');
        RemoveAmbientBrackets(sRest, '(', ')');

        AddCondition(i->m_sLeftSide, sBegin);
      }

      // remove substituted expresion
      i = m_lConditions.erase(i);

      bRet = false; 
    }
    else
      i++;
  }

#ifdef DEBUG_PARSE
  Write("rule2.txt");
#endif

  //==========================================================================
  //  rule 3:
  //
  //          {x,y} <<=>>  x | y
  //
  //==========================================================================

  for(i = m_lConditions.begin(); i != m_lConditions.end(); i++)
  {
    if(RemoveAmbientBrackets(i->m_sLeftSide, '{', '}'))
    {
      if(Parse::Split(i->m_sLeftSide, ",", sBegin, sRest))
      {
        i->m_sLeftSide = sBegin;

        while(!sRest.empty())
        {
          Parse::Split(sRest, ",", sBegin, sRest);
          RemoveAmbientBrackets(sBegin, '{', '}');
  
          i->m_sLeftSide += "|" + sBegin;
        }
        bRet = false; 
      }
    }

    if(RemoveAmbientBrackets(i->m_sRightSide, '{', '}'))
    {
      if(Parse::Split(i->m_sRightSide, ",", sBegin, sRest))
      {
        i->m_sRightSide = sBegin;

        while(!sRest.empty())
        {
          Parse::Split(sRest, ",", sBegin, sRest);
          RemoveAmbientBrackets(sBegin, '{', '}');

          i->m_sRightSide += "|" + sBegin;
        }
        bRet = false; 
      }
    }
  }

#ifdef DEBUG_PARSE
  Write("rule3.txt");
#endif

  //==========================================================================
  //  rule 4: 
  //
  //          x <= N(y) <<=>>  x_y <= E(G) 
  //
  //==========================================================================
  
  for(i = m_lConditions.begin(); i != m_lConditions.end(); i++)
  {
    if(ExtractParameterFromFunction(i->m_sRightSide, "N", '(', ')', sParameter))
      if(IsNode(i->m_sLeftSide) && IsNode(sParameter))
      {
        i->m_sLeftSide += "_" + sParameter;
        i->m_sRightSide = "E(G)";
        bRet = false; 
      }
  }

#ifdef DEBUG_PARSE
  Write("rule4.txt");
#endif

  return bRet;
}

//////////////////////////////////////////////////////////////////////////////
//
//  bool ConditionSet::IsNode(string sNodeName)
//
//////////////////////////////////////////////////////////////////////////////

bool ConditionSet::IsNode(string sNodeName)
{
  return (m_pssNodes->find(sNodeName) != m_pssNodes->end()) ? true : false; 
}

//////////////////////////////////////////////////////////////////////////////
//
//  bool ConditionSet::IsNodeU(string sNodeName)
//
//////////////////////////////////////////////////////////////////////////////

bool ConditionSet::IsNodeU(string sNodeName)
{
  return (*m_psNodeU) == sNodeName; 
}

//////////////////////////////////////////////////////////////////////////////
//
//  bool ConditionSet::IsNodeV(string sNodeName)
//
//////////////////////////////////////////////////////////////////////////////

bool ConditionSet::IsNodeV(string sNodeName)
{
  return (*m_psNodeV) == sNodeName; 
}

//////////////////////////////////////////////////////////////////////////////
//
//  short ConditionSet::AddRemovableEdges(Graph *pG)
//
//////////////////////////////////////////////////////////////////////////////

short ConditionSet::AddRemovableEdges(Graph *pG, bool bTestCorruption)
{
  list<Condition>::iterator i;
  string sBegin, sRest;
  short iSourceNode, iDestinationNode;
  Graph StructureGraph(m_pssNodes->size());
  short iI,iJ;
  set<string>::iterator ssIt;

  ssIt = m_pssNodes->begin();
  for(iI = 0; iI < StructureGraph.GetNumOfNodes(); iI++)
  {
    StructureGraph.SetNodeName(iI, *ssIt);
    ssIt++;
  }

  i = m_lConditions.begin();
  while(i != m_lConditions.end())
  {
    if( i->m_sRightSide == "E(G)" )
    {
      if(Parse::Split(i->m_sLeftSide, "_", sBegin, sRest))
      {
        if( IsNode(sBegin) && IsNode(sRest) )
        {
          //-----------------------------------------------------------------
          // exclude all graphs configuration which cannot contain this edge,
          // beacause we make intersection of graph configurations and 
          // conditions sets there 
          //-----------------------------------------------------------------

          if(sBegin != sRest) // loop edge is possible, but is not nessesary
            if(pG->AddEdge(sBegin, sRest) == RETURN_BAD_NODE)
              return RETURN_FAIL; 

          if(sBegin != sRest) // loop edge is possible, but is not nessesary
            // add edge to the structure graph
            if(StructureGraph.AddEdge(sBegin, sRest) == RETURN_BAD_NODE)
              return RETURN_FAIL; 
        }
      }

      i = m_lConditions.erase(i);
    }
    else
      i++;
  }

  // test if in configuration is corrupted structure
  if(bTestCorruption)
  {
    // all edges which aren't in structural graph
    for(iI = 0; iI < StructureGraph.GetNumOfNodes(); iI++)
      for(iJ = iI + 1; iJ < StructureGraph.GetNumOfNodes(); iJ++)
        if(!StructureGraph.Edge(iI, iJ))
        {
          // needn't be in configuration
          iSourceNode = pG->FindNode(StructureGraph.GetNodeName(iI));
          iDestinationNode = pG->FindNode(StructureGraph.GetNodeName(iJ)); 
  
          if( (iSourceNode == -1) || (iDestinationNode == -1) )
            return RETURN_BAD_NODE;

          if(pG->Edge(iSourceNode, iDestinationNode))
          {
            pG->Write("corrupted.dot");
            return RETURN_STRUCTURE_CORRUPTED;
          }
        }
  }

  return RETURN_OK;
}

//////////////////////////////////////////////////////////////////////////////
//
//  short ConditionSet::AddEdges(Graph *pG, list<Graph *> & lGraph)
//
//////////////////////////////////////////////////////////////////////////////

short ConditionSet::AddEdges(Graph *pG, list<Graph *> & lGraph)
{
  short iRet;
  list<Condition>::iterator i;
  bool bInvalidCondition;
  bool bContinue = true;
  set<short> siLeftNodes, siRightNodes, siCoreNodes;
  set<short>::iterator siIt, siJt;

  while(bContinue)
  {
    bContinue = false;
    bInvalidCondition = false;
    
    for(i = m_lConditions.begin(); i != m_lConditions.end(); i++)
    {
      string str = i->m_sLeftSide;
      // get set of nodes on the left side
      siLeftNodes.clear();
      siCoreNodes.clear();
      iRet = pG->SolveExpression(i->m_sLeftSide, siLeftNodes, siCoreNodes);
      if(iRet != RETURN_OK)
      {
        bInvalidCondition = true;
        break;
      }


      //pG->Write("test.dot");

      // solve clique on the right side
      if(i->m_sRightSide == "clique")
      {
        // clique is important only for two nodes an more
        if((siLeftNodes.size() > 1) && !pG->IsClique(siLeftNodes))
        {
          // create new graphs with edges between core nodes (x node of N(x)) and N(y)
          for(siIt = siCoreNodes.begin(); siIt != siCoreNodes.end(); siIt++)
            for(siJt = siLeftNodes.begin(); siJt != siLeftNodes.end(); siJt++)
            {
              Graph * pNewG = new Graph(*pG);

              pNewG->AddEdge(*siIt, *siJt);

              lGraph.push_front(pNewG);
            }

          pG->SetClique(siLeftNodes);
          bContinue = true;
        }
      }
      else
      {
        // get set of nodes on the right side
        siRightNodes.clear();
        siCoreNodes.clear();
        iRet = pG->SolveExpression(i->m_sRightSide, siRightNodes, siCoreNodes);

        if(iRet != RETURN_OK)
        {
          bInvalidCondition = true;
          break;
        }

        // solve neighbourhood on the right side
        for(siIt = siLeftNodes.begin(); siIt != siLeftNodes.end(); siIt++)
          if(siRightNodes.find(*siIt) == siRightNodes.end())
          {
            // node isn't in set
            //pG->Write("test.dot");
            ASSERT(pG->MakeNeighbourhood(i->m_sRightSide, *siIt, lGraph));

            bContinue = true;
          }
      }
    }

    if(bInvalidCondition)
    {
      // invalid syntax in closure definition => abort
      cout << "Invalid condition in closure set: " << i->m_sLeftSide 
        << " <= " << i->m_sRightSide << endl;
      abort();
    }

    // pG->Write("test.dot");
  }

  return RETURN_OK;
}
