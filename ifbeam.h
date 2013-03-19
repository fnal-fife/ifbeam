#ifndef IFBEAM_H
#define IFBEAM_H
#include <string>
#include <vector>
#include "../util/WebAPI.h"

namespace ifbeam_ns {

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
    void find_name(int &first_time_slot, double &first_time, int &search_slot, std::string curvar);
    void find_first(int &first_time_slot, double &first_time, double when);

    double _valid_window;
public:
    static int _debug;

    // constructor
    BeamFolder(std::string bundle_name, std::string url, double time_width);

    // accessors
    void GetNamedData(double from_time, std::string variable_list, ... ) throw(WebAPIException);
    std::vector<double> GetNamedVector(double when,  std::string variable_name, double *actual_time = 0) throw(WebAPIException);

    // info about what is in cache...
    double GetCacheStartTime(){ return _cache_start; };
    double GetCacheEndTime(){ return _cache_end; };
    std::vector<double> GetTimeList();
    std::vector<std::string> GetDeviceList();
    void setValidWindow(double);
    double getValidWindow();
};

}

using namespace ifbeam_ns;
#endif //IFBEAM_H
