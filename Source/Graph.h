//////////////////////////////////////////////////////////////////////////////
//
//  Hamiltonian closure testing by Filip Jurcicek
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __GRAPH
#define __GRAPH

#include "GlobalDefine.h"
#include "Parse.h"

class Graph: public Parse
{
  static long m_liIdCounter;
  short m_iNumOfNodes;
  bool * m_pbIncidentMatrix;
  string * m_psNodeName;
  long * m_pliNodeId;

  short * m_piHamiltonNodes;
  bool m_bIsHamilton;

public: 
  Graph(short iNumOfNodes);
  Graph(Graph & G);
  ~Graph(void);

  short GetNumOfNodes(void) { return m_iNumOfNodes;}
  void SetNodeName(short iNode, string sName);
  string & GetNodeName(short iNode) { return m_psNodeName[iNode]; }
  void SetNodeId(short iNode, long liNodeId);
  long GetNodeId(short iNode) { return m_pliNodeId[iNode]; }

  short FindNode(string sNode);
  short FindNodeById(long liNodeId);
  
  short AddEdge(short iSourceNode, short iDestinationNode);
  short AddEdgeById(short iSourceNodeId, short iDestinationNodeId);
  short AddEdge(string sSourceNode, string sDestinationNode);
  short RemoveEdge(short iSourceNode, short iDestinationNode);
  short RemoveEdgeById(short iSourceNodeId, short iDestinationNodeId);
  short RemoveEdge(string sSourceNode, string sDestinationNode);
  short RemoveAllEdges(void);

  bool IsHamiltonianCycle(void);

  bool HamPathRec(short iIdx, bool bTPath);
  bool IsHamiltonianPath(void);
  bool IsHamiltonianRPath(void);
  bool IsHamiltonianSTPath(void);

  bool IsInduced(Graph & G, set<string> &ssCoNodes);
  bool IsOrigInduced(Graph & G);

  short Write(string sFileName);
  static short Read(string sFileName, list<Graph *> *pGraphList);

  short CreateNodesNeighbourhood(Graph ** ppNodesNeighbourhoodG, set<string> &ssNodes, 
    short iDegree = 1, bool bUseUV = false);
  short ReduceGraph(Graph ** ppG, set<string> &ssNodes);
  short GetNeighbourhood(short iNode, set<short> & siNeighbourhood);
  bool IsClique(set<short> & siNodes);
  void SetClique(set<short> & siNodes);
  
  short SolveExpression(string sExpression, set<short> & siNodes, set<short> & siCoreNodes);
  bool MakeNeighbourhood(string sExpression, short iNode, list<Graph *> & lGraph);

  bool & Edge(short iSourceNode, short iDestinationNode)
  {
    ASSERT(iSourceNode != iDestinationNode);
    ASSERT(iSourceNode >= 0 && iSourceNode < m_iNumOfNodes);
    ASSERT(iDestinationNode >= 0 && iDestinationNode < m_iNumOfNodes);

    if(iSourceNode <= iDestinationNode)
      return m_pbIncidentMatrix[iSourceNode * m_iNumOfNodes + iDestinationNode];
    else
      return m_pbIncidentMatrix[iDestinationNode * m_iNumOfNodes + iSourceNode];
  }

  bool operator == (Graph & G);
};

#endif
