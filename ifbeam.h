
class BeamFolder {
private:
    // initialization parameters
    double _time_width;
    std::string _bundle_name;
    std::string _url;


    // cached info from fetches
    double _cache_start;
    double _cache_end;

    int _cache_slot;
    // vector (in _variable_names order) of vectors of csv data lines 
    std::vector< std::string  > _values;
    int _n_values;

    void FillCache(double time);

public:
    // constructor
    BeamFolder(std::string bundle_name, std::string url, double time_width);

    // accessor
    void GetNamedData(double from_time, std::string variable_list, ... );
};
