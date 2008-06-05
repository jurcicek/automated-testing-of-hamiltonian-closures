//////////////////////////////////////////////////////////////////////////////
//
//  Generating of forbidden graphs for hamiltonian cycle by Filip Jurcicek
//
//////////////////////////////////////////////////////////////////////////////

#include "../Source/GlobalDefine.h"
#include "../Source/Graph.h"
#include "../Source/ForbiddenGraphs.h"
#include <time.h>

int main(int argn, char ** argv)
{
  time_t t1, t2;
  ForbiddenGraphs cl;

  time(&t1);
  //==========================================================================

  cout << "Generating of forbidden graphs for hamiltonian cycle by Filip Jurcicek" << endl;
  cout << "----------------------------------------------------------------------" << endl;

  cout << "Removing all output files" << endl;
  ////////////////////////////////////////////////////////////////////////////
  system("`rm ./Output/*`");
  system("`rm ./corrupted.dot`");

  system("del /Q Output\\*.dot");
  system("del /Q corrupted.dot");

  cout << "Processing of closure." << endl;
  ////////////////////////////////////////////////////////////////////////////
    
  if(argv[1])
    cl.Read(string(argv[1]));
  else 
    cl.Read("closure.txt");
  cl.Print();

  cl.ParseClosure();
  cl.WriteConditionSet("Output/condition-set.txt");
  cl.GenerateGraphs("Output/graph");

  //==========================================================================
  time(&t2);
  cout << "Time ellapsed in seconds: " << t2 - t1 << endl;
  cout << "Time ellapsed in minutes: " << (t2 - t1) / 60 << endl;
  cout << "Time ellapsed in hours:   " << (t2 - t1) / 3600 << endl;

  return RETURN_OK;
}

  /*
  Graph g1(4), g2(4);
  set<string> ssCoNodes;

  ssCoNodes.insert("x");
  ssCoNodes.insert("y");

  g1.SetNodeName(0, "u");
  g1.SetNodeName(1, "x");
  g1.SetNodeName(2, "y");
  g1.SetNodeName(3, "v");

  g1.AddEdge(0,1);
  g1.AddEdge(1,2);
  g1.AddEdge(2,3);
  g1.AddEdge(3,0);

  g2.SetNodeName(0, "u");
  g2.SetNodeName(1, "y");
  g2.SetNodeName(2, "x");
  g2.SetNodeName(3, "v");

  g2.AddEdge(0,1);
  g2.AddEdge(1,2);
  g2.AddEdge(2,3);
  g2.AddEdge(3,0);


  if(g1.IsInduced(g2, ssCoNodes))
    cout << "Induced" << endl;
  else
    cout << "Not Induced" << endl;

  return 0;
  */
