//////////////////////////////////////////////////////////////////////////////
//
//  Hamiltonian closure testing by Filip Jurcicek
//
//////////////////////////////////////////////////////////////////////////////

#include "GlobalDefine.h"
#include "ForbiddenGraphs.h"
#include "Graph.h"
#include "math.h"

//////////////////////////////////////////////////////////////////////////////
//
//  Closure::Closure(void)
//
//////////////////////////////////////////////////////////////////////////////

ForbiddenGraphs::ForbiddenGraphs(void): m_ConditionSet(&m_ssNodes, &m_sNodeU, &m_sNodeV)
{
  m_bUseUV = false;
  m_iDegree = 1;
  m_iMinNumOfConfigurationNodes = 0;
}

//////////////////////////////////////////////////////////////////////////////
//
//  ForbiddenGraphs::~ForbiddenGraphs(void)
//
//////////////////////////////////////////////////////////////////////////////

ForbiddenGraphs::~ForbiddenGraphs(void)
{
}

//////////////////////////////////////////////////////////////////////////////
//
//  short ForbiddenGraphs::Read(string sFileName)
//
//////////////////////////////////////////////////////////////////////////////

short ForbiddenGraphs::Read(string sFileName)
{
  fstream fin(sFileName.c_str(), ios::in);
  strstream str;
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

    // degree
    if(sLine.substr(0, sizeof("degree:") - 1) == "degree:")
    {
      sLine.erase(0, sizeof("degree:") - 1);

      str.clear();
      str << sLine;
      str >> m_iDegree;

      continue;
    }

    // use_uv
    if(sLine.substr(0, sizeof("use_uv:") - 1) == "use_uv:")
    {
      sLine.erase(0, sizeof("use_uv:") - 1);

      if(sLine == "false")
        m_bUseUV = false;
      else
        m_bUseUV = true;

      continue;
    }

    // min_num_of_configuration
    if(sLine.substr(0, sizeof("min_num_of_configuration_nodes:") - 1) == "min_num_of_configuration_nodes:")
    {
      sLine.erase(0, sizeof("min_num_of_configuration_nodes:") - 1);

      str.clear();
      str << sLine;
      str >> m_iMinNumOfConfigurationNodes;

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

    cout << endl;
    cout << "Erorr: Unrecognized command: " << sLine << 
      " in closure definition file: " << sFileName << endl << endl;
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
//  short ForbiddenGraphs::Print(void)
//
//////////////////////////////////////////////////////////////////////////////

short ForbiddenGraphs::Print(void)
{
  list<ConditionSet>::iterator i;
  set<string>::iterator ssIt; 
  
  cout << endl;

  cout << "nodes:  ";
  if(!m_ssNodes.empty())
  {
    ssIt = m_ssNodes.begin();
    cout << *ssIt;
    ssIt++;

    for(; ssIt != m_ssNodes.end(); ssIt++)
      cout << "," << *ssIt ;
  }
  cout << endl;

  cout << "path:                           " << m_sNodeU << "," << m_sNodeV << endl;
  cout << "degree:                         " << m_iDegree << endl;
  cout << "use_uv:                         " << m_bUseUV << endl;
  cout << "min_num_of_configuration_nodes: " << m_iMinNumOfConfigurationNodes << endl;

  cout << endl;

  m_ConditionSet.Print();

  cout << endl;

  return RETURN_OK;
}

//////////////////////////////////////////////////////////////////////////////
//
//  short ForbiddenGraphs::Parse(void)
//
//////////////////////////////////////////////////////////////////////////////

short ForbiddenGraphs::ParseClosure(void)
{
  while(!m_ConditionSet.Expand());

  return RETURN_OK;
}

//////////////////////////////////////////////////////////////////////////////
//
//  short ForbiddenGraphs::WriteConditionSets(string sFileName)
//
//////////////////////////////////////////////////////////////////////////////

short ForbiddenGraphs::WriteConditionSet(string sFileName)
{
  m_ConditionSet.Write(sFileName);

  return RETURN_OK;
}

//////////////////////////////////////////////////////////////////////////////
//
//  void ForbiddenGraphs::CombineGraphStates(short * piSelectedParts, short iMaxPartsNum)
//
//////////////////////////////////////////////////////////////////////////////

void ForbiddenGraphs::CombineGraphStates(short * piSelectedParts, short iMaxPartsNum)
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
//  void ForbiddenGraphs::PermuteCoNodes(vector<string> * pvsPermutedCoNodes)
//
//////////////////////////////////////////////////////////////////////////////

bool ForbiddenGraphs::PermuteCoNodes(vector<string> * pvsPermutedCoNodes)
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
//  void ForbiddenGraphs::SetPermutedNodeNames(Graph *pG, vector<string> * pvsPermutedCoNodes,
//                                     short * piSelectedParts, short iMaxPartsNum)
//
//////////////////////////////////////////////////////////////////////////////

void ForbiddenGraphs::SetPermutedNodeNames(Graph *pG, vector<string> * pvsPermutedCoNodes,
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
//  short ForbiddenGraphs::GenerateGraphs(string sFileName)
//
//////////////////////////////////////////////////////////////////////////////

/* 
    selected parts

    0)    u - x             | x          --          y |             y - v               

    1)    u - (x-) - x      | x    -  (x+:y-)  -     y |      y - (y-) - v        

    2)    u - // - (x-) - x | x - (x+)    -   (y-) - y | y - (y-) - // - v

    3)                      | x - (x+) - // - (y-) - y | 


*/

short ForbiddenGraphs::GenerateGraphs(string sFileName)
{
  vector<string> vsPermutedCoNodes;
  string sSetFileName;
  strstream str;
  long lFileCounter = 0; 
  long iI, iJ;
  Graph * pG = NULL;
  Graph * pNodesNeighbourhoodG = NULL;
  Graph * pReducedNodesNeighbourhoodG = NULL;
  Graph * pExceptionG = NULL;
  short iMaxPartsNum = 2 + m_ssCoNodes.size() - 1; // U,V + co nodes - 1
  short * piSelectedParts = new short[iMaxPartsNum];  
  short iNodesNum;
  long iConfigurationCounter = 1;
  ConditionSet * pSet;
  list<Graph *> lPossibleForbiddenGraphs;
  list<Graph *> lPossibleForbiddenGraphsG;
  list<Graph *> lExpandedPossibleForbiddenGraphs;
  list<Graph *> lForbiddenGraphs;
  list<Graph *>::iterator lIt, lJt;
  short iRet;
  bool bAlreadyExpanded;
  bool bInduced;
  short iPercent = 0;
  short iPercentNew;
  bool bCorrectClosure = true;

  memset(piSelectedParts, 0, sizeof(short)*iMaxPartsNum);

  // only for k4
/*
  piSelectedParts[0] = 2;
  piSelectedParts[1] = 3;
  piSelectedParts[2] = 2;
*/
  while(true) 
  {
    // generate size node configuration of graph 
    //==========================================================================
    iNodesNum = 0;
    for(iI = 0; iI < iMaxPartsNum; iI++)
      iNodesNum += piSelectedParts[iI] + 1;
  
    iNodesNum++;

    if(iNodesNum >= m_iMinNumOfConfigurationNodes)
      // for all permutations of co nodes
      while(PermuteCoNodes(&vsPermutedCoNodes))
      {
        // create new graph
        pG = new Graph(iNodesNum);
        ASSERT(pG);
        pG->SetNodeName(0, m_sNodeU);
        pG->SetNodeName(iNodesNum - 1, m_sNodeV);
        SetPermutedNodeNames(pG, &vsPermutedCoNodes, piSelectedParts, iMaxPartsNum);

        pSet = new ConditionSet(m_ConditionSet);
        ASSERT(pSet);

        pG->RemoveAllEdges();
        // add edges on the path between U and V
        for(iI = 0; iI < iNodesNum - 1; iI++)
          pG->AddEdge(iI, iI + 1);

        // for this condition set generate graph
        if(pSet->AddRemovableEdges(pG, true) != RETURN_OK)
          continue;
  /*
        pG->Write("a.dot");
  */
        if(!pG->IsHamiltonianCycle())
        {
          // isn't hamiltonian cycle there => we have first possible forbidden graphs

          // create m_ssNodes first neighbourhood
          //pG->Write("original.dot");
          pG->CreateNodesNeighbourhood(&pNodesNeighbourhoodG, m_ssNodes, m_iDegree, m_bUseUV);
          //pNodesNeighbourhoodG->Write("originalNeighbourhood.dot");

          // reduce first neighbourhood
          pNodesNeighbourhoodG->ReduceGraph(&pReducedNodesNeighbourhoodG, m_ssNodes);

          // pNodesNeighbourhoodG overbear some edges in graph
          // if we add any edge from this set, a new graph must be hamiltonian
          // or we have possible new forbidden graph. 
          //    a new possible forbidden graph overbear some edges in graph ... 
          // (test continue, we have tree)

          lPossibleForbiddenGraphs.clear();
          lPossibleForbiddenGraphsG.clear();
          lPossibleForbiddenGraphs.push_back(pReducedNodesNeighbourhoodG);
          lPossibleForbiddenGraphsG.push_back(pG);

          //cout << endl;

          while(!lPossibleForbiddenGraphs.empty())
          {
  /*
            cout << "                                          \r";
            cout << "Size of PFGs: " << lPossibleForbiddenGraphs.size();
            cout << " Size of ExpandedPFGs: " << lExpandedPossibleForbiddenGraphs.size();
  */  
            // get forbidden graph which will be tested if all overbear edges 
            // make graph hamiltonian
            pReducedNodesNeighbourhoodG = *lPossibleForbiddenGraphs.begin();
            lPossibleForbiddenGraphs.pop_front();

            pG = *lPossibleForbiddenGraphsG.begin();
            lPossibleForbiddenGraphsG.pop_front();

            // test if I need expand this possible forbidden graph
            bAlreadyExpanded = false;
            iI = 0;

            for(lIt = lExpandedPossibleForbiddenGraphs.begin(); lIt != lExpandedPossibleForbiddenGraphs.end(); lIt++, iI++)
              // there I cannot use condition of induction,
              // because I prune only expansion of already expanded graphs => equal condition is need !!!
              // but two equal graphs can have two different incidence matrixs
              // I will must improve comparison of two graphs (maybe some sort of incidence matrix)   ###
              if( **lIt == *pReducedNodesNeighbourhoodG) 
              {
                // the same type of possible forbidden graph was already expanded
                bAlreadyExpanded = true;
                break;
              }

            //cout << "AE" << iI << endl;

            if(bAlreadyExpanded)
            {
              // don't expand this possible forbidden graph, because is already expanded

              // you can delete this graph
              delete pReducedNodesNeighbourhoodG;
              delete pG;

              continue;
            }

            ///////////////////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////////

            iPercentNew = (int)((double)iConfigurationCounter / (3*3*pow(4,m_ssCoNodes.size() - 1)) * 100);
            if(iPercent < iPercentNew)
            {
              iPercent = iPercentNew;
              cout << "Processed " << iPercent << " % of all graphs.";
              cout << " PFG number: " << lFileCounter << endl;
            }

            lFileCounter++;

            /*
            str.clear();
            str << sFileName << setw(4) << setfill('0') << lFileCounter;
            str >> sSetFileName;

            pReducedNodesNeighbourhoodG->Write(sSetFileName + ".pfg.dot");
            pG->Write(sSetFileName + ".G.pfg.dot");
            /**/
  /*
            pReducedNodesNeighbourhoodG->Write("b.dot");
            pG->Write("c.dot");
  */
            ///////////////////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////////

            lExpandedPossibleForbiddenGraphs.push_back(pReducedNodesNeighbourhoodG);

            // cout  << endl << lFileCounter - 1;
            /*
            Graph NullGrap(1);  
            NullGrap.SetNodeName(0, "NullNode");
            NullGrap.Write("k4-f-pfg.dot");
          
            pReducedNodesNeighbourhoodG->Write("k4-f-pfg.dot");
  */

            for(iI = 0; iI < pReducedNodesNeighbourhoodG->GetNumOfNodes(); iI++)
              for(iJ = iI + 1; iJ < pReducedNodesNeighbourhoodG->GetNumOfNodes(); iJ++)
                if(!pReducedNodesNeighbourhoodG->Edge(iI, iJ))
                {
                  // iI - iJ is overbeared edge, which cause that graph is hamiltonian or 
                  // we have new possible forbidden graph
                  long iX = pReducedNodesNeighbourhoodG->GetNodeId(iI);
                  long iY = pReducedNodesNeighbourhoodG->GetNodeId(iJ);
                
                  // cout << "E(" << iX  << iI << "," << iY << iJ << ")";

                  iRet = pG->AddEdgeById(iX, iY); 
                  if(iRet != RETURN_OK)
                    return iRet;
                
                  if(!pG->IsHamiltonianCycle())
                  { 
                    if( (m_ssNodes.find(pReducedNodesNeighbourhoodG->GetNodeName(iI)) != m_ssNodes.end()) &&
                        (m_ssNodes.find(pReducedNodesNeighbourhoodG->GetNodeName(iJ)) != m_ssNodes.end()) )
                    {
                      // this is edge of structure and cannot be added
                      bCorrectClosure = false;

                      if(pExceptionG == NULL)
                      {
                        pExceptionG = new Graph(*pG);
                      }
                      else
                      {
                        delete pExceptionG;
                        pExceptionG = new Graph(*pG);
                      }

                    }
                    else
                    {
                      //pG->Write("test.dot");

                      // we have a new possible forbidden graph
                      Graph *pNewPossibleForbiddenGraph = new Graph(*pReducedNodesNeighbourhoodG);
                      Graph *pNewG = new Graph(*pG);
    
                      pNewPossibleForbiddenGraph->AddEdge(iI, iJ);

                      lPossibleForbiddenGraphs.push_back(pNewPossibleForbiddenGraph);
                      lPossibleForbiddenGraphsG.push_back(pNewG);
                      //cout << "!";
                    }
                  }
                  //else
                  //  cout << "*";

                  // pG->Write("k4-f-pfg.dot");
                  pG->RemoveEdgeById(iX, iY);
                }

            //cout << endl;

            delete pG;
          }

          delete pNodesNeighbourhoodG;
        }
        else 
          delete pG;

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

  cout << endl << endl;
  
  cout << "PFG number is: " << lExpandedPossibleForbiddenGraphs.size() << endl;

  cout << endl << endl;

  // select only forbidden graphs
  // save forbidden graps
  iPercent = 0;
  lFileCounter = 0;
  iI = 0;
  for(lIt = lExpandedPossibleForbiddenGraphs.begin(); lIt != lExpandedPossibleForbiddenGraphs.end(); lIt++, iI++)
  {
    bInduced = false;
    for(lJt = lForbiddenGraphs.begin(); lJt != lForbiddenGraphs.end(); lJt++)
      if( (*lIt)->IsInduced(**lJt, m_ssCoNodes) )
      {
        // lIt possible forbidden graph is already forbidden by lJt forbiden graph
        bInduced = true;
        break;
      }

    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////

    iPercentNew = (int)((double)iI / (lExpandedPossibleForbiddenGraphs.size() - 1) * 100);
    if(iPercent < iPercentNew)
    {
      iPercent = iPercentNew;
      cout << "Processed " << iPercent << " % of all PFGs.";
      cout << " FG number: " << lFileCounter << endl;
    }
    
    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////

    if(!bInduced)
    {
      lForbiddenGraphs.push_back(*lIt);
      lFileCounter++;

      cout << "Processed PFG number: " << iI << " FG number: " << lFileCounter << endl;
      (*lIt)->Write(sFileName + ".fg.dot");
    }
  }

  cout << endl << endl;

  for(lIt = lExpandedPossibleForbiddenGraphs.begin(); lIt != lExpandedPossibleForbiddenGraphs.end(); lIt++)
    delete *lIt;

  
  cout << "Closure is correct: " << bCorrectClosure << endl << endl;

  if(pExceptionG)
    pExceptionG->Write(sFileName + ".exc.dot");

  delete [] piSelectedParts;

  return RETURN_OK;
}



  /*
  // make cross induction of graphs
  lExpandedPossibleForbiddenGraphs.clear();

  lFileCounter = 0;
  iI = 0;
  for(lIt = lForbiddenGraphs.begin(); lIt != lForbiddenGraphs.end(); lIt++, iI++)
  {
    bInduced = false;
    for(lJt = lForbiddenGraphs.begin(); lJt != lForbiddenGraphs.end(); lJt++)
      if( !(**lIt == **lJt) )
        if( (*lIt)->IsInduced(**lJt) )
        {
          // lIt possible forbidden graph is already forbidden by lJt forbiden graph
          bInduced = true;
          break;
        }

    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////

    cout << "                                          \r";
    cout << "Processed " << (int)((double)iI / 
      (lForbiddenGraphs.size() - 1) * 100) << " % of all FGs.";
    cout << " Cross induction FG number: " << lFileCounter;
    
    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////

    if(!bInduced)
    {
      lExpandedPossibleForbiddenGraphs.push_back(*lIt);
      
      str.clear();
      str << sFileName << setw(4) << setfill('0') << lFileCounter++;
      str >> sSetFileName;

      (*lIt)->Write(sSetFileName + ".cfg.dot");
    }
  }

  cout << endl << endl;
  */
