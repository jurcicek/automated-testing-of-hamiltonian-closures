//////////////////////////////////////////////////////////////////////////////
//
//  Hamiltonian closure testing by Filip Jurcicek
//
//////////////////////////////////////////////////////////////////////////////

#include "GlobalDefine.h"
#include "ClosureHamCycle.h"
#include "Graph.h"
#include "math.h"

//#define WRITE_OK_DOT

//////////////////////////////////////////////////////////////////////////////
//
//  Closure::Closure(void)
//
//////////////////////////////////////////////////////////////////////////////

ClosureHamCycle::ClosureHamCycle(void): m_ConditionSet(&m_ssNodes, &m_sNodeU, &m_sNodeV)
{
}

//////////////////////////////////////////////////////////////////////////////
//
//  ClosureHamCycle::~ClosureHamCycle(void)
//
//////////////////////////////////////////////////////////////////////////////

ClosureHamCycle::~ClosureHamCycle(void)
{
}


//////////////////////////////////////////////////////////////////////////////
//
//  short ClosureHamCycle::Read(string sFileName)
//
//////////////////////////////////////////////////////////////////////////////

short ClosureHamCycle::Read(string sFileName)
{
  fstream fin(sFileName.c_str(), ios::in);
  string sLine;
  string sNodeName;
  string sLeftSide, sRightSide;
  set<string>::iterator i;

  ASSERT(fin.good());

  while(fin.good())
  {
    getline(fin, sLine);

    RemoveWS(sLine);

    // empty line
    if(sLine.empty())
      continue;

    // coment
    if(sLine[0] == '/')
      continue;

    // nodes
    if(sLine.substr(0, sizeof("nodes:") - 1) == "nodes:")
    {
      sLine.erase(0, sizeof("nodes:") - 1);

      while(!sLine.empty())
      {
        Split(sLine, ",", sNodeName, sLine);
        m_ssNodes.insert(sNodeName);
      }

      continue;
    }

    // path
    if(sLine.substr(0, sizeof("path:") - 1) == "path:")
    {
      sLine.erase(0, sizeof("path:") - 1);

      Split(sLine, ",", m_sNodeU, m_sNodeV);

      continue;
    }

    // condition
    if(sLine.substr(0, sizeof("condition:") - 1) == "condition:")
    {
      sLine.erase(0, sizeof("condition:") - 1);

      Split(sLine, "<=", sLeftSide, sRightSide);
        
      m_ConditionSet.AddCondition(sLeftSide, sRightSide);

      continue;
    }
  }

 

  m_ssCoNodes = m_ssNodes;
  i = m_ssCoNodes.find(m_sNodeU);
  if(i != m_ssNodes.end())
    m_ssCoNodes.erase(i);
  i = m_ssCoNodes.find(m_sNodeV);
  if(i != m_ssNodes.end())
    m_ssCoNodes.erase(i);

  return RETURN_OK;
}

//////////////////////////////////////////////////////////////////////////////
//
//  short ClosureHamCycle::Print(void)
//
//////////////////////////////////////////////////////////////////////////////

short ClosureHamCycle::Print(void)
{
  set<string>::iterator ssIt;
  
  cout << endl;

  cout << "nodes: ";
  if(!m_ssNodes.empty())
  {
    ssIt = m_ssNodes.begin();
    cout << *ssIt;
    ssIt++;

    for(; ssIt != m_ssNodes.end(); ssIt++)
      cout << "," << *ssIt ;
  }
  cout << endl;

  cout << "path:  " << m_sNodeU << "," << m_sNodeV << endl;

  m_ConditionSet.Print();

  cout << endl;

  return RETURN_OK;
}

//////////////////////////////////////////////////////////////////////////////
//
//  short ClosureHamCycle::Parse(void)
//
//////////////////////////////////////////////////////////////////////////////

short ClosureHamCycle::ParseClosure(void)
{
  while(!m_ConditionSet.Expand());

  return RETURN_OK;
}

//////////////////////////////////////////////////////////////////////////////
//
//  short ClosureHamCycle::WriteConditionSets(string sFileName)
//
//////////////////////////////////////////////////////////////////////////////

short ClosureHamCycle::WriteConditionSet(string sFileName)
{
  m_ConditionSet.Write(sFileName);

  return RETURN_OK;
}

//////////////////////////////////////////////////////////////////////////////
//
//  void ClosureHamCycle::CombineGraphStates(short * piSelectedParts, short iMaxPartsNum)
//
//////////////////////////////////////////////////////////////////////////////

