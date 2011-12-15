#include <string>
#include <vector>
#include "WebAPI.h"

class BeamFolder {
private:
    // initialization parameters
    double _time_width;
    std::string _bundle_name;
    std::string _url;


    // cached info from fetches
    double _cache_start;
    double _cache_end;

    // cached search slot
    int _cache_slot;
    double _cache_slot_time;

    // vector (in _variable_names order) of vectors of csv data lines 
    std::vector<std::string> _values;
    int _n_values;

    void FillCache(double time) throw(WebAPIException);
    double slot_time(int n);
    std::string slot_var(int n);
    double slot_value(int n, int j);
   

public:
    static int _debug;

    // constructor
    BeamFolder(std::string bundle_name, std::string url, double time_width);

    // accessor
    void GetNamedData(double from_time, std::string variable_list, ... ) throw(WebAPIException);

    double getCacheStartTime(){ return _cache_start; };
    double getCacheEndTime(){ return _cache_end; };

};
