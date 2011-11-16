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
    FetchBundleInfo();
}

void
BeamFolder::FetchBundleInfo() {
   std::string st;
   std::stringstream fullurl;

   // scrape event name and 

   fullurl << _url << "/GUI/edit_bundle?b=" << _bundle_name;
   _variable_names.clear();

   WebAPI s(fullurl.str());
   while(!s.data().eof()) {
       (void)getline(s.data(), st);
       if (string:npos != st.find("name=bundle") && string:npos == st.find(_bundle_name)) {
            throw WebAPIException("Got wrong page looking up bundle",st);
       }
       if (string:npos != st.find("name=\"event\"")) {
           pos = st.find("value=\"");
           pos2 = st.find("\"", pos + 8);
           _event = st.substr(pos+8,pos2);
       }
       if (string:npos != (pos = st.find("name=\"del_var:"))) {
           pos2 = st.find("\"", pos+15);
           _variable_names.push_back(st.substr(pos+15, pos2));
       }
    }
    s.data().close()
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

    for( i = 0, it = _variable_names.begin(); it != _variable_names.end(); it++,i++ ) {
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
            _values[i][j] = st;
            _n_values[i]++;
            j++;
        }
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
   int slot;
   int found;
   
   va_start(al, variable_list);

   variables = split(variable_list);
    
   for( rvit = variables.begin(); rvit != variables.end(); rvit++) {
       // find varname to lookup :
       // if we have a [n] on the end, set slot to n, and 
       // make it var[]
       bpos = rvit->find('[');
       if (bpos != string::npos) {
           slot = atoi(rvit->c_str()+bpos+1);
           curvar = *rvit;
           curvar[bpos+1] = ']';
       } else {
           curvar *rvit;
           slot = 0;
       }

       // the place to put the value is the next varargs parameter
       curdest = va_next(al,void*);
     
       found = 0;
       for ( i = 0, it = _variable_names.begin(); it != _variable_names.end(); it++,i++ ) {
           if ( curvar == *it ) {
               found = 1;
               // assume even distribution, guess a row proportionally
               // through the list, and linear search from there.
	       guess_row = _n_values[i] * (from_time - _cache_start) / (_cache_end - _cache_start);
               while( guess_row > 0 && get_time(_values[i][guess_row]) > from_time ) {
                   guess_row--;
               }
               while( guess_row < _n_values[i] && get_time(_values[i][guess_row]) < from_time ) {
                   guess_row++;
               }
               // we should be at the smallest row above from_time
               // if our time is closer to the one below us, go down
               // one.
               if (guess_row > 0 && 
                     (get_time(_values[i][guess_row])-from_time) > 
 		       (from_time - get_time(_values[i][guess_row-1]))) {
		   guess_row--;
	       }
	       *curdest = get_value(_values[i][guess_row], slot);
           }
           if (!found) {
               throw(WebAPIException(curvar, "-- variable not found"));
           }
       }
   }
}
