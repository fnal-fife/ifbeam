#include <sys/time.h>
#include <vector>
#include <list>
#include <sstream>
#include <fstream>
#include "WebAPI.h"

class Folder {
public:
     struct tk { double when; long int key; };
private:
     double _cache_start;
     double _cache_end;
     long int _cache_key;
     std::vector<std::string> _columns;
     std::vector<std::string> _types;
     std::vector<std::string> _cache_data;
     int _n_datarows;
     std::vector<tk> _times;
     std::string _url;
     std::string _foldername;
     std::string _tag;

     int parse_fields(std::vector<std::string> names,const char *, va_list);

public:
     static int _debug;
     // internal in back of getChannelData1
     std::vector<tk> 
	getTimes(double when) throw(WebAPIException);    // gets list of times in week surrounding when

     void fetchData(double when) throw(WebAPIException); // gets data for a time in cache
     void fetchData(long key) throw(WebAPIException);    // gets data for a time in cache

     Folder( std::string name, std::string url, std::string tag = "") throw(WebAPIException); // bookkeeping...
     int getNamedChannelData_va(double t, int chan, std::vector<std::string> names,va_list al) throw(WebAPIException); // fetches data 
     int getNamedChannelData(double t, int chan, std::string names,...) throw(WebAPIException); // fetches data 
     int getChannelData(double t, int chan, ...) throw(WebAPIException); // fetches data 
     long int getKey(double) throw(WebAPIException);
};

