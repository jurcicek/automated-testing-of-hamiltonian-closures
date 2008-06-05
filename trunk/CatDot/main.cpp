//////////////////////////////////////////////////////////////////////////////
//
//  Generating of forbidden graphs for hamiltonian cycle by Filip Jurcicek
//
//////////////////////////////////////////////////////////////////////////////

#include "../Source/GlobalDefine.h"

bool IsNumber(string &str)
{
  short iI;

  if(!str.size())
    return false;

  for(iI = 0; iI < str.size(); iI++)
    if( (str[iI] < '0') || (str[iI] > '9') )
      return false;
  
  return true;
}

int main(int argn, char ** argv)
{
  long lGraphCounter = 0;
  long lCatCounter = 0;
  bool bExit = false;
  string sFileName;

  cout << "Cat one dot of forbidden graps to many dots by Filip Jurcicek" << endl;
  cout << "----------------------------------------------------------------------" << endl;

  if(argv[1])
    sFileName = string(argv[1]);
  else 
    sFileName = "graph.fg.dot";

  fstream fin(sFileName.c_str());

  if(!fin.is_open())
  {
    cout << "Cannot open file: " << sFileName;
    return -1;
  }
  
  while(true)
  {
    strstream str;
    string sCat;

    str << sFileName << "." << setw(2) << setfill('0') << lCatCounter++ << ".dot";
    str >> sCat;

    fstream fout(sCat.c_str(), ios::out);

    string sE;

    fout << "graph G" << endl;
    fout << "{" << endl;
    fout << "  rankdir = LR;" << endl;
    fout << "  size = \"10,7\";" << endl;

    lGraphCounter = 0;
    while(true)
    {
      if(!fin.good())
      {
        bExit = true;
        break;
      }

      fin >> sE;

      if(sE == "graph")
      {
        lGraphCounter++;

        fout << "  subgraph G" << lGraphCounter << endl;
        fout << "  {" << endl;
        fout << "    label = \"\\n Graph " << lGraphCounter << "\";"<< endl;
        fout << "    color = blue;" << endl;
	      fout << "    fontsize=20;" << endl << "   ";

        continue;
      }

      if(sE == "G") 
        continue;

      if(sE == "{")
        continue;
    
      if(sE == "}")
      {
        fout << "}" << endl;

        if(lGraphCounter % 12 == 0)
          break;

        continue;
      }

      if(IsNumber(sE))
      {
        fout << " " << setw(2) << setfill('0') << lGraphCounter << setw(2) << setfill('0') << sE;
        continue;
      }

      if(sE.size() && sE[sE.size() - 1] == ';')
      {
        if(IsNumber(sE.substr(0, sE.size() - 1)))
          fout << " " << setw(2) << setfill('0') << lGraphCounter << setw(3) << setfill('0') << sE << endl << "   ";
        else
          fout << " " << sE << endl << "   ";

        continue;
      }
      else
      {
        fout << " " << sE;
        continue;
      }
    }

    fout << "}" << endl;

    string s;

    s = "dot -Tps " + sCat + " -o " + sCat + ".ps";
    system(s.c_str());

    if(bExit)
      break;
  }

  return RETURN_OK;
}
