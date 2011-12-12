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
    WebAPI s(varurl.str());

    getline(s.data(), st); // header...

    while( !s.data().eof() ) {
	getline(s.data(), st);
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
     return s.substr(p1,p2);
}

double
get_value(std::string s, int n) {
     int i;
     size_t p1,p2;
     p2=0;
     for( i = 0; i < n+2 ; i++ ) {
         p1 = p2;
         p2 = s.find(',',p1); 
     }
     return atof(s.substr(p1,p2).c_str());
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

    // fetch data into cache (if needed)
    FillCache(from_time);

    // find first slot with this time; may have it cached...
    if (_cache_slot >= 0 && get_time(_values[_cache_slot]) == from_time ) {
         first_time_slot = _cache_slot;
    } else {
	// start proportionally through the data and search for the time we want
	first_time_slot = _n_values * (from_time - _cache_start) / (_cache_end - _cache_start);
	while( first_time_slot < _n_values && get_time(_values[first_time_slot]) < from_time ) {
	   first_time_slot++;
	}
	while( first_time_slot > 0 && get_time(_values[first_time_slot]) > from_time ) {
	   first_time_slot--;
	}
        first_time = get_time(_values[first_time_slot]);
	while( first_time_slot > 0 && get_time(_values[first_time_slot]) == first_time ) {
	   first_time_slot--;
	}
        _cache_slot = first_time_slot;
    }
   
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
        } else {
            curvar =  *rvit;
            array_slot = 0;
        }
     
        // the place to put the value is the next varargs parameter
        curdest = (double *) va_arg(al,void*);
      
 
       // scan for named variable with this time
       while( get_time(_values[search_slot]) == first_time && get_name(_values[search_slot]) < curvar) 
           search_slot++;

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
    double from_time = 1323722817.0;
    double etor101;
    WebAPI::_debug = 1;
    BeamFolder bf("NuMI_Physics", "http://dbweb0.fnal.gov/ifbeam",3600);
    bf.GetNamedData(from_time,"E:TOR101",&etor101);
}
#endif

