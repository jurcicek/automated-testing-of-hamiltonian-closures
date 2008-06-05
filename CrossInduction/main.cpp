//////////////////////////////////////////////////////////////////////////////
//
//  Generating of forbidden graphs for hamiltonian cycle by Filip Jurcicek
//
//////////////////////////////////////////////////////////////////////////////

#include "../Source/GlobalDefine.h"
#include "../Source/Graph.h"
#include <stdio.h> 

int main(int argn, char ** argv)
{
  string sFileName;
  list<Graph *> GraphList;
  list<Graph *>::iterator lIt, lJt;
  long iI, iJ;
  long lCrossCounter, lSize;  
  bool bInduced;

  cout << "Cross induction one dot of forbidden graps to another dot by Filip Jurcicek" << endl;
  cout << "---------------------------------------------------------------------------" << endl;

  cout << "Removing all output files" << endl;
  ////////////////////////////////////////////////////////////////////////////
  remove("graph.fg.new.dot");

  if(argv[1])
    sFileName = string(argv[1]);
  else 
    sFileName = "graph.fg.dot";

  remove((sFileName + ".cfg.dot").c_str());
  remove((sFileName + ".ncfg.dot").c_str());

  Graph::Read(sFileName, &GraphList);

  cout << endl << "Was read " << GraphList.size() << " graphs" << endl << endl;

  cout << "Processing of cross induction." << endl << endl;
  ////////////////////////////////////////////////////////////////////////////

  lCrossCounter = 0;
  lSize = GraphList.size();

  for(lIt = GraphList.begin(), iI = 0; lIt != GraphList.end(); lIt++, iI++)
  {
    bInduced = false;
    for(lJt = GraphList.begin(), iJ = 0; lJt != GraphList.end(); lJt++, iJ++)
      if( !(**lIt == **lJt) )
        if( (*lIt)->IsOrigInduced(**lJt) )
        {
          // lIt possible forbidden graph is already forbidden by lJt forbiden graph
          bInduced = true;
          break;
        }

    if(!bInduced)
    {
      (*lIt)->Write(sFileName + ".cfg.dot");
      lCrossCounter++;
    }
    else
    {
      (*lIt)->Write(sFileName + ".ncfg.dot"); 

      cout << endl << "In " << iI + 1 << " induced graph " << iJ + 1<< endl;

      lIt = GraphList.erase(lIt);
      lIt--; 
    }

    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////

    cout << "                                          \r";
    cout << "Processed " << (int)((double)iI / 
      (lSize - 1) * 100) << " % of all FGs.";
    cout << " Cross induction FG number: " << lCrossCounter;
    
    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////
  }

  cout << endl << endl;

  return RETURN_OK;
}

