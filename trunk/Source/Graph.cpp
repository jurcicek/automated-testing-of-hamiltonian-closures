//////////////////////////////////////////////////////////////////////////////
//
//  Hamiltonian closure testing by Filip Jurcicek
//
//////////////////////////////////////////////////////////////////////////////

#include "GlobalDefine.h"
#include "Graph.h"

//#define DEBUD_HAMILTON
#define NO_NODE  (-1)

long Graph::m_liIdCounter = 0;

//////////////////////////////////////////////////////////////////////////////
//
//  Graph::Graph(short iNumOfNodes)
//
//////////////////////////////////////////////////////////////////////////////

Graph::Graph(short iNumOfNodes)
{
  short iI;

  m_pbIncidentMatrix = new bool[iNumOfNodes * iNumOfNodes];
  ASSERT(m_pbIncidentMatrix);
  memset(m_pbIncidentMatrix, 0, iNumOfNodes * iNumOfNodes * sizeof(bool));

  m_psNodeName = new string[iNumOfNodes];
  ASSERT(m_psNodeName);

  m_pliNodeId = new long[iNumOfNodes];
  ASSERT(m_pliNodeId);

  m_piHamiltonNodes = new short[iNumOfNodes];
  ASSERT(m_piHamiltonNodes);
  memset(m_piHamiltonNodes, 0, iNumOfNodes * sizeof(short));

  m_iNumOfNodes = iNumOfNodes;
  m_bIsHamilton = false;

  for(iI = 0; iI < m_iNumOfNodes; iI++)
    m_pliNodeId[iI] = m_liIdCounter++;

  if(m_liIdCounter > 256*256*256*32)
    m_liIdCounter = 0;
}


//////////////////////////////////////////////////////////////////////////////
//
//  Graph::Graph(Graph & G)
//
//////////////////////////////////////////////////////////////////////////////

Graph::Graph(Graph & G)
{
  short iI;

  m_iNumOfNodes = G.m_iNumOfNodes;
  m_bIsHamilton = G.m_bIsHamilton;

  m_pbIncidentMatrix = new bool[m_iNumOfNodes * m_iNumOfNodes];
  ASSERT(m_pbIncidentMatrix);
  memcpy(m_pbIncidentMatrix, G.m_pbIncidentMatrix, m_iNumOfNodes * m_iNumOfNodes * sizeof(bool));

  m_psNodeName = new string[m_iNumOfNodes];
  ASSERT(m_psNodeName);
  for(iI = 0; iI < m_iNumOfNodes; iI++)
    m_psNodeName[iI] = G.m_psNodeName[iI];

  m_pliNodeId = new long[m_iNumOfNodes];
  ASSERT(m_pliNodeId);
  memcpy(m_pliNodeId, G.m_pliNodeId, m_iNumOfNodes * sizeof(long));

  m_piHamiltonNodes = new short[m_iNumOfNodes];
  ASSERT(m_piHamiltonNodes);
  memcpy(m_piHamiltonNodes, G.m_piHamiltonNodes, m_iNumOfNodes * sizeof(short));
}

//////////////////////////////////////////////////////////////////////////////
//
//  Graph::~Graph(void)
//
//////////////////////////////////////////////////////////////////////////////

Graph::~Graph(void)
{
  delete [] m_pbIncidentMatrix;
  delete [] m_psNodeName;
  delete [] m_piHamiltonNodes;
  delete [] m_pliNodeId;
}

//////////////////////////////////////////////////////////////////////////////
//
//  void Graph::SetNodeName(short iNode, string sName)
//
//////////////////////////////////////////////////////////////////////////////

void Graph::SetNodeName(short iNode, string sName)
{
  ASSERT(iNode >= 0 && iNode < m_iNumOfNodes);
  m_psNodeName[iNode] = sName;
}

//////////////////////////////////////////////////////////////////////////////
//
// void Graph::SetNodeId(short iNode, long liNodeId)
//
//////////////////////////////////////////////////////////////////////////////

void Graph::SetNodeId(short iNode, long liNodeId)
{
  ASSERT(iNode >= 0 && iNode < m_iNumOfNodes);
  m_pliNodeId[iNode] = liNodeId;
}

//////////////////////////////////////////////////////////////////////////////
//
//  short Graph::FindNode(string sNode)
//
//////////////////////////////////////////////////////////////////////////////

short Graph::FindNode(string sNode)
{
  short iNode = NO_NODE;
  short iI;
  string sBegin, sRest;

  for(iI = 0; iI < m_iNumOfNodes; iI++)
  {
    if(m_psNodeName[iI] == sNode)
    {
      iNode = iI;
      break;
    }
    else
      if(Split(m_psNodeName[iI], ":", sBegin, sRest))
      {
        if(sBegin == sNode)
        {
          iNode = iI;
          break;
        }
        if(sRest == sNode)
        {
          iNode = iI;
          break;
        }
      }
  }

  return iNode;
}

//////////////////////////////////////////////////////////////////////////////
//
//  short Graph::FindNodeById(long liNodeId)
//
//////////////////////////////////////////////////////////////////////////////

short Graph::FindNodeById(long liNodeId)
{
  short iI;

  for(iI = 0; iI < m_iNumOfNodes; iI++)
    if(m_pliNodeId[iI] == liNodeId)
      return iI;

  return NO_NODE;
}

//////////////////////////////////////////////////////////////////////////////
//
//  short Graph::CreateNodesNeighbourhood(Graph ** ppNodesNeighbourhoodG, set<string> &ssNodes, 
//                                      short iDegree, bool bUseUV)
//
//////////////////////////////////////////////////////////////////////////////

