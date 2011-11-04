
class BeamFolder {
private:
    // initialization parameters
    double _time_width;
    std::string _bundle_name;
    std::string _url;


    // info gotten from bundle
    std::string _event;
    std::vector<std::string> _variable_names;

    // cached info from fetches
    double _cache_start;
    double _cache_end;
    // vector (in _variable_names order) of vectors of csv data lines 
    std::vector< std::vector< std::string > > _values;

    void FetchBundleInfo();
    void FillCache(double time);

public:
    // constructor
    BeamFolder(std::string bundle_name, std::string url, double time_width);

    // accessor
    void GetNamedData(double from_time, std::string variable_list, ... );
};
