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
    double when = 1323722800.0;
    double twhen = 1334332800.0;
    double t2when = 1334332800.4;
    double  nodatatime = 1373970148.000000;
    double ehmgpr, em121ds0, em121ds5, trtgtd;
    double t1, t2;
    BeamFolder::_debug = 1;
    std::string teststr("1321032116708,E:HMGPR,TORR,687.125");

    std::cout << std::setiosflags(std::ios::fixed);
 
  // test with someone who doesn't have any data, to check error timeouts
 // std::cout << "Trying a nonexistent location\n";
 // BeamFolder bfu("NuMI_Physics_A9","http://bel-kwinith.fnal.gov/");
 // bfu.set_epsilon(.125);
//
 // try {
  //  bfu.GetNamedData(nodatatime,"E:HP121@[1]",&ehmgpr,&t1);
   // std::cout << "got values " << ehmgpr <<  "for E:HP121[1]at time " << t1 << "\n";
 // } catch (WebAPIException &we) {
  //     std::cout << "got exception:" << we.what() << "\n";
  //}

  std::cout << "Trying the default location\n";
  BeamFolder bf("NuMI_Physics_A9");
  bf.set_epsilon(.125);

  try {
    double tlist[] = {1386058021.613409,1386058023.279863,1386058079.149919,1386058080.816768};
    for (int i = 0; i < 4 ; i++) {
       bf.GetNamedData(tlist[i],"E:TRTGTD@",&trtgtd,&t1);
       std::cout << "got values " << trtgtd <<  "for E:TRTGTD at time " << t1 << "\n";
    }
  } catch (WebAPIException &we) {
       std::cout << "got exception:" << we.what() << "\n";
  }
  exit(0);
  try {
    bf.GetNamedData(nodatatime,"E:HP121@[1]",&ehmgpr,&t1);
    std::cout << "got values " << ehmgpr <<  "for E:HP121[1]at time " << t1 << "\n";
  } catch (WebAPIException &we) {
       std::cout << "got exception:" << we.what() << "\n";
  }
  try {
    bf.GetNamedData(nodatatime,"E:HP121@[1]",&ehmgpr,&t1);
    std::cout << "got values " << ehmgpr <<  "for E:HP121[1]at time " << t1 << "\n";
  } catch (WebAPIException &we) {
       std::cout << "got exception:" << we.what() << "\n";
  }

  try {
    bf.GetNamedData(t2when,"E:NOSUCHVARIABLE",&ehmgpr,&t1);
    std::cout << "got values " << ehmgpr <<  "for E:HP121[1]at time " << t1 << "\n";
  } catch (WebAPIException &we) {
       std::cout << "got exception:" << we.what() << "\n";
  }

  try {
    bf.GetNamedData(t2when,"E:HP121@[1]",&ehmgpr,&t1);
    std::cout << "got values " << ehmgpr <<  "for E:HP121[1]at time " << t1 << "\n";

    bf.GetNamedData(when,"E:TR101D@",&ehmgpr,&t1);
    std::cout << "got values " << ehmgpr <<  "for E:TR101D at time " << t1 << "\n";
    bf.GetNamedData(twhen,"E:TR101D@",&ehmgpr,&t1);
    std::cout << "got values " << ehmgpr <<  "for E:TR101D at time " << t1 << "\n";
    
    bf.GetNamedData(when,"E:HMGPR",&ehmgpr);
    bf.GetNamedData(when,"E:M121DS[0],E:M121DS[5]",&em121ds0, &em121ds5);
    std::cout << "got value " << ehmgpr << "for E:HMGPR\n";
    std::cout << "got values " << em121ds0 << ',' << em121ds5 << "for E:M121DS[0,5]\n";

    bf.GetNamedData(when,"E:M121DS@[0],E:M121DS@[5]",&em121ds0, &t1, &em121ds5, &t2);
    std::cout << "got values " << em121ds0 << ',' << em121ds5 << "for E:M121DS[0,5] at time " << t1 << "\n";
 
    std::cout << "time stamps:";
    std::vector<double> times = bf.GetTimeList();
    for (size_t i = 0; i < times.size(); i++) {
        std::cout << times[i] << ", ";
    }
    std::cout << "\n";

    std::cout << "variables:";
    std::vector<std::string> vars = bf.GetDeviceList();
    for (size_t i = 0; i < vars.size(); i++) {
        std::cout << vars[i] << ", ";
    }
    std::cout << "\n";

    std::cout << "vector E:M121DS[]:";
    std::vector<double> values = bf.GetNamedVector(when,"E:M121DS[]");
    for (size_t i = 0; i < values.size(); i++) {
        std::cout << values[i] << ", ";
    }
    std::cout << "\n";

    std::cout << "vector E:M121DS[] with time:";
    values = bf.GetNamedVector(when,"E:M121DS[]",&t1);
    std::cout << "at time: " << t1 << ": ";
    for (size_t i = 0; i < values.size(); i++) {
        std::cout << values[i] << ", ";
    }
    std::cout << "\n";
    
    bf.GetNamedData(1323726316.528,"E:HMGPR",&ehmgpr);
    std::cout << "got value " << ehmgpr << "for E:HMGPR\n";
    bf.GetNamedData(1323726316.528,"E:HMGPR@",&ehmgpr, &t1);
    std::cout << "got value " << ehmgpr << "for E:HMGPR at time " << t1 << "\n";
    bf.GetNamedData(1323726318.594,"E:HMGPR",&ehmgpr);
    std::cout << "got value " << ehmgpr << "for E:HMGPR\n";
    bf.GetNamedData(1323726318.594,"E:HMGPR@",&ehmgpr,&t2);
    std::cout << "got value " << ehmgpr << "for E:HMGPR at time " << t2 << "\n";
    std::cout << "Done!\n";
  } catch (WebAPIException &we) {
       std::cout << "got exception:" << we.what() << "\n";
  }
}
