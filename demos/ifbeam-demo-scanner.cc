#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "ifbeam.h"

int
main() {
  std::string name;
  double t;
  std::vector<double> vals;
  int count = 0;

  BeamFolderScanner bfs("NuMI_Physics_A9", 1334332800.4);
  while( bfs.NextDataRow( t, name, vals ) && count++ < 1000 )  {
    std::cout.precision(11);
    std::cout << "got time "<< t << " name " << name << " values: " ;
    for(unsigned i = 0; i < vals.size(); i++ )
       std::cout << vals[i] << ", " ;
    std::cout << "\n";
  }

}
