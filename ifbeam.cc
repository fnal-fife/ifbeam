#include <fstream>
#include <iostream>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <iomanip>
#include "ifbeam.h"
#include "utils.h"

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
    std::vector<std::string>::iterator it
    int i,j;
    std::string st;

    if (for_time >= _cache_start && for_time <= _cache_end ) {
        // we're already in the cache...
        return;
    }

    _values.clear();
    _n_values.clear();
    _cache_slot = -1;

    std::stringstream varurl;
    varurl << _url << "/Data/data" 
           << "?v=" << *it 
           << "&e=" << _event 
           << "&t0=" << time_string(for_time)
           << "&t1=" << time_string(for_time + _time_width);
    WebAPI s(varurl.str());
    getline(s.data(), st);
    j = 0;
    _n_values[i] = 0;
    while( !s.data().eof() ) {
	getline(s.data(), st);
	_values[i] = st;
	_n_values++;
	j++;
    }
    _cache_start = for_time;
    _cache_start = for_time + _time_width;
}

void
BeamFolder::GetNamedData(double from_time, std::string variable_list, ...) {
    std::vector<std::string> variables, values;
    std::vector<std::string>::iterator rvit, it;
    std::string curvar;
    FillCache(from_time);
    double *curdest;
    size_t bpos;
    va_list al;
    int first_time_slot;
    int array_slot;
    int search_slot;
    int found;
    double first_time;
    std:string *vallist

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
   }
   _cache_slot = first_time_slot;
   first_time = get_time(values[first_time_slot])
   
   va_start(al, variable_list);

   variables = split(variable_list);
    
   for( rvit = variables.begin(); rvit != variables.end(); rvit++) {
       search_slot = first_time_slot;
       // find varname to lookup :
       // if we have a [n] on the end, set slot to n, and 
       // make it var[]
       //  
       //
       found = 0
       bpos = rvit->find('[');
       if (bpos != string::npos) {
           array_slot = atoi(rvit->c_str()+bpos+1);
           curvar = *rvit;
           curvar[bpos+1] = ']';
       } else {
           curvar *rvit;
           array_slot = 0;
       }
    
       // the place to put the value is the next varargs parameter
       curdest = va_next(al,void*);
     

      for( rvit= get_time(_values[search_slot]) == first_time && _values[search_slot].name < curvar; rvit++) 
          ;
       if ( curvar == getname(values[search_slot])) {
          vallist = split(',', values[search_slot]);
          *curdest = atofd(vallist[slot+2);
       } else {
           throw(WebAPIException(curvar, "-- variable not found"));
       }
   }
}