short Graph::CreateNodesNeighbourhood(Graph ** ppNodesNeighbourhoodG, set<string> &ssNodes, 
                                      short iDegree, bool bUseUV)
{
  short *piNeighbourhoodNodes = new short[m_iNumOfNodes];
  set<string>::iterator sIt;
  short iNode;
  short iI, iJ, iK;
  Graph * pG;

  ASSERT(piNeighbourhoodNodes);  
  memset(piNeighbourhoodNodes, -1, m_iNumOfNodes * sizeof(short));

  // find all set nodes
  for(sIt = ssNodes.begin(); sIt != ssNodes.end(); sIt++)
  {
    iNode = FindNode(*sIt);
    if(iNode == NO_NODE)
      return RETURN_FAIL;

    piNeighbourhoodNodes[iNode] = 0;
  }

  // find all connected nodes up to iDegree
  for(iI = 0; iI < iDegree; iI++)
    for(iJ = 0; iJ < m_iNumOfNodes; iJ++)
      if(piNeighbourhoodNodes[iJ] == iI)        // use only last added countour line
        for(iK = 0; iK < m_iNumOfNodes; iK++)
        {
          if(!bUseUV)
          {
            //don't use nodes which go only from U or V
            if( (iJ == 0) || (iK == 0) )
              continue; // U node

            if( (iJ == m_iNumOfNodes - 1) || (iK == m_iNumOfNodes - 1) )
              continue; // V node
          }

          if( (iJ != iK) && Edge(iJ, iK) && (piNeighbourhoodNodes[iK] < 0) ) 
            // exclude nodes (piNeighbourhoodNodes[iK] < 0) which was already added
            // no edges will be in the future added, although is a edge there
            piNeighbourhoodNodes[iK] = iI + 1;
        }

  // add all induced edges which go from one round of neighbourhood to another neighbourhood
  // the number of the contour line is in piNeighbourhoodNodes
  /*

         / - - - - \

        |  / - - \  |
        |  |     |  |
        |  |     |  |
        |  \ - - /  |

         \ - - - - /
  */

  pG = new Graph(m_iNumOfNodes);
  ASSERT(pG);

  for(iI = 0; iI < m_iNumOfNodes; iI++)
    for(iJ = iI + 1; iJ < m_iNumOfNodes; iJ++)
      if( piNeighbourhoodNodes[iI] >= 0 && piNeighbourhoodNodes[iJ] >= 0 && Edge(iI, iJ) )
        pG->AddEdge(iI, iJ);

  // set set nodes names
  for(iI = 0; iI < m_iNumOfNodes; iI++)
  {
    pG->SetNodeName(iI, GetNodeName(iI));
    pG->SetNodeId(iI, GetNodeId(iI));
  }

  *ppNodesNeighbourhoodG = pG;

  delete [] piNeighbourhoodNodes;

  return RETURN_OK;
}

//////////////////////////////////////////////////////////////////////////////
//
//  short Graph::ReduceGraph(Graph ** ppG, set<string> &ssNodes)
//
//  Remove from graph unused nodes. => Remove nodes with degree zero.
//  In reduced graph must always stay nodes of structure (ssNodes)
//
//////////////////////////////////////////////////////////////////////////////

short Graph::ReduceGraph(Graph ** ppG, set<string> &ssNodes)
{
  short *piNewNodes = new short[m_iNumOfNodes];
  short *piNodesDegree = new short[m_iNumOfNodes];
  short iI, iJ, iK;
  set<string>::iterator sIt;
  Graph * pG;

  // compute degree of all nodes
  for(iI = 0; iI < m_iNumOfNodes; iI++)
  {
    piNodesDegree[iI] = 0;

    for(iJ = 0; iJ < m_iNumOfNodes; iJ++)
      if( (iI != iJ) && Edge(iI, iJ) )
        piNodesDegree[iI]++;
  }

  // compute a number of nodes of a new graph
  // assign numbers of new nodes to the selected nodes
  iK = 0;
  for(iI = 0; iI < m_iNumOfNodes; iI++)
    if((piNodesDegree[iI] > 0) || (ssNodes.find(GetNodeName(iI)) != ssNodes.end()))
      piNewNodes[iI] = iK++;
    else
      piNewNodes[iI] = -1;

  pG = new Graph(iK);

  // set set nodes names
  // set set nodes Ids
  for(iI = 0; iI < m_iNumOfNodes; iI++)
    if(piNewNodes[iI] >=0 )
    {
      pG->SetNodeName(piNewNodes[iI], GetNodeName(iI));
      pG->SetNodeId(piNewNodes[iI], GetNodeId(iI));
    }

  // add all edges
  for(iI = 0; iI < m_iNumOfNodes; iI++)
    for(iJ = iI + 1; iJ < m_iNumOfNodes; iJ++)
      if(Edge(iI, iJ))
        pG->AddEdge(piNewNodes[iI], piNewNodes[iJ]);  

  *ppG = pG;

  delete [] piNewNodes;
  delete [] piNodesDegree;

  return RETURN_OK;
}

//////////////////////////////////////////////////////////////////////////////
//
//  short Graph::GetNeighbourhood(short iNode, set<short> & siNeighbourhood)
//
//////////////////////////////////////////////////////////////////////////////

short Graph::GetNeighbourhood(short iNode, set<short> & siNeighbourhood)
{
  short iI;
  
  for(iI = 0; iI < m_iNumOfNodes; iI++)
    if( (iNode != iI) && Edge(iNode, iI) )
      siNeighbourhood.insert(iI);

  return RETURN_OK;
}

//////////////////////////////////////////////////////////////////////////////
//
//  short Graph::AddEdge(short iSourceNode, short iDestinationNode)
//
//////////////////////////////////////////////////////////////////////////////

short Graph::AddEdge(short iSourceNode, short iDestinationNode)
{
  Edge(iSourceNode, iDestinationNode) = true;

  return RETURN_OK;
}

//////////////////////////////////////////////////////////////////////////////
//
//  short Graph::AddEdge(string sSourceNode, string sDestinationNode)
//
//////////////////////////////////////////////////////////////////////////////

short Graph::AddEdge(string sSourceNode, string sDestinationNode)
{
  short iSourceNode;
  short iDestinationNode;

  iSourceNode = FindNode(sSourceNode);
  iDestinationNode = FindNode(sDestinationNode); 
  
  if( (iSourceNode == -1) || (iDestinationNode == -1) )
    return RETURN_BAD_NODE;

  if(Edge(iSourceNode, iDestinationNode))
    return RETURN_EXIST;

  Edge(iSourceNode, iDestinationNode) = true;

  return RETURN_OK;
}

