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

void
print_POT_window(double first, double last) {
  std::string name;
  double t;
  std::vector<double> vals;
  int count = 0;
  BeamFolderScanner bfs("NuMI_Physics_A9", first);
  while( bfs.NextDataRow( t, name, vals ) && t < last)  {

    if (name == "E:TRTGTD" || name == "E:TR101D") {
       std::cout.precision(11);
       std::cout << "got time "<< t << " name " << name << " values: " ;
       for(unsigned i = 0; i < vals.size(); i++ )
          std::cout << vals[i] << ", " ;
       std::cout << "\n";
    }
  }
}

int
main() {
  double t = 1334332800.0;
  print_POT_window(t, t+3600);
}