void ClosureHamCycle::CombineGraphStates(short * piSelectedParts, short iMaxPartsNum)
{
  short iI;

  iI = iMaxPartsNum - 1;

  while(iI >= 0)
  {
    if(iI == iMaxPartsNum - 1)
      if(piSelectedParts[iI] == 2)
      {
        piSelectedParts[iI] = 0;
        iI--;
        continue;
      }
      else
      {
        piSelectedParts[iI]++;
        break;
      }
    if(iI == 0)
      if(piSelectedParts[iI] == 2)
      {
        piSelectedParts[iI] = 0;
        iI--;
        continue;
      }
      else
      {
        piSelectedParts[iI]++;
        break;
      }

    if(piSelectedParts[iI] == 3)
    {
      piSelectedParts[iI] = 0;
      iI--;
      continue;
    }
    else
    {
      piSelectedParts[iI]++;
      break;
    }
  }
}

//////////////////////////////////////////////////////////////////////////////
//
//  void ClosureHamCycle::PermuteCoNodes(vector<string> * pvsPermutedCoNodes)
//
//////////////////////////////////////////////////////////////////////////////

bool ClosureHamCycle::PermuteCoNodes(vector<string> * pvsPermutedCoNodes)
{
  ASSERT(pvsPermutedCoNodes);
  set<string>::iterator sIt;

  if(pvsPermutedCoNodes->empty())
  {
    // create first permutation
    for(sIt = m_ssCoNodes.begin(); sIt != m_ssCoNodes.end(); sIt++)
      pvsPermutedCoNodes->push_back(*sIt);

    sort(pvsPermutedCoNodes->begin(), pvsPermutedCoNodes->end());
    return true;
  }
  else
  {
    // make other permutation
    return next_permutation(pvsPermutedCoNodes->begin(), pvsPermutedCoNodes->end());
  }

  return false;
}

//////////////////////////////////////////////////////////////////////////////
//
//  void ClosureHamCycle::SetPermutedNodeNames(Graph *pG, vector<string> * pvsPermutedCoNodes,
//                                     short * piSelectedParts, short iMaxPartsNum)
//
//////////////////////////////////////////////////////////////////////////////

void ClosureHamCycle::SetPermutedNodeNames(Graph *pG, vector<string> * pvsPermutedCoNodes,
                                   short * piSelectedParts, short iMaxPartsNum)
{
  short iI;
  short iNodeIdx = 0;

  for(iI = 0; iI < iMaxPartsNum; iI++)
  {
    if(iI == 0)
      switch(piSelectedParts[iI])
      {
        case 0:
          iNodeIdx += 1;
          break;
        case 1:
          pG->SetNodeName(1, (*pvsPermutedCoNodes)[0] + "-");
          iNodeIdx += 2;
          break;
        case 2:
          pG->SetNodeName(2, (*pvsPermutedCoNodes)[0] + "-");
          iNodeIdx += 3;
          break;
      }
    if(iI == iMaxPartsNum - 1)
      switch(piSelectedParts[iI])
      {
        case 0:
          pG->SetNodeName(pG->GetNumOfNodes() - 2, (*pvsPermutedCoNodes)[pvsPermutedCoNodes->size() - 1]);
          break;
        case 1:
          pG->SetNodeName(pG->GetNumOfNodes() - 2, (*pvsPermutedCoNodes)[pvsPermutedCoNodes->size() - 1] + "+");
          pG->SetNodeName(pG->GetNumOfNodes() - 3, (*pvsPermutedCoNodes)[pvsPermutedCoNodes->size() - 1]);
          break;
        case 2:
          pG->SetNodeName(pG->GetNumOfNodes() - 3, (*pvsPermutedCoNodes)[pvsPermutedCoNodes->size() - 1] + "+");
          pG->SetNodeName(pG->GetNumOfNodes() - 4, (*pvsPermutedCoNodes)[pvsPermutedCoNodes->size() - 1]);
          break;
      }
    if( (iI >0) && (iI < iMaxPartsNum - 1) )
      switch(piSelectedParts[iI])
      {
        case 0:
          pG->SetNodeName(iNodeIdx, (*pvsPermutedCoNodes)[iI - 1]);
          iNodeIdx += 1;
          break;
        case 1:
          pG->SetNodeName(iNodeIdx, (*pvsPermutedCoNodes)[iI - 1]);
          pG->SetNodeName(iNodeIdx + 1, (*pvsPermutedCoNodes)[iI - 1] + "+:" + (*pvsPermutedCoNodes)[iI] + "-");
          iNodeIdx += 2;
          break;
        case 2:
          pG->SetNodeName(iNodeIdx, (*pvsPermutedCoNodes)[iI - 1]);
          pG->SetNodeName(iNodeIdx + 1, (*pvsPermutedCoNodes)[iI - 1] + "+");
          pG->SetNodeName(iNodeIdx + 2, (*pvsPermutedCoNodes)[iI] + "-");
          iNodeIdx += 3;
          break;
        case 3:
          pG->SetNodeName(iNodeIdx, (*pvsPermutedCoNodes)[iI - 1]);
          pG->SetNodeName(iNodeIdx + 1, (*pvsPermutedCoNodes)[iI - 1] + "+");
          pG->SetNodeName(iNodeIdx + 3, (*pvsPermutedCoNodes)[iI] + "-");
          iNodeIdx += 4;
          break;
      }
  }
}