//////////////////////////////////////////////////////////////////////////////
//
//  short Graph::AddEdgeById(short iSourceNodeId, short iDestinationNodeId)
//
//////////////////////////////////////////////////////////////////////////////

short Graph::AddEdgeById(short iSourceNodeId, short iDestinationNodeId)
{
  short iSourceNode;
  short iDestinationNode;

  iSourceNode = FindNodeById(iSourceNodeId);
  iDestinationNode = FindNodeById(iDestinationNodeId); 
  
  if( (iSourceNode == -1) || (iDestinationNode == -1) )
    return RETURN_BAD_NODE;

  if(Edge(iSourceNode, iDestinationNode))
    return RETURN_EXIST;

  Edge(iSourceNode, iDestinationNode) = true;

  return RETURN_OK;
}


//////////////////////////////////////////////////////////////////////////////
//
//  short Graph::RemoveEdge(short iSourceNode, short iDestinationNode)
//
//////////////////////////////////////////////////////////////////////////////

short Graph::RemoveEdge(short iSourceNode, short iDestinationNode)
{
  Edge(iSourceNode, iDestinationNode) = false;

  m_bIsHamilton = false;

  return RETURN_OK;
}

//////////////////////////////////////////////////////////////////////////////
//
//  short Graph::RemoveEdge(string sSourceNode, string sDestinationNode)
//
//////////////////////////////////////////////////////////////////////////////

short Graph::RemoveEdge(string sSourceNode, string sDestinationNode)
{
  short iSourceNode;
  short iDestinationNode;

  iSourceNode = FindNode(sSourceNode);
  iDestinationNode = FindNode(sDestinationNode); 
  
  if( (iSourceNode == -1) || (iDestinationNode == -1) )
    return RETURN_BAD_NODE;

  Edge(iSourceNode, iDestinationNode) = false;

  return RETURN_OK;
}

//////////////////////////////////////////////////////////////////////////////
//
//  short Graph::RemoveEdgeById(short iSourceNodeId, short iDestinationNodeId)
//
//////////////////////////////////////////////////////////////////////////////

short Graph::RemoveEdgeById(short iSourceNodeId, short iDestinationNodeId)
{
  short iSourceNode;
  short iDestinationNode;

  iSourceNode = FindNodeById(iSourceNodeId);
  iDestinationNode = FindNodeById(iDestinationNodeId); 
  
  if( (iSourceNode == -1) || (iDestinationNode == -1) )
    return RETURN_BAD_NODE;

  Edge(iSourceNode, iDestinationNode) = false;

  return RETURN_OK;
}

//////////////////////////////////////////////////////////////////////////////
//
//  short Graph::RemoveAllEdges(void)
//
//////////////////////////////////////////////////////////////////////////////

short Graph::RemoveAllEdges(void)
{
  short iI, iJ;

  for(iI = 0; iI < m_iNumOfNodes - 1; iI++)
    for(iJ = iI + 1; iJ < m_iNumOfNodes; iJ++)
      Edge(iI, iJ) = false;

  return RETURN_OK;
}

//////////////////////////////////////////////////////////////////////////////
//
//  bool Graph::operator == (Graph & G)
//
//////////////////////////////////////////////////////////////////////////////

bool Graph::operator == (Graph & G)
{
  short iI, iJ;

  if(m_iNumOfNodes != G.m_iNumOfNodes)
    return false;

  for(iI = 0; iI < m_iNumOfNodes - 1; iI++)
    for(iJ = iI + 1; iJ < m_iNumOfNodes; iJ++)
      if(Edge(iI, iJ) != G.Edge(iI, iJ) )
        return false;

  return true;
}


//////////////////////////////////////////////////////////////////////////////
//
//  bool Graph::IsHamiltonianCycle(void)
//
//
//  Efficiency on Celeron 375MHz is 500 graphs with 20 nodes in a second
//
//////////////////////////////////////////////////////////////////////////////

