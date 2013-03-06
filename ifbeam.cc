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
#include "../util/utils.h"
#include "../util/WebAPI.h"
#include <math.h>


namespace ifbeam_ns {

int BeamFolder::_debug;

BeamFolder::BeamFolder(std::string bundle_name, std::string url, double time_width) {
    _time_width = time_width;
    _bundle_name =  bundle_name;
    _url = url;
    _cache_start = 0.0;
    _cache_end = 0.0;
    _cache_slot = -1;
    _valid_window = 60.0;
}

void 
BeamFolder::setValidWindow(double w) {
   _valid_window = w;
}

double 
BeamFolder::getValidWindow() {
   return _valid_window;
}

void
BeamFolder::FillCache(double when) throw(WebAPIException) {
    std::vector<std::string>::iterator it;
    std::string st;

    if (when >= _cache_start && when < _cache_end ) {
        // we're already in the cache...
        return;
    }

    // cache is flushed...
    _values.clear();
    // we average 15 lines/second so preallocate some space
    _values.reserve(int(_time_width*15));
    _n_values = 0;
    _cache_slot = -1;

    std::stringstream varurl;
    
    varurl << _url << "/data/data" 
           << "?b=" << _bundle_name
           << std::setiosflags(std::ios::fixed) 
           << "&t0=" << when
           << "&t1=" << when + _time_width;

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
    _cache_start = when;
    _cache_end = when + _time_width;
}

double 
BeamFolder::slot_time(int n) {
   return atof(_values[n].c_str())/1000.0;
}

std::string 
BeamFolder::slot_var(int n) {
     std::string s = _values[n];
     size_t p1, p2;
     p1 = s.find(',');
     p2 = s.find(',',p1+1);
     return s.substr(p1+1,p2-p1-1);
}

double 
BeamFolder::slot_value(int n, int j) {
     std::string s = _values[n];
     int i;
     size_t p1,p2;
     p2=0;
     for( i = 0; i < j+4 ; i++ ) {
         p1 = p2;
         p2 = s.find(',',p1+1); 
     }
     if (p1 == std::string::npos) {
          throw(WebAPIException("not that many values in vector",""));
     }
     return atof(s.substr(p1+1,p2-p1).c_str());
}

int time_eq(double x, double y) { return fabs(x -  y) < .0001; }

void
BeamFolder::find_first(int &first_time_slot, double &first_time, double when) {

    if (_cache_slot >= 0 && time_eq(_cache_slot_time,when) ) {

        _debug && std::cout << "finding cached slot: " << _cache_slot << "\n";

        first_time_slot = _cache_slot;
        first_time = slot_time(first_time_slot);

    } else {

	// start proportionally through the data and search for the time boundary
	// nearest the time we are looking for

	first_time_slot = int(_n_values * (when - _cache_start) / (_cache_end - _cache_start));

	while( first_time_slot < _n_values-1 && slot_time(first_time_slot) < when ) {
	   first_time_slot++;
	}
	while( first_time_slot > 0 && slot_time(first_time_slot) > when ) {
	   first_time_slot--;
	}

        _debug && std::cout << "after scans: " << first_time_slot 
		            << "data: " << _values[first_time_slot] <<"\n";

        // pick the closer time
        if (first_time_slot < _n_values-1 && 
            fabs(slot_time(first_time_slot) - when) > fabs(slot_time(first_time_slot+1) - when) ){

            _debug && std::cout << "switching from reference time:" 
				<< slot_time(first_time_slot) << "\n";
            first_time_slot = first_time_slot+1;
        }
        first_time = slot_time(first_time_slot);

        _debug && std::cout << "picked reference time:" << first_time << "\n";
       
        // find the start of the picked time
	while( first_time_slot > 1 && time_eq(slot_time(first_time_slot-1),first_time) ) {
	   first_time_slot--;
	}

        // chache this result
        _cache_slot = first_time_slot;
        _cache_slot_time = when;
    }
}

void
BeamFolder::find_name(int &first_time_slot, double &first_time, int &search_slot, std::string curvar) {
        search_slot = first_time_slot;
 
        // scan for named variable with this time
        _debug && std::cout << "searching for var: " << curvar << "\n";
        _debug && std::cout << "checking slot: " << search_slot << " name: " << slot_var(search_slot) << " value: " << _values[search_slot] <<"\n";
 
        if (search_slot < _n_values-1)
            _debug && std::cout << "true?" 
	    		    << time_eq(slot_time(search_slot+1),first_time) << "\n";
            _debug && std::cout << "true?" 
			    << (slot_var(search_slot) < curvar)<<  "\n";

        while( search_slot < _n_values-1 && time_eq(slot_time(search_slot+1),first_time) && slot_var(search_slot) < curvar)  {

            search_slot++;

            _debug && std::cout << "checking slot: " << search_slot 
				<< " name: " << slot_var(search_slot) 
				<< " value: " << _values[search_slot] <<"\n";

            _debug && std::cout << "true?" 
				<< time_eq(slot_time(search_slot+1), first_time) << "\n";

            _debug && std::cout << "true?" 
				<< (slot_var(search_slot) < curvar)<<  "\n";
         }

        _debug && std::cout << "after namesearch, search_slot is : " << search_slot 
			<< "data: " << _values[search_slot] <<"\n";
}
void
BeamFolder::GetNamedData(double when, std::string variable_list, ...)  throw(WebAPIException) {
    std::vector<std::string> variables, values;
    std::vector<std::string>::iterator rvit, it;
    std::string curvar;
    double *curdest;
    size_t bpos;
    va_list al;
    int first_time_slot;
    int array_slot;
    int search_slot;
    double first_time;

    _debug && std::cout << "looking for time" <<  when << "\n";
    // fetch data into cache (if needed)
    FillCache(when);

    // find first slot with this time; may have it cached...


    find_first(first_time_slot, first_time, when);

    _debug && std::cout << "after scans, first_time_slot is : " 
                        << first_time_slot << "data: " 
                        << _values[first_time_slot] <<"\n";
   
    va_start(al, variable_list);

    variables = split(variable_list,',');
    
    for( rvit = variables.begin(); rvit != variables.end(); rvit++) {


        // find varname to lookup :
        // if we have a [n] on the end, set slot to n, and 
        // make it var[]
        //  
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

     
        // the place to put the value is the next varargs parameter
        curdest = (double *) va_arg(al,void*);



        find_name(first_time_slot, first_time, search_slot, curvar);


        if ( curvar == slot_var(search_slot)) {
           std::vector <std::string> vallist;
           
           *curdest = slot_value(search_slot,array_slot);
        } else {
            throw(WebAPIException(curvar, "-- variable not found"));
        }
    }
}

std::vector<double> 
BeamFolder::GetNamedVector(double when, std::string variable_name) throw (WebAPIException) {
    double first_time;
    int first_time_slot;
    int search_slot;
    double value;
    int i;
    std::vector<double> res;
   
    FillCache(when);

    find_first(first_time_slot, first_time, when);
    find_name(first_time_slot, first_time, search_slot, variable_name);

    if ( variable_name != slot_var(search_slot)) {
        throw(WebAPIException(variable_name, "-- variable not found"));
    }

    if ( fabs(slot_time(search_slot) - when) > _valid_window ) {
        throw(WebAPIException(variable_name, "-- only stale data found"));
    }

    //  keep looking for a value until we get an exception
    //  
    for (i = 0; i < 10000; i++ ) {
       try {
           value = slot_value(search_slot, i);   
           res.push_back(value);
       } catch (WebAPIException e) {
           break;
       }
   }
   return res;
}
 

std::vector<double> 
BeamFolder::GetTimeList() {
   std::vector<double> res;
   std::string lookfor;

   // happily assume first variable in first sample appears
   // in each sample, so find all the times for which  it appears
   lookfor = slot_var(0);
   for(int i = 0; i < _n_values; i++) {
       if (lookfor == slot_var(i)) {
           res.push_back(slot_time(i));
       }
   }
   return res;
}

// hmm...

std::vector<std::string> 
BeamFolder::GetDeviceList() {
   std::vector<std::string> res;
   std::string lookfor;
   // happily assume the second time you see the first variable,
   // you've seen everybody once.
   lookfor = slot_var(0);
   res.push_back(lookfor);
   for(int  i = 1; i < _n_values; i++) {
        if (lookfor == slot_var(i)) {
            break;
        }
        res.push_back(slot_var(i));
   }
   return res;
}

}