//////////////////////////////////////////////////////////////////////////////
//
//  short ClosureHamCycle::GenerateGraphs(string sFileName)
//
//////////////////////////////////////////////////////////////////////////////

/* 
    selected parts

    0)    u - x             | x          --          y |             y - v               

    1)    u - (x-) - x      | x    -  (x+:y-)  -     y |      y - (y-) - v        

    2)    u - // - (x-) - x | x - (x+)    -   (y-) - y | y - (y-) - // - v

    3)                      | x - (x+) - // - (y-) - y | 


*/

short ClosureHamCycle::GenerateGraphs(string sFileName)
{
  bool bIsHamiltonian = true;
  vector<string> vsPermutedCoNodes;
  list<Graph *> lGraph;
  string sSetFileName;
  strstream str;
  long lFileCounter = 0; 
  short iI;
  Graph * pG = NULL;
  short iMaxPartsNum = 2 + m_ssCoNodes.size() - 1; // U,V + co nodes - 1
  short * piSelectedParts = new short[iMaxPartsNum];  
  short iNodesNum;
  long lNumOfNonHamiltonianGraphs = 0;
  short iConfigurationCounter = 1;
  ConditionSet * pSet;
  short iPercent = 0;
  short iPercentNew;
  long liMaxGraphNum = 0;

  memset(piSelectedParts, 0, sizeof(short)*iMaxPartsNum);

  while(true) 
  {
    // generate size node configuration of graph 
    //==========================================================================
    iNodesNum = 0;
    for(iI = 0; iI < iMaxPartsNum; iI++)
      iNodesNum += piSelectedParts[iI] + 1;
  
    iNodesNum++;

    // for all permutations of co nodes
    while(PermuteCoNodes(&vsPermutedCoNodes))
    {
      // create new graph
      pG = new Graph(iNodesNum);
      ASSERT(pG);
      pG->SetNodeName(0, m_sNodeU);
      pG->SetNodeName(iNodesNum - 1, m_sNodeV);
      SetPermutedNodeNames(pG, &vsPermutedCoNodes, piSelectedParts, iMaxPartsNum);

      // prepare graph set 
      lGraph.clear();
      lGraph.push_back(pG);

      pSet = new ConditionSet(m_ConditionSet);
      ASSERT(pSet);

      pG->RemoveAllEdges();
      // add edges on the path between U and V
      for(iI = 0; iI < iNodesNum - 1; iI++)
        pG->AddEdge(iI, iI + 1);


      // for this condition set generate graph
      if(pSet->AddRemovableEdges(pG) != RETURN_OK)
        continue;

      iPercentNew = (int)((double)iConfigurationCounter / (3*3*pow(4,m_ssCoNodes.size() - 1)) * 100);
      if(iPercent < iPercentNew)
      {
        iPercent = iPercentNew;
        cout << "Processed " << iPercent << " % of all graphs.";
        cout << " Graph number: " << lFileCounter << endl;
      }

      while(!lGraph.empty())
      {
        // each processed graph has unique number
        lFileCounter++;

        pG = *lGraph.begin();
        lGraph.pop_front();

        pSet->AddEdges(pG, lGraph);

        if(liMaxGraphNum < lGraph.size())
        {
          liMaxGraphNum = lGraph.size();
          cout << "Max graph size: " << liMaxGraphNum << " Graph number: " << lFileCounter << endl;
        }

        if(!pG->IsHamiltonianCycle())
        {
          bIsHamiltonian = false;
          lNumOfNonHamiltonianGraphs++;
          pG->Write(sFileName + ".bad.dot");
        }
#ifdef WRITE_OK_DOT
        else
        {
          pG->Write(sFileName + ".good.dot");
        }
#endif

//        pG->Write(sFileName + ".all.dot");

        delete pG;
      }

      delete pSet;
    }
    // prepare for new permutation
    vsPermutedCoNodes.clear();

    // test if all configartions were tested
    if(iNodesNum == 2 + m_ssCoNodes.size()*3 + m_ssCoNodes.size() + 1)
      break;

    CombineGraphStates(piSelectedParts, iMaxPartsNum);

    iConfigurationCounter++;
  }

  delete [] piSelectedParts;

  cout << endl << endl << "Is hamiltonian: " << bIsHamiltonian << endl;
  cout << "Number of non-hamiltonian graphs is: " << lNumOfNonHamiltonianGraphs << endl << endl ;

  return RETURN_OK;
}

