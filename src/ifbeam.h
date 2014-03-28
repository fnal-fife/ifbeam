#ifndef IFBEAM_H
#define IFBEAM_H
#include <string>
#include <vector>
#include "../util/WebAPI.h"

#ifndef OLD_CACHE
#include "ifbeam_c.h"
#endif

namespace ifbeam_ns {

class BeamFolder {
protected:
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
#ifdef OLD_CACHE
    std::vector<std::string> _values;
#else
    Dataset _values;
    Tuple   _cur_row;
    int     _cur_row_num;

    Tuple   cachedGetTuple(int n);
#endif
    int _n_values;

    int _values_column;

    double slot_time(int n);
    std::string slot_var(int n);
    double slot_value(int n, int j);
    void find_name(int &first_time_slot, double &first_time, int &search_slot, std::string curvar);
    void find_first(int &first_time_slot, double &first_time, double when);

    double _valid_window;

    double _epsilon;
public:

    void set_epsilon( double e ); 

    int time_eq(double x, double y);

    static int _debug;

    // constructor
    BeamFolder(std::string bundle_name, std::string url = "", double time_width = 1200.0);
    ~BeamFolder();

    void FillCache(double time) throw(WebAPIException);
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

class BeamFolderScanner : public BeamFolder {
private: 
    int cur_slot;
public:
    BeamFolderScanner(std::string bundle, double start_time);
    int NextDataRow(double &time, std::string &name, std::vector<double> &values);
};

}

using namespace ifbeam_ns;
#endif //IFBEAM_H
