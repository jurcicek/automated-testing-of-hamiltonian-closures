//////////////////////////////////////////////////////////////////////////////
//
//  Hamiltonian closure testing by Filip Jurcicek
//
//////////////////////////////////////////////////////////////////////////////

#include "../Source/GlobalDefine.h"
#include "../Source/Graph.h"
#include "../Source/ClosureHamCycle.h"
#include <time.h>

int main(int argn, char ** argv)
{
  time_t t1, t2;
  ClosureHamCycle cl;

  time(&t1);
  //==========================================================================

  cout << "Testing of cycle closure by Filip Jurcicek" << endl;
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