bool Graph::IsHamiltonianCycle(void)
{
  short iLastHamiltonNode = 0;
  short iI, iJ;
  short iStartNode;
  bool bIsHamiltonNode;

  ASSERT(m_iNumOfNodes >= 3);

  iStartNode = 0;
  m_bIsHamilton = false;
  memset(m_piHamiltonNodes, 0, m_iNumOfNodes * sizeof(short));

#ifdef DEBUD_HAMILTON
  cout << " 0 ";
#endif

  while(true)
  {
    // for all edges from last node
    for(iI = iStartNode; iI < m_iNumOfNodes; iI++)
    {
      iStartNode = 0;

      if(m_piHamiltonNodes[iLastHamiltonNode] == iI)
        continue;

      if(Edge(m_piHamiltonNodes[iLastHamiltonNode], iI))
      {
        // which don't go to the hamilton node
        bIsHamiltonNode = false;
        for(iJ = 0; iJ <= iLastHamiltonNode; iJ++)
          if(m_piHamiltonNodes[iJ] == iI)
          {
            bIsHamiltonNode = true;
            break;
          }

        if(bIsHamiltonNode)
          continue;

        m_piHamiltonNodes[++iLastHamiltonNode] = iI;
        iI = 0;

#ifdef DEBUD_HAMILTON
        cout << setw(2) << m_piHamiltonNodes[iLastHamiltonNode] << " ";
#endif
        // find cycle of length five (zero based index)
        if(iLastHamiltonNode == m_iNumOfNodes - 1)
          break;
      }
    }

#ifdef DEBUD_HAMILTON
    cout << endl;
#endif

    // you found path. is cycle and is over all nodes?
    if((iLastHamiltonNode == m_iNumOfNodes - 1) && Edge(0, m_piHamiltonNodes[iLastHamiltonNode]))
    {
      m_bIsHamilton = true;
      break;
    }
    else
    {
      iLastHamiltonNode--;
      // find next node with greater index
      while(iLastHamiltonNode >= 0)
      {
        iStartNode = 0;
        for(iI = m_piHamiltonNodes[iLastHamiltonNode + 1] + 1; iI < m_iNumOfNodes; iI++)
        {
          if(m_piHamiltonNodes[iLastHamiltonNode] == iI)
            continue;

          if(Edge(m_piHamiltonNodes[iLastHamiltonNode], iI))
          {
            // which don't go to the hamilton node
            bIsHamiltonNode = false;
            for(iJ = 0; iJ <= iLastHamiltonNode; iJ++)
              if(m_piHamiltonNodes[iJ] == iI)
              {
                bIsHamiltonNode = true;
                break;
              }

            if(bIsHamiltonNode)
              continue;

            iStartNode = iI;
    
            break;
          }
        }

        if(iStartNode)
          break;

        iLastHamiltonNode--;
      }

      if(iLastHamiltonNode == -1)
        break;

#ifdef DEBUD_HAMILTON
      for(iJ = 0; iJ <= iLastHamiltonNode; iJ++)
        cout << setw(2) << m_piHamiltonNodes[iJ] << " ";
#endif
    }
  }

  return m_bIsHamilton;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

bool Graph::HamPathRec(short iIdx, bool bTPath)
{
  static short iLastHamiltonNode = 0;
  bool bIsHamiltonNode;
  short iI, iJ;

  if(iLastHamiltonNode == m_iNumOfNodes - 1)
    if(!bTPath)
      return true;
    else
      return m_piHamiltonNodes[iLastHamiltonNode] == m_iNumOfNodes -1;

  for(iI = 0; iI < m_iNumOfNodes; iI++)
  {
    if(iIdx == iI)
      continue;

    if(Edge(iIdx, iI))
    {
      // we can gou through this edge
      if(Edge(m_piHamiltonNodes[iLastHamiltonNode], iI))
      {
        // which don't go to the hamilton node
        bIsHamiltonNode = false;
        for(iJ = 0; iJ <= iLastHamiltonNode; iJ++)
          if(m_piHamiltonNodes[iJ] == iI)
          {
            bIsHamiltonNode = true;
            break;
          }

        if(bIsHamiltonNode)
          continue;

        m_piHamiltonNodes[++iLastHamiltonNode] = iI;

#ifdef DEBUD_HAMILTON
        cout << setw(2) << m_piHamiltonNodes[iLastHamiltonNode] << " ";
#endif
        if(HamPathRec(iI, bTPath))
        {
          --iLastHamiltonNode;
          return true;
        }
        else
        {
          --iLastHamiltonNode;
#ifdef DEBUD_HAMILTON
          cout << endl;
          for(iJ = 0; iJ <= iLastHamiltonNode; iJ++)
            cout << setw(2) << m_piHamiltonNodes[iJ] << " ";
#endif
        }
      }
    }
  }

  return false;
}

//////////////////////////////////////////////////////////////////////////////
//
//  bool Graph::IsHamiltonianPath(void)
//
//////////////////////////////////////////////////////////////////////////////

bool Graph::IsHamiltonianPath(void)
{
  short iI;
  ASSERT(m_iNumOfNodes >= 3);

  for(iI = 0; iI < m_iNumOfNodes; iI++)
  {
#ifdef DEBUD_HAMILTON
    cout << setw(2) << iI << " ";
#endif
    m_piHamiltonNodes[0] = iI;
    m_bIsHamilton = HamPathRec(iI, false);

    if(m_bIsHamilton)
      return m_bIsHamilton;
  }

  return false;
}

//////////////////////////////////////////////////////////////////////////////
//
//  bool Graph::IsHamiltonianRPath(void)
//
//////////////////////////////////////////////////////////////////////////////

bool Graph::IsHamiltonianRPath(void)
{
  short iI;

  m_bIsHamilton = false;

  iI = 0;
#ifdef DEBUD_HAMILTON
  cout << setw(2) << iI << " ";
#endif
  m_piHamiltonNodes[0] = iI;
  if(!HamPathRec(iI, false))
    return false;

  iI = m_iNumOfNodes - 1;
#ifdef DEBUD_HAMILTON
  cout << setw(2) << iI << " ";
#endif
  m_piHamiltonNodes[0] = iI;
  if(!HamPathRec(iI, false))
    return false;

  m_bIsHamilton = true;

  return true;
}

//////////////////////////////////////////////////////////////////////////////
//
//  bool Graph::IsHamiltonianSTPath(void)
//
//////////////////////////////////////////////////////////////////////////////

bool Graph::IsHamiltonianSTPath(void)
{
  m_piHamiltonNodes[0] = 0;
  m_bIsHamilton = HamPathRec(0, true);
  return m_bIsHamilton;
}

//////////////////////////////////////////////////////////////////////////////
//
//  bool Graph::IsInduced(Graph & G, set<string> &ssCoNodes)
//
//  Test if graph G is induced in this graph. Induction from Cada.
//
//////////////////////////////////////////////////////////////////////////////

bool Graph::IsInduced(Graph & G, set<string> &ssCoNodes)
{
  short iI, iJ;
  short * piSelectedNodes = new short[G.m_iNumOfNodes];
  Graph NewG(G.m_iNumOfNodes);
  bool bCombinationOk;

  if(m_iNumOfNodes < G.m_iNumOfNodes)
  {
    delete piSelectedNodes;
    return false;
  }

  // test for corespondence of u - u and v - v 

  ////////////////////////////////////////////////////////////

  // initialize combination
  for(iI = 0; iI < G.m_iNumOfNodes; iI++)
    piSelectedNodes[iI] = iI;

  piSelectedNodes[G.m_iNumOfNodes - 1] = m_iNumOfNodes - 1;

  /*
  cout  << endl << "COMStart: ";
  cout  << endl << "COM: ";
  for(iI = 0; iI < G.m_iNumOfNodes; iI++)
    cout << setw(3) << piSelectedNodes[iI];
  */
  while(true)
  {
    NewG.RemoveAllEdges();

    for(iI = 0; iI < G.m_iNumOfNodes; iI++)
      for(iJ = iI + 1; iJ < G.m_iNumOfNodes; iJ++)
        if(Edge(piSelectedNodes[iI], piSelectedNodes[iJ]))
          NewG.AddEdge(iI, iJ);

    bool bXYNodesOk = true;
    for(iI = 1; iI < G.m_iNumOfNodes - 1; iI++)
    {
      bool bGXY = ssCoNodes.find(G.GetNodeName(iI)) != ssCoNodes.end();
      bool bXY = ssCoNodes.find(GetNodeName(piSelectedNodes[iI])) != ssCoNodes.end();

      if( (!bGXY && bXY)  || (bGXY && !bXY) ) 
      {
        // no appropiate "ordering" of co-nodes
        bXYNodesOk = false;
        break;
      }
    }       

    if( bXYNodesOk && (NewG == G) )
    {
      // cout << " Induced ";
      delete piSelectedNodes;

      return true;
    }

    // get next combination - don't combine first and last nodes (they are U and V)
    //   only if we should not end combination

    bCombinationOk = piSelectedNodes[G.m_iNumOfNodes - 1] - 1 == piSelectedNodes[1];
    for(iI = 1; iI < G.m_iNumOfNodes - 2; iI++)
        if( (piSelectedNodes[iI] - piSelectedNodes[iI + 1]) != 1)
        {
          bCombinationOk = false;

          break;
        }

    if(bCombinationOk)
      break;

    do
    {
      /*
      cout << endl << "COMPrev: ";
      for(iI = 0; iI < G.m_iNumOfNodes; iI++)
        cout << setw(3) << piSelectedNodes[iI];
      */

      iI = G.m_iNumOfNodes - 2; // we don't combine last node
      while(iI >= 1) // we don't combine first node
      {
        if(piSelectedNodes[iI] == m_iNumOfNodes - 2) // we don't combine last node
        {
          piSelectedNodes[iI] = 1; // we don't combine first node
          iI--;
          continue;
        }
        else
        {
          piSelectedNodes[iI]++;
          break;
        }
      }

      bCombinationOk = true;
      for(iI = 0; iI < G.m_iNumOfNodes; iI++)
        for(iJ = iI + 1; iJ < G.m_iNumOfNodes; iJ++)
          if(piSelectedNodes[iI] == piSelectedNodes[iJ])
          {
            bCombinationOk = false;

            iI = G.m_iNumOfNodes;
            break;
          }
    }
    while(!bCombinationOk);

    /*
    cout << endl << "COM: ";
    for(iI = 0; iI < G.m_iNumOfNodes; iI++)
      cout << setw(3) << piSelectedNodes[iI];
    */
  }

  // test for corespondence of u - v and v - u 
  ////////////////////////////////////////////////////////////

  // initialize combination
  for(iI = 0; iI < G.m_iNumOfNodes; iI++)
    piSelectedNodes[iI] = iI;

  piSelectedNodes[G.m_iNumOfNodes - 1] = 0;
  piSelectedNodes[0] = m_iNumOfNodes - 1;

  /*
  cout  << endl << "COMStart: ";
  cout  << endl << "COM: ";
  for(iI = 0; iI < G.m_iNumOfNodes; iI++)
    cout << setw(3) << piSelectedNodes[iI];
  */
  while(true)
  {
    NewG.RemoveAllEdges();

    for(iI = 0; iI < G.m_iNumOfNodes; iI++)
      for(iJ = iI + 1; iJ < G.m_iNumOfNodes; iJ++)
        if(Edge(piSelectedNodes[iI], piSelectedNodes[iJ]))
          NewG.AddEdge(iI, iJ);

    bool bXYNodesOk = true;
    for(iI = 1; iI < G.m_iNumOfNodes - 1; iI++)
    {
      bool bGXY = ssCoNodes.find(G.GetNodeName(iI)) != ssCoNodes.end();
      bool bXY = ssCoNodes.find(GetNodeName(piSelectedNodes[iI])) != ssCoNodes.end();

      if( (!bGXY && bXY)  || (bGXY && !bXY) ) 
      {
        // no appropiate "ordering" of co-nodes
        bXYNodesOk = false;
        break;
      }
    }       

    if( bXYNodesOk && (NewG == G) )
    {
      // cout << " Induced ";
      delete piSelectedNodes;

      return true;
    }

    // get next combination - don't combine first and last nodes (they are U and V)
    //   only if we should not end combination

    bCombinationOk = piSelectedNodes[0] - 1 == piSelectedNodes[1];
    for(iI = 1; iI < G.m_iNumOfNodes - 2; iI++)
        if( (piSelectedNodes[iI] - piSelectedNodes[iI + 1]) != 1)
        {
          bCombinationOk = false;

          break;
        }

    if(bCombinationOk)
      break;

    do
    {
      /*
      cout << endl << "COMPrev: ";
      for(iI = 0; iI < G.m_iNumOfNodes; iI++)
        cout << setw(3) << piSelectedNodes[iI];
      */

      iI = G.m_iNumOfNodes - 2; // we don't combine last node
      while(iI >= 1) // we don't combine first node
      {
        if(piSelectedNodes[iI] == m_iNumOfNodes - 2) // we don't combine last node
        {
          piSelectedNodes[iI] = 1; // we don't combine first node
          iI--;
          continue;
        }
        else
        {
          piSelectedNodes[iI]++;
          break;
        }
      }

      bCombinationOk = true;
      for(iI = 0; iI < G.m_iNumOfNodes; iI++)
        for(iJ = iI + 1; iJ < G.m_iNumOfNodes; iJ++)
          if(piSelectedNodes[iI] == piSelectedNodes[iJ])
          {
            bCombinationOk = false;

            iI = G.m_iNumOfNodes;
            break;
          }
    }
    while(!bCombinationOk);

    /*
    cout << endl << "COM: ";
    for(iI = 0; iI < G.m_iNumOfNodes; iI++)
      cout << setw(3) << piSelectedNodes[iI];
    */
  }

  delete piSelectedNodes;

  return false;
}

//////////////////////////////////////////////////////////////////////////////
//
//  bool Graph::IsOrigInduced(Graph & G)
//
//  Test if graph G is induced in this graph. 
//
//////////////////////////////////////////////////////////////////////////////

bool Graph::IsOrigInduced(Graph & G)
{
  short iI, iJ;
  short * piSelectedNodes = new short[G.m_iNumOfNodes];
  Graph NewG(G.m_iNumOfNodes);
  bool bCombinationOk;

  if(m_iNumOfNodes < G.m_iNumOfNodes)
  {
    delete piSelectedNodes;
    return false;
  }

  // initialize combination
  for(iI = 0; iI < G.m_iNumOfNodes; iI++)
    piSelectedNodes[iI] = iI;

  /*
  cout  << endl << "COMStart: ";
  cout  << endl << "COM: ";
  for(iI = 0; iI < G.m_iNumOfNodes; iI++)
    cout << setw(3) << piSelectedNodes[iI];
  */
  while(true)
  {
    NewG.RemoveAllEdges();

    for(iI = 0; iI < G.m_iNumOfNodes; iI++)
      for(iJ = iI + 1; iJ < G.m_iNumOfNodes; iJ++)
        if(Edge(piSelectedNodes[iI], piSelectedNodes[iJ]))
          NewG.AddEdge(iI, iJ);

    if(NewG == G)
    {
      // cout << " Induced ";
      delete piSelectedNodes;

      //NewG.Write("NewG.dot");
      //G.Write("G.dot");

      return true;
    }

    // get next combination -  stop if we got end combination

    bCombinationOk = m_iNumOfNodes - 1 == piSelectedNodes[0];
    for(iI = 0; iI < G.m_iNumOfNodes - 1; iI++)
      if( (piSelectedNodes[iI] - piSelectedNodes[iI + 1]) != 1)
      {
        bCombinationOk = false;

        break;
      }

    if(bCombinationOk)
      // we searched all combination
      break;

    do
    {
      /*
      cout << endl << "COMPrev: ";
      for(iI = 0; iI < G.m_iNumOfNodes; iI++)
        cout << setw(3) << piSelectedNodes[iI];
      */

      iI = G.m_iNumOfNodes - 1; 
      while(iI >= 0) 
      {
        if(piSelectedNodes[iI] == m_iNumOfNodes - 1) 
        {
          piSelectedNodes[iI] = 0; 
          iI--;
          continue;
        }
        else
        {
          piSelectedNodes[iI]++;
          break;
        }
      }

      bCombinationOk = true;
      for(iI = 0; iI < G.m_iNumOfNodes; iI++)
        for(iJ = iI + 1; iJ < G.m_iNumOfNodes; iJ++)
          if(piSelectedNodes[iI] == piSelectedNodes[iJ])
          {
            bCombinationOk = false;

            iI = G.m_iNumOfNodes;
            break;
          }
    }
    while(!bCombinationOk);

    /*
    cout << endl << "COM: ";
    for(iI = 0; iI < G.m_iNumOfNodes; iI++)
      cout << setw(3) << piSelectedNodes[iI];
    */
  }

  delete piSelectedNodes;

  return false;
}

//////////////////////////////////////////////////////////////////////////////
//
//  short Graph::Write(string sFileName)
//
//////////////////////////////////////////////////////////////////////////////

short Graph::Write(string sFileName)
{
  fstream fout(sFileName.c_str(), ios::out | ios::app);
  short iI, iJ, iK;
  bool bHamiltonEdge;

  fout << "graph G" << endl;
  fout << "{" << endl;
  fout << "  rankdir = LR;" << endl;
  fout << "  node [shape = circle, fontname = Helvetica];" << endl;

  
  for(iI = 0; iI < m_iNumOfNodes; iI++)
    if(m_psNodeName[iI].size())
      fout << "  " << iI << " [label = \"" <<  m_psNodeName[iI] << "\"];" << endl;
    else
      fout << "  " << iI << " [label = \"\", style = filled];" << endl;
  
  for(iI = 0; iI < m_iNumOfNodes; iI++)
    for(iJ = iI + 1; iJ < m_iNumOfNodes; iJ++)
    {
      if(Edge(iI, iJ))
      {
        // is on hamilton cycle ?
        bHamiltonEdge = false;
        if(m_bIsHamilton)
        {
          for(iK = 0; iK < m_iNumOfNodes; iK++)
            if( (m_piHamiltonNodes[iK] == iI) && (m_piHamiltonNodes[(iK + 1) % m_iNumOfNodes] == iJ) ||
                (m_piHamiltonNodes[iK] == iJ) && (m_piHamiltonNodes[(iK + 1) % m_iNumOfNodes] == iI) )
            {
              fout << "  " << iI << " -- " << iJ << " [style = bold, color = red];" << endl;
              bHamiltonEdge = true;
              break;
            }
        }

        if(!bHamiltonEdge)
          fout << "  " << iI << " -- " << iJ << ";" << endl;
      }
    }
  fout << "}" << endl;

  fout.close();

  return RETURN_OK;
}

//////////////////////////////////////////////////////////////////////////////
//
//  short Graph::Read(string sFileName, set<Graph *> pGraphSet)
//
//////////////////////////////////////////////////////////////////////////////
short Graph::Read(string sFileName, list<Graph *> *pGraphList)
{
  fstream fin(sFileName.c_str());
  char pchLine[1000];
  string sLine;
  list<string> lsNodesNames;
  list<string>::iterator lIt;
  short iI;
  Graph *pG = NULL;
  strstream str;
  short iSourceNode, iDestinationNode;
  string sBegin, sRest;
  Parse P;

  if(!fin.is_open())
  {
    cout << "Cannot open file: " << sFileName;
    return RETURN_FAIL;
  }

  while(true)
  {
    // we now start reading of new graph
    if(!fin.good())
      break;
    
    // read nodes names
    fin.getline(pchLine, 1000);
    sLine = string(pchLine);

    // end of graph
    P.RemoveWS(sLine);
    if(sLine == "}")
    {
      pGraphList->push_back(pG);
      pG = NULL;

      continue;
    }

    // is line with name of node
    if(sLine.find("[label=\"") != string::npos) 
    {
      sRest = sLine.substr(sLine.find("[label=\"") + sizeof("[label=\"") - 1);

      // remove end "];
      sRest = sRest.substr(0, sRest.size() - 3);
      lsNodesNames.push_back(sRest);

      continue;
    }

    // is line with name of node
    if(P.Split(sLine, "--", sBegin, sRest))
    {
      if(pG == NULL)
      {
        pG = new Graph(lsNodesNames.size());

        for(lIt = lsNodesNames.begin(), iI = 0; lIt !=lsNodesNames.end(); lIt++, iI++)
          pG->SetNodeName(iI, *lIt);

        lsNodesNames.clear();
      }

      str.clear();
      str << sBegin;
      str >> iSourceNode;

      // remove end ;
      sRest = sRest.substr(0, sRest.size() - 1);
      str.clear();
      str << sRest;
      str >> iDestinationNode;

      pG->AddEdge(iSourceNode, iDestinationNode);

      continue;
    }
  }

  return RETURN_OK;
}

//////////////////////////////////////////////////////////////////////////////
//
//  bool Graph::SetClique(set<short> & siNodes)
//
//////////////////////////////////////////////////////////////////////////////

void Graph::SetClique(set<short> & siNodes)
{
  set<short>::iterator siSourceIt, siDestinationIt; 

  // make complete graph
  for(siSourceIt = siNodes.begin(); siSourceIt != siNodes.end(); siSourceIt++)
    for(siDestinationIt = siSourceIt; siDestinationIt != siNodes.end(); siDestinationIt++)
      if( (*siSourceIt) != (*siDestinationIt) ) // loop edge is possible, but is not nessesary
        Edge(*siSourceIt, *siDestinationIt) = true;
}

//////////////////////////////////////////////////////////////////////////////
//
//  bool Graph::IsClique(set<short> & siNodes)
//
//////////////////////////////////////////////////////////////////////////////

bool Graph::IsClique(set<short> & siNodes)
{
  set<short>::iterator siSourceIt, siDestinationIt; 

  // make complete graph
  for(siSourceIt = siNodes.begin(); siSourceIt != siNodes.end(); siSourceIt++)
    for(siDestinationIt = siSourceIt; siDestinationIt != siNodes.end(); siDestinationIt++)
      if( (*siSourceIt) != (*siDestinationIt) ) // loop edge is possible, but is not nessesary
        if(!Edge(*siSourceIt, *siDestinationIt))
        {
          return false; 
        }

  return true;
}

//////////////////////////////////////////////////////////////////////////////
//
//  short Graph::SolveExpression(string sExpression, set<short> & siNodes)
//
//////////////////////////////////////////////////////////////////////////////

short Graph::SolveExpression(string sExpression, set<short> & siNodes, set<short> & siCoreNodes)
{
  short iNode;
  string sBegin, sRest, sParameter, sUsedDelimeter;
  set<string> ssOperators;
  set<short> siNodesA, siNodesB;
  set<short> siCoreNodesDummy;
  set<short>::iterator siIt, siItRem;

  ssOperators.insert("\\");
  ssOperators.insert("|");

  //==========================================================================
  //  rule 0:
  //
  //              empty expression hasn't nodes
  //
  //==========================================================================

  if(sExpression.empty())
    return RETURN_OK;

  //==========================================================================
  //  rule 1:
  //
  //                   (exp)  <<=>>  exp 
  //
  //==========================================================================

  RemoveAmbientBrackets(sExpression, '(', ')');

  //==========================================================================
  //  rule 2:
  //
  //                      is exp node ?
  //
  //==========================================================================

  if((iNode = FindNode(sExpression)) != NO_NODE)
  {
    siNodes.insert(iNode);
    
    return RETURN_OK;
  }

  //==========================================================================
  //  rule 3:
  //
  //                 is exp in neighbourhood ?
  //
  //==========================================================================

  if(ExtractParameterFromFunction(sExpression, "N", '(', ')', sParameter))
    if((iNode = FindNode(sParameter)) != NO_NODE)
    {
      GetNeighbourhood(iNode, siNodes);
      
      siCoreNodes.insert(iNode);

      return RETURN_OK;
    }
    else
      return RETURN_FAIL;

  //==========================================================================
  //  rule 4:
  //
  //              is exp in closed neighbourhood ?
  //
  //==========================================================================

  if(ExtractParameterFromFunction(sExpression, "N", '[', ']', sParameter))
    if((iNode = FindNode(sParameter)) != NO_NODE)
    {
      GetNeighbourhood(iNode, siNodes);
      siNodes.insert(iNode);
      siCoreNodes.insert(iNode);

      return RETURN_OK;
    }
    else
      return RETURN_FAIL;

  //==========================================================================
  //  rule 5:
  //
  //                    insert {x,y} in nodes
  //
  //==========================================================================

  if(RemoveAmbientBrackets(sExpression, '{', '}'))
  {
    sRest = sExpression;
    while(!sRest.empty())
    {
      Parse::Split(sRest, ",", sBegin, sRest);

      if((iNode = FindNode(sBegin)) != NO_NODE)
        siNodes.insert(iNode);
      else
        return RETURN_FAIL;
    }
  
    return RETURN_OK;
  }

  //==========================================================================
  //  rule 6:
  //
  //              exp1 | exp2    or   exp1 \ exp2
  //
  //==========================================================================

  if(Split(sExpression, ssOperators, sBegin, sRest, sUsedDelimeter))
  {
    if(sUsedDelimeter == "|")
    {
      SolveExpression(sBegin, siNodes, siCoreNodes);
      SolveExpression(sRest, siNodes, siCoreNodes);
   
      return RETURN_OK;
    }
    else
      if(sUsedDelimeter == "\\")
      {
        SolveExpression(sBegin, siNodesA, siCoreNodesDummy);

        while(sUsedDelimeter == "\\")
        {
          if(!Split(sRest, ssOperators, sBegin, sRest, sUsedDelimeter))
          {
            sUsedDelimeter = "";
            sBegin = sRest;
          }

          SolveExpression(sBegin, siNodesB, siCoreNodes);

          for(siIt = siNodesB.begin(); siIt != siNodesB.end(); siIt++)
          {
            siItRem = siNodesA.find(*siIt); 
            if(siItRem != siNodesA.end())
              siNodesA.erase(*siItRem);
          }

          siNodesB.clear();
        }

        for(siIt = siNodesA.begin(); siIt != siNodesA.end(); siIt++)
            siNodes.insert(*siIt);

        siNodesA.clear();

        if(sUsedDelimeter == "|")
          SolveExpression(sRest, siNodes, siCoreNodesDummy);
      }
      else
        return RETURN_FAIL;
    
      return RETURN_OK;
    }

  //==========================================================================
  //  rule 7:
  //
  //                       exp1 & exp2
  //
  //==========================================================================

  if(Split(sExpression, "&", sBegin, sRest))
  {
    SolveExpression(sBegin, siNodesA, siCoreNodesDummy);
    SolveExpression(sRest, siNodesB, siCoreNodesDummy);
   
    for(siIt = siNodesA.begin(); siIt != siNodesA.end(); siIt++)
      if(siNodesB.find(*siIt) != siNodesB.end())
         siNodes.insert(*siIt);

    siNodesA.clear();
    siNodesB.clear();

    return RETURN_OK;
  }

  return RETURN_FAIL;
}

//////////////////////////////////////////////////////////////////////////////
//
//  bool Graph::MakeNeighbourhood(string sExpression, short iNode, 
//                                list<Graph *> & lGraph)
//
//  Make neighbourhood conections from iNode to node in sExpression.
//  return true if was graph updated (added edge).
//
//////////////////////////////////////////////////////////////////////////////

bool Graph::MakeNeighbourhood(string sExpression, short iNode, list<Graph *> & lGraph)
{
  short iFoundNode;
  bool bRet;
  string sBegin, sRest, sParameter, sUsedDelimeter;
  set<string> ssOperators;
  list<Graph *> lGraphA, lGraphB;
  list<Graph *>::iterator sgIt;
  Graph * pNewG;

  ssOperators.insert("\\");
  ssOperators.insert("|");

  //==========================================================================
  //  rule 0:
  //
  //              empty expression hasn't nodes
  //
  //==========================================================================

  if(sExpression.empty())
    return false;

  //==========================================================================
  //  rule 1:
  //
  //                   (exp)  <<=>>  exp 
  //
  //==========================================================================

  RemoveAmbientBrackets(sExpression, '(', ')');

  //==========================================================================
  //  rule 2:
  //
  //                      is exp iNode ?
  //
  //==========================================================================

  if((iFoundNode = FindNode(sExpression)) != NO_NODE)
    return false;

  //==========================================================================
  //  rule 3:
  //
  //                    insert {x,y} in nodes
  //
  //==========================================================================

  if(RemoveAmbientBrackets(sExpression, '{', '}'))
    // this set has now impact to neighbourhood
    return false;

  //==========================================================================
  //  rule 4:
  //
  //                 is iNode in neighbourhood ?
  //              is iNode in closed neighbourhood ?
  //
  //==========================================================================

  if(ExtractParameterFromFunction(sExpression, "N", '(', ')', sParameter) ||
     ExtractParameterFromFunction(sExpression, "N", '[', ']', sParameter) )
    if((iFoundNode = FindNode(sParameter)) != NO_NODE)
    {
      if(iNode == iFoundNode)
        // self loop is bad
        return false;

      AddEdge(iNode, iFoundNode);

      return true;
    }
    else

      return false;

  //==========================================================================
  //  rule 5:
  //
  //              exp1 | exp2    or    exp1 \ exp2
  //
  //==========================================================================

  if(Split(sExpression, ssOperators, sBegin, sRest, sUsedDelimeter))
  {
    if(sUsedDelimeter == "|")
    {
      pNewG = new Graph(*this);
      ASSERT(pNewG);

      bRet = MakeNeighbourhood(sBegin, iNode, lGraph);

      if(pNewG->MakeNeighbourhood(sRest, iNode, lGraph))
      {
        lGraph.push_back(pNewG);
        bRet = true;
      }
      else
        delete pNewG;

      return bRet;
    }
    else
      if(sUsedDelimeter == "\\")
      {
        pNewG = new Graph(*this);
        ASSERT(pNewG);

        bRet = MakeNeighbourhood(sBegin, iNode, lGraph);

        while(sUsedDelimeter == "\\")
        {
          if(!Split(sRest, ssOperators, sBegin, sRest, sUsedDelimeter))
            sUsedDelimeter = "";
        }

        if(sUsedDelimeter == "|")
        {
          if(pNewG->MakeNeighbourhood(sRest, iNode, lGraph))
          {
            lGraph.push_back(pNewG);
            bRet = true;
          }
          else
            delete pNewG;
        }
        else
          delete pNewG;

        return bRet;
      }
      else
        // unsupported delimeter used
        return false;
  }

  //==========================================================================
  //  rule 7:
  //
  //                       exp1 & exp2
  //
  //==========================================================================

  if(Split(sExpression, "&", sBegin, sRest))
  {
    lGraphA.clear();
    bRet = MakeNeighbourhood(sBegin, iNode, lGraphA);

    if(bRet)
    {
      lGraphB.clear();
      bRet = MakeNeighbourhood(sRest, iNode, lGraphB);

      for(sgIt = lGraphA.begin(); sgIt != lGraphA.end(); sgIt++)
          bRet |= (*sgIt)->MakeNeighbourhood(sRest, iNode, lGraphB);
    }

    if(bRet)
      for(sgIt = lGraphB.begin(); sgIt != lGraphB.end(); sgIt++)
        lGraph.push_back(*sgIt);
    
    return bRet;
  }

  return false;
}
