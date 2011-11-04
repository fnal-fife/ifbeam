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
    _time_width = time_width
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
BeamFolder::FillCache(double time) {

}

void
BeamFolder::GetNamedData(double from_time, std::string variable_list, ...) {
   FillCache(from_time);
}
