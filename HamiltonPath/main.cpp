//////////////////////////////////////////////////////////////////////////////
//
//  Hamiltonian closure testing by Filip Jurcicek
//
//////////////////////////////////////////////////////////////////////////////

#include "../Source/GlobalDefine.h"
#include "../Source/Graph.h"
#include "../Source/ClosureHamPath.h"
#include <time.h>

int main(int argn, char ** argv)
{
  time_t t1, t2;
  ClosureHamPath cl;

  time(&t1);
  //==========================================================================

  cout << "Testing of path closure by Filip Jurcicek" << endl;
  cout << "----------------------------------------------------" << endl;

  cout << "Removing all files from Output folder." << endl;
  ////////////////////////////////////////////////////////////////////////////
  system("`rm ./Output/*`");
  system("del /Q Output\\*.dot");

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
  Graph G(13);

  G.SetNodeName(0, "u");
  G.SetNodeName(2, "y1-");
  G.SetNodeName(3, "y1");
  G.SetNodeName(4, "y1+");
  G.SetNodeName(5, "x-");
  G.SetNodeName(6, "x");
  G.SetNodeName(7, "x+");
  G.SetNodeName(8, "y2-");
  G.SetNodeName(9, "y2");
  G.SetNodeName(10, "y2+");
  G.SetNodeName(12, "v");
  
  // path
  G.AddEdge(0,1);
  G.AddEdge(1,2);
  G.AddEdge(2,3);
  G.AddEdge(3,4);
  G.AddEdge(4,5);
  G.AddEdge(5,6);
  G.AddEdge(6,7);
  G.AddEdge(7,8);
  G.AddEdge(8,9);
  G.AddEdge(9,10);
  G.AddEdge(10,11);
  G.AddEdge(11,12);
  G.AddEdge(12,13);

  // structural element
  G.AddEdge("u", "x");
  G.AddEdge("u", "y1");
  G.AddEdge("u", "y2");
  G.AddEdge("v", "x");
  G.AddEdge("v", "y1");
  G.AddEdge("v", "y2");
  G.AddEdge("x", "y1");
  G.AddEdge("x", "y2");
  G.AddEdge("y1", "y2");

  // N(x)
  G.AddEdge("x-", "u");
  G.AddEdge("x+", "v");

  // N(y1)
  //G.AddEdge("y1-", "y1+"); // not coplete clique
  // N(y2)
  //G.AddEdge("y2-", "y2+"); // not coplete clique

  G.IsHamiltonian();
  G.Write("problem.dot");

  return RETURN_OK;
  */