#ifdef UNITTEST
int
main() {
    double when = 1323722800.0;
    double ehmgpr, em121ds0, em121ds5;
    WebAPI::_debug = 1;
    BeamFolder::_debug = 1;
    std::string teststr("1321032116708,E:HMGPR,TORR,687.125");

    std::cout << std::setiosflags(std::ios::fixed);
 
  try {
    // BeamFolder bf("NuMI_Physics", "http://dbweb0.fnal.gov/ifbeam",3600);
    BeamFolder bf("NuMI_Physics", "http://dbweb3.fnal.gov:8080/ifbeam",3600);
    bf.GetNamedData(when,"E:HMGPR",&ehmgpr);
    bf.GetNamedData(when,"E:M121DS[0],E:M121DS[5]",&em121ds0, &em121ds5);
    std::cout << "got value " << ehmgpr << "for E:HMGPR\n";
    std::cout << "got values " << em121ds0 << ',' << em121ds5 << "for E:M121DS[0,5]\n";
 
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
    
    bf.GetNamedData(1323726316.528,"E:HMGPR",&ehmgpr);
    std::cout << "got value " << ehmgpr << "for E:HMGPR\n";
    bf.GetNamedData(1323726318.594,"E:HMGPR",&ehmgpr);
    std::cout << "got value " << ehmgpr << "for E:HMGPR\n";
    bf.GetNamedData(1323726318.594,"E:HMGPR",&ehmgpr);
    std::cout << "got value " << ehmgpr << "for E:HMGPR\n";
    std::cout << "Done!\n";
  } catch (WebAPIException we) {
       std::cout << "got exception:" << &we << "\n";
  }

}
#endif
