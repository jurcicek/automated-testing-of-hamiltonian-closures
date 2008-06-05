//////////////////////////////////////////////////////////////////////////////
//
//  Hamiltonian closure testing by Filip Jurcicek
//
//////////////////////////////////////////////////////////////////////////////

#include "GlobalDefine.h"
#include "ClosureHamPath.h"
#include "Graph.h"
#include "math.h"

//#define WRITE_OK_DOT

//////////////////////////////////////////////////////////////////////////////
//
//  ClosureHamPath::ClosureHamPath(void)
//
//////////////////////////////////////////////////////////////////////////////

ClosureHamPath::ClosureHamPath(void): m_ConditionSet(&m_ssNodes, &m_sNodeU, &m_sNodeV),
  m_PathType(aPath)
{
}

//////////////////////////////////////////////////////////////////////////////
//
//  ClosureHamPath::~ClosureHamPath(void)
//
//////////////////////////////////////////////////////////////////////////////

ClosureHamPath::~ClosureHamPath(void)
{
}

//////////////////////////////////////////////////////////////////////////////
//
//  short ClosureHamPath::Read(string sFileName)
//
//////////////////////////////////////////////////////////////////////////////

short ClosureHamPath::Read(string sFileName)
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
  
    // test
    if(sLine.substr(0, sizeof("test:") - 1) == "test:")
    {
      sLine.erase(0, sizeof("test:") - 1);
      Parse::RemoveWS(sLine);
      
      if(sLine == "aPath")
      {
        m_PathType = aPath;
        continue;
      }
      if(sLine == "sPath")
      {
        m_PathType = sPath;
        continue;
      }
      if(sLine == "stPath")
      {
        m_PathType = stPath;
        continue;
      }

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
//  short ClosureHamPath::Print(void)
//
//////////////////////////////////////////////////////////////////////////////

short ClosureHamPath::Print(void)
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
  cout << "test:  ";
  switch(m_PathType)
  {
  case aPath: 
    cout << "aPath" <<  endl;
    break;
  case sPath:
    cout << "sPath" <<  endl;
    break;
  case stPath:
    cout << "stPath" <<  endl;
    break;
  }

  m_ConditionSet.Print();

  cout << endl;

  return RETURN_OK;
}

//////////////////////////////////////////////////////////////////////////////
//
//  short ClosureHamPath::Parse(void)
//
//////////////////////////////////////////////////////////////////////////////

short ClosureHamPath::ParseClosure(void)
{
  while(!m_ConditionSet.Expand());

  return RETURN_OK;
}

//////////////////////////////////////////////////////////////////////////////
//
//  short ClosureHamPath::WriteConditionSets(string sFileName)
//
//////////////////////////////////////////////////////////////////////////////

short ClosureHamPath::WriteConditionSet(string sFileName)
{
  m_ConditionSet.Write(sFileName);

  return RETURN_OK;
}

//////////////////////////////////////////////////////////////////////////////
//
//  void ClosureHamPath::CombineGraphStates(short * piSelectedParts, short iMaxPartsNum)
//
//////////////////////////////////////////////////////////////////////////////

