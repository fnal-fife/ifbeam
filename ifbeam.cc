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
#include "utils.h"
#include "WebAPI.h"

int BeamFolder::_debug;

BeamFolder::BeamFolder(std::string bundle_name, std::string url, double time_width) {
    _time_width = time_width;
    _bundle_name =  bundle_name;
    _url = url;
    _cache_start = 0.0;
    _cache_end = 0.0;
    _cache_slot = -1;
}

void
BeamFolder::FillCache(double for_time) {
    std::vector<std::string>::iterator it;
    int i,j;
    std::string st;

    if (for_time >= _cache_start && for_time <= _cache_end ) {
        // we're already in the cache...
        return;
    }

    // cache is flushed...
    _values.clear();
    _n_values = 0;
    _cache_slot = -1;

    std::stringstream varurl;
    
    varurl << _url << "/data/data" 
           << "?b=" << _bundle_name
           << std::setiosflags(std::ios::fixed) 
           << "&t0=" << for_time
           << "&t1=" << for_time + _time_width;

    _debug && std::cout << "fetching url: " << varurl.str() << "\n";

    WebAPI s(varurl.str());

    getline(s.data(), st); // header...
    _debug && std::cout << "got line: " << st << "\n";

    while( !s.data().eof() ) {
	getline(s.data(), st);

        _debug && std::cout << "got line: " << st << "\n";

	_values.push_back(st);
        _n_values++;
    }
    _cache_start = for_time;
    _cache_end = for_time + _time_width;
}

double
get_time(std::string s) {
    return atof(s.c_str());
}

std::string 
get_name(std::string s) {
     size_t p1, p2;
     p1 = s.find(',');
     p2 = s.find(',',p1+1);
     std::cout << "finding name " << s.substr(p1+1,p2-p1-1) << " in: " << s <<"\n";
     return s.substr(p1+1,p2-p1-1);
}

double
get_value(std::string s, int n) {
     int i;
     size_t p1,p2;
     p2=0;
     for( i = 0; i < n+4 ; i++ ) {
         p1 = p2;
         p2 = s.find(',',p1+1); 
         std::cout << "part " << i << " is " << s.substr(p1+1,p2-p1) << "\n";
     }
     return atof(s.substr(p1+1,p2-p1).c_str());
}

void
BeamFolder::GetNamedData(double from_time, std::string variable_list, ...) {
    std::vector<std::string> variables, values;
    std::vector<std::string>::iterator rvit, it;
    std::string curvar;
    double *curdest;
    size_t bpos;
    va_list al;
    int first_time_slot;
    int array_slot;
    int search_slot;
    int found;
    double first_time;

    _debug && std::cout << "looking for time" <<  from_time << "\n";
    // fetch data into cache (if needed)
    FillCache(from_time);

    // find first slot with this time; may have it cached...
    if (_cache_slot >= 0 && get_time(_values[_cache_slot]) == from_time ) {
         _debug && std::cout << "finding cached slot: " << _cache_slot << "\n";
         first_time_slot = _cache_slot;
    } else {
	// start proportionally through the data and search for the time we want
	first_time_slot = _n_values * (from_time - _cache_start) / (_cache_end - _cache_start);
         _debug && std::cout << "starting search at: " << first_time_slot << "data: " << _values[first_time_slot] <<"\n";
	while( first_time_slot < _n_values && get_time(_values[first_time_slot]) < from_time ) {
	   first_time_slot++;
	}
        _debug && std::cout << "after scan forward: " << first_time_slot << "data: " << _values[first_time_slot] <<"\n";
	while( first_time_slot > 0 && get_time(_values[first_time_slot]) > from_time ) {
	   first_time_slot--;
	}
        _debug && std::cout << "after scan backward: " << first_time_slot << "data: " << _values[first_time_slot] <<"\n";
        first_time = get_time(_values[first_time_slot]);
	while( first_time_slot > 1 && get_time(_values[first_time_slot-1]) == first_time ) {
	   first_time_slot--;
	}
        _debug && std::cout << "after scan equals: " << first_time_slot << "data: " << _values[first_time_slot] <<"\n";
        _cache_slot = first_time_slot;
    }

    _debug && std::cout << "after scan, first_time_slot is : " << first_time_slot << "data: " << _values[first_time_slot] <<"\n";
   
    va_start(al, variable_list);

    variables = split(variable_list,',');
    
    for( rvit = variables.begin(); rvit != variables.end(); rvit++) {
        search_slot = first_time_slot;
        // find varname to lookup :
        // if we have a [n] on the end, set slot to n, and 
        // make it var[]
        //  
        //
        found = 0;
        bpos = rvit->find('[');
        if (bpos != std::string::npos) {
            array_slot = atoi(rvit->c_str()+bpos+1);
            curvar = *rvit;
            curvar[bpos+1] = ']';
            curvar = curvar.substr(0,bpos+2);
        } else {
            curvar =  *rvit;
            array_slot = 0;
        }
        _debug && std::cout << "searching for var: " << curvar << "\n";
     
        // the place to put the value is the next varargs parameter
        curdest = (double *) va_arg(al,void*);
      
 
       // scan for named variable with this time
       _debug && std::cout << "checking slot: " << search_slot << " name: " << get_name(_values[search_slot]) << " value: " << _values[search_slot] <<"\n";

       while( search_slot < _n_values && get_time(_values[search_slot]) == first_time && get_name(_values[search_slot]) < curvar)  {
           search_slot++;
            _debug && std::cout << "checking slot: " << search_slot << " name: " << get_name(_values[search_slot]) << " value: " << _values[search_slot] <<"\n";
        }

       _debug && std::cout << "after namesearch, search_slot is : " << search_slot << "data: " << _values[search_slot] <<"\n";

        if ( curvar == get_name(_values[search_slot])) {
           std::vector <std::string> vallist;
           
           *curdest = get_value(_values[search_slot],array_slot);
        } else {
            throw(WebAPIException(curvar, "-- variable not found"));
        }
    }
}


#ifdef UNITTEST
main() {
    double from_time = 1323602817.0;
    double ehmgpr, em121ds0, em121ds1;
    WebAPI::_debug = 1;
  try {
    BeamFolder::_debug = 1;
    BeamFolder bf("NuMI_Physics", "http://dbweb0.fnal.gov/ifbeam",3600);
    bf.GetNamedData(from_time,"E:HMGPR",&ehmgpr);
    bf.GetNamedData(from_time,"E:M121DS[0],E:M121DS[1]",&em121ds0, &em121ds1);
    std::cout << "got value " << ehmgpr << "for E:HMGPR\n";
    std::cout << "got values " << em121ds0 << ',' << em121ds1 << "for E:M121DS[1,2]\n";
    bf.GetNamedData(1323726316528.0,"E:HMGPR",&ehmgpr);
    bf.GetNamedData(1323726318594.0,"E:HMGPR",&ehmgpr);
    std::cout << "got value " << ehmgpr << "for E:HMGPR\n";
    bf.GetNamedData(1323726318594.0,"E:HMGPR",&ehmgpr);
    std::cout << "got value " << ehmgpr << "for E:HMGPR\n";
  } catch (WebAPIException we) {
       std::cout << "got exception:" << &we << "\n";
  }

}
#endif