void ClosureHamPath::CombineGraphStates(short * piSelectedParts, short iMaxPartsNum)
{
  short iI;

  iI = iMaxPartsNum - 1;

  while(iI >= 0)
  {
    /*
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
    */

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
//  void ClosureHamPath::PermuteCoNodes(vector<string> * pvsPermutedCoNodes)
//
//////////////////////////////////////////////////////////////////////////////

bool ClosureHamPath::PermuteCoNodes(vector<string> * pvsPermutedCoNodes)
{
  ASSERT(pvsPermutedCoNodes);
  set<string>::iterator sIt;
  
  if(pvsPermutedCoNodes->empty())
  {
    // create first permutation
    for(sIt = m_ssCoNodes.begin(); sIt != m_ssCoNodes.end(); sIt++)
      pvsPermutedCoNodes->push_back(*sIt);

/**/
    // insert one co node for nodes U and V
    pvsPermutedCoNodes->push_back("UV");

    sort(pvsPermutedCoNodes->begin(), pvsPermutedCoNodes->end());

/**/
/*    sort(pvsPermutedCoNodes->begin(), pvsPermutedCoNodes->end());

    // insert one co node for nodes U and V
    vector<string>::iterator vIt;
    vIt = pvsPermutedCoNodes->begin();
    vIt++;

    pvsPermutedCoNodes->insert(vIt, "UV");
/**/
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
//  void ClosureHamPath::SetPermutedNodeNames(Graph *pG, vector<string> * pvsPermutedCoNodes,
//                                     short * piSelectedParts, short iMaxPartsNum)
//
//////////////////////////////////////////////////////////////////////////////

void ClosureHamPath::SetPermutedNodeNames(Graph *pG, vector<string> * pvsPermutedCoNodes,
                                   short * piSelectedParts, short iMaxPartsNum)
{
  short iI;
  short iNodeIdx = 0;
  vector<string> vsPCN(*pvsPermutedCoNodes);

  for(iI = 0; iI < iMaxPartsNum; iI++)
  {
    if(iI == 0)
      switch(piSelectedParts[iI])
      {
        case 0:
          break;
        case 1:
          iNodeIdx += 1;
          break;
        case 2:
          if(vsPCN[0] == "UV")
            pG->SetNodeName(1, "u-");
          else
            pG->SetNodeName(1, vsPCN[0] + "-");
          iNodeIdx += 2;


          break;
        case 3:
          if(vsPCN[0] == "UV")
            pG->SetNodeName(2, "u-");
          else
            pG->SetNodeName(2, vsPCN[0] + "-");
          iNodeIdx += 3;
          break;
      }
    if(iI == iMaxPartsNum - 1)
      switch(piSelectedParts[iI])
      {
        case 0:
          if(vsPCN[vsPCN.size() - 1] == "UV")
          {
            pG->SetNodeName(pG->GetNumOfNodes() - 1, "v");
            pG->SetNodeName(pG->GetNumOfNodes() - 2, "u");
          }

          else
            pG->SetNodeName(pG->GetNumOfNodes() - 1, vsPCN[vsPCN.size() - 1]);
          break;
        case 1:
          if(vsPCN[vsPCN.size() - 1] == "UV")
          {
            pG->SetNodeName(pG->GetNumOfNodes() - 2, "v");
            pG->SetNodeName(pG->GetNumOfNodes() - 3, "u");
          }
          else
            pG->SetNodeName(pG->GetNumOfNodes() - 2, vsPCN[vsPCN.size() - 1]);
          break;
        case 2:
          if(vsPCN[vsPCN.size() - 1] == "UV")
          {
            pG->SetNodeName(pG->GetNumOfNodes() - 2, "v+");
            pG->SetNodeName(pG->GetNumOfNodes() - 3, "v");
            pG->SetNodeName(pG->GetNumOfNodes() - 4, "u");
          }
          else
          {
            pG->SetNodeName(pG->GetNumOfNodes() - 2, vsPCN[vsPCN.size() - 1] + "+");
            pG->SetNodeName(pG->GetNumOfNodes() - 3, vsPCN[vsPCN.size() - 1]);
          }
          break;
        case 3:
          if(vsPCN[vsPCN.size() - 1] == "UV")
          {
            pG->SetNodeName(pG->GetNumOfNodes() - 3, "v+");
            pG->SetNodeName(pG->GetNumOfNodes() - 4, "v");
            pG->SetNodeName(pG->GetNumOfNodes() - 5, "u");
          }
          else
          {
            pG->SetNodeName(pG->GetNumOfNodes() - 3, vsPCN[vsPCN.size() - 1] + "+");
            pG->SetNodeName(pG->GetNumOfNodes() - 4, vsPCN[vsPCN.size() - 1]);
          }
          break;
      }
    if( (iI > 0) && (iI < iMaxPartsNum - 1) )
    {
      switch(piSelectedParts[iI])
      {
        case 0:
          if(vsPCN[iI - 1] == "UV")
          {
            pG->SetNodeName(iNodeIdx, "u");
            pG->SetNodeName(iNodeIdx + 1, "v");
            iNodeIdx++;
          }
          else
            pG->SetNodeName(iNodeIdx, vsPCN[iI - 1]);
          iNodeIdx += 1;
          break;
        case 1:
          if(vsPCN[iI - 1] == "UV")
          {
            pG->SetNodeName(iNodeIdx, "u");
            pG->SetNodeName(iNodeIdx + 1, "v");
            pG->SetNodeName(iNodeIdx + 2, "v+:" + vsPCN[iI] + "-");
            iNodeIdx++;
          }
          else
          {
            pG->SetNodeName(iNodeIdx, vsPCN[iI - 1]);

            if(vsPCN[iI] == "UV")
              pG->SetNodeName(iNodeIdx + 1, vsPCN[iI - 1] + "+:" + "u-");
            else
              pG->SetNodeName(iNodeIdx + 1, vsPCN[iI - 1] + "+:" + vsPCN[iI] + "-");
          }
          iNodeIdx += 2;
          break;
        case 2:
          if(vsPCN[iI - 1] == "UV")
          {
            pG->SetNodeName(iNodeIdx, "u");
            pG->SetNodeName(iNodeIdx + 1, "v");
            pG->SetNodeName(iNodeIdx + 2, "v+");
            pG->SetNodeName(iNodeIdx + 3, vsPCN[iI] + "-");
            iNodeIdx++;
          }
          else
          {
            pG->SetNodeName(iNodeIdx, vsPCN[iI - 1]);
            pG->SetNodeName(iNodeIdx + 1, vsPCN[iI - 1] + "+");

            if(vsPCN[iI] == "UV")
              pG->SetNodeName(iNodeIdx + 2, "u-");
            else
              pG->SetNodeName(iNodeIdx + 2, vsPCN[iI] + "-");
          }
          iNodeIdx += 3;
          break;
        case 3:
          if(vsPCN[iI - 1] == "UV")
          {
            pG->SetNodeName(iNodeIdx, "u");
            pG->SetNodeName(iNodeIdx + 1, "v");
            pG->SetNodeName(iNodeIdx + 2, "v+");
            pG->SetNodeName(iNodeIdx + 4, vsPCN[iI] + "-");
            iNodeIdx++;
          }
          else
          {
            pG->SetNodeName(iNodeIdx, vsPCN[iI - 1]);
            pG->SetNodeName(iNodeIdx + 1, vsPCN[iI - 1] + "+");
            if(vsPCN[iI] == "UV")
              pG->SetNodeName(iNodeIdx + 3, "u-");
            else
              pG->SetNodeName(iNodeIdx + 3, vsPCN[iI] + "-");
          }
          iNodeIdx += 4;
          break;
      }
    }
  }
}

//////////////////////////////////////////////////////////////////////////////
//
//  short ClosureHamPath::GenerateGraphs(string sFileName)
//
//////////////////////////////////////////////////////////////////////////////

/* 
    selected parts

    0)    x                 | x          --           UV | UV          --           y |                 y                   

    1)    s - x             | x    -  (x+:UV-)  -     UV | UV    -  (UV+:y-)  -     y |             y - t               

    2)    s - (x-) - x      | x - (x+)    -   (UV-) - UV | UV - (UV+)    -   (y-) - y |      y - (y-) - t        

    3)    s - // - (x-) - x | x - (x+) - // - (UV-) - UV | UV - (UV+) - // - (y-) - y | y - (y-) - // - t   

*/

short ClosureHamPath::GenerateGraphs(string sFileName)
{
  bool bIsHamiltonian = true;
  vector<string> vsPermutedCoNodes;
  list<Graph *> lGraph;
  string sSetFileName;
  strstream str;
  long lFileCounter = 0; 
  short iI;
  Graph * pG = NULL;
  short iMaxPartsNum = 2 + m_ssCoNodes.size(); // S,T + co nodes  + UV node
  short * piSelectedParts = new short[iMaxPartsNum];  
  short iNodesNum;
  long lNumOfNonHamiltonianGraphs = 0;
  short iConfigurationCounter = 1;
  ConditionSet * pSet;
  bool bIsGraphHamiltonian = true;
  short iPercent = 0;
  short iPercentNew;
  long liMaxGraphNum = 0;

  memset(piSelectedParts, 0, sizeof(short)*iMaxPartsNum);

/* 
  // onlyfor k5-5-full
  piSelectedParts[0] = 3;
  piSelectedParts[1] = 2;
  piSelectedParts[2] = 3;
  piSelectedParts[iMaxPartsNum-1] = 3;
*/

  // onlyfor k4-4-full

/*
  piSelectedParts[0] = 0;
  piSelectedParts[1] = 1;
  piSelectedParts[2] = 0;
  piSelectedParts[3] = 1;
  piSelectedParts[4]  = 0;
*/
  while(true) 
  {
    // generate size node configuration of graph 
    //==========================================================================
    iNodesNum = 0;
    for(iI = 0; iI < iMaxPartsNum; iI++)
      iNodesNum += piSelectedParts[iI] + 1;
  
    iNodesNum--; // you need not nodes for x in the first selected part and y in last selected part

    iNodesNum += 1; // NODE FOR v

    // for all permutations of co nodes
    while(PermuteCoNodes(&vsPermutedCoNodes))
    {
      // create new graph
      pG = new Graph(iNodesNum);
      ASSERT(pG);
      pG->SetNodeName(0, "s");
      pG->SetNodeName(iNodesNum - 1, "t");
      SetPermutedNodeNames(pG, &vsPermutedCoNodes, piSelectedParts, iMaxPartsNum);

      // prepare graph set 
      lGraph.clear();
      lGraph.push_back(pG);

      pSet = new ConditionSet(m_ConditionSet);
      ASSERT(pSet);

      pG->RemoveAllEdges();
      // add edges on the path between S and T
      for(iI = 0; iI < iNodesNum - 1; iI++)
        pG->AddEdge(iI, iI + 1);

      // remove edge between U and V
      pG->RemoveEdge("u", "v");
      
      //////////////////////////////////////////////////////
      //////////////////////////////////////////////////////
      //////////////////////////////////////////////////////
      /*
      str.clear();
      str << sFileName << setw(4) << setfill('0') << lFileCounter++;
      str >> sSetFileName;

      pG->Write(sSetFileName + ".bad.dot");

      continue;
      /**/
      //////////////////////////////////////////////////////
      //////////////////////////////////////////////////////
      //////////////////////////////////////////////////////

      // for this condition set generate graph
      if(pSet->AddRemovableEdges(pG) != RETURN_OK)
        continue;

      // pG->Write("k4-f-konfigurace.dot");

      iPercentNew = (int)((double)iConfigurationCounter / (pow(4,m_ssCoNodes.size() + 1)*4) * 100);
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

        switch(m_PathType)
        {
        case aPath: 
          bIsGraphHamiltonian = pG->IsHamiltonianPath();
          break;
        case sPath:
          bIsGraphHamiltonian = pG->IsHamiltonianRPath();
          break;
        case stPath:
          bIsGraphHamiltonian = pG->IsHamiltonianSTPath();
          break;
        }

        if(!bIsGraphHamiltonian)
        {
/*
          str.clear();
          str << sFileName << setw(4) << setfill('0') << lFileCounter;
          str >> sSetFileName;

          bIsHamiltonian = false;
          lNumOfNonHamiltonianGraphs++;
          pG->Write(sSetFileName + ".bad.dot");
*/
          bIsHamiltonian = false;
          lNumOfNonHamiltonianGraphs++;
          pG->Write(sFileName + ".bad.dot");
        }
#ifdef WRITE_OK_DOT
        else
        {
/*
          str.clear();
          str << sFileName << setw(4) << setfill('0') << lFileCounter;
          str >> sSetFileName;

          pG->Write(sSetFileName + ".dot");
*/
          pG->Write(sFileName + ".good.dot");
        }
#endif 
        delete pG;
      }

      delete pSet;
    }
    // prepare for new permutation
    vsPermutedCoNodes.clear();

    // test if all configartions were tested
    if(iNodesNum == 2 + m_ssCoNodes.size()*4 + 1 + 5) // 5 for co-node UV (remember UV are two nodes u and v)
      break;

    CombineGraphStates(piSelectedParts, iMaxPartsNum);

    iConfigurationCounter++;
  }

  delete [] piSelectedParts;

  switch(m_PathType)
  {
  case aPath: 
    cout << endl << endl << "Is hamiltonian path: " << bIsHamiltonian << endl;
    break;
  case sPath:
    cout << endl << endl << "Is hamiltonian R path: " << bIsHamiltonian << endl;
    break;
  case stPath:
    cout << endl << endl << "Is hamiltonian ST path: " << bIsHamiltonian << endl;
    break;
  }
  cout << "Number of non-hamiltonian graphs is: " << lNumOfNonHamiltonianGraphs << endl << endl ;

  return RETURN_OK;
}
