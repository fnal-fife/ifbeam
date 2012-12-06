#include "ifdh.h"
#include "utils.h"
#include <string>
#include <sstream>
#include <iostream>
#include <../numsg/numsg.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

using namespace std;

namespace ifdh_ns {

string cpn_loc  = "/grid/fermiapp/minos/scripts/cpn";
string fermi_gsiftp  = "gsiftp://fg-bestman1.fnal.gov:2811";
string bestmanuri = "srm://fg-bestman1.fnal.gov:10443";

string datadir() {
    stringstream dirmaker;
    
    dirmaker << (
       getenv("_CONDOR_SCRATCH_DIR")?getenv("_CONDOR_SCRATCH_DIR"):
       getenv("TMPDIR")?getenv("TMPDIR"):
       "/var/tmp"
    )
             << "/ifdh_" << getppid();

    if ( 0 != access(dirmaker.str().c_str(), W_OK) ) {
        mkdir(dirmaker.str().c_str(),0700);
    }
    return dirmaker.str().c_str();
}

int
ifdh::_debug = 0;

int
ifdh::cleanup() {
    string cmd("rm -rf ");
    cmd = cmd + datadir();
    return system(cmd.c_str());
}


// file io

int 
ifdh::cp(string src_path, string dest_path) {
    stringstream cmd;
    int dest_dir_loc;

    //
    // pick out the directory component in the destination
    //   
    dest_dir_loc = dest_path.rfind("/");
    //
    // if we can access source file for read and destination director for write
    // 
    if ( 0 == getenv("IFDH_FORCE_SRM") && 0 == access(src_path.c_str(), R_OK) && 0 == access(dest_path.substr(0,dest_dir_loc).c_str(), W_OK) ) {
        cmd << "/bin/sh " << cpn_loc << " " << src_path << " " << dest_path;
        // otherwise, use srmpcp
    } else {
        cmd << "srmcp";

        if ( 0 == access(src_path.c_str(),R_OK) ) {
	   cmd << " file:///" << src_path;  
        } else {
	   cmd << " " << bestmanuri << src_path;  
        }

        if (0 == access(dest_path.substr(0,dest_dir_loc).c_str(), W_OK) ) {
	   cmd << " file:///" << dest_path;  
        } else {
	   cmd << " " << bestmanuri  << dest_path;  
        }
    }
    _debug && cout << "running: " << cmd.str() << "\n";
    return system(cmd.str().c_str());
}

string 
ifdh::localPath( string src_uri ) {
    int baseloc = src_uri.rfind("/") + 1;
    return datadir() + "/" + src_uri.substr(baseloc);
}

string 
ifdh::fetchInput( string src_uri ) {
    stringstream cmd;
    stringstream err;
    string path;
    int res, res2;
    int p1, p2;

    if (src_uri.substr(0,7) == "file://") {
	cmd << "/bin/sh " << cpn_loc 
            << " " << src_uri.substr(7) 
            << " " << localPath( src_uri )
            << " >&2" ;
        _debug && cout << "running: " << cmd.str() << "\n";
        res = system(cmd.str().c_str());
        _debug && cout << "res is: " << res << "\n";
        p1 = cmd.str().rfind(" ");
        p2 = cmd.str().rfind(" ", p1-1);
        path = cmd.str().substr(p2+1, p1 - p2 -1);
        res2 = access(path.c_str(), R_OK);
        _debug && cout << "access res is: " << res2 << "\n";
        if (res != 0 || res2 != 0 ) {
            err << "exit code: " << res << " errno: " << errno << "path: " << path << "access:" << res2;
            throw(WebAPIException("cpn falied:", err.str().c_str() ));
        }
        _debug && cout << "returning:" << path;
        return path;
    }
    if (src_uri.substr(0,6) == "srm://") {
        cmd << "srmcp" 
            << " " << src_uri 
            << " " << "file:///" << localPath( src_uri )
            << " >&2" ;
        _debug && cout << "running: " << cmd.str() << "\n";
        res = system(cmd.str().c_str());
        _debug && cout << "res is: " << res << "\n";
        p1 = cmd.str().rfind(" ");
        p2 = cmd.str().rfind(" ", p1-1);
        path = cmd.str().substr(p2 + 8 , p1 - p2 - 8);
        res2 = access(path.c_str(), R_OK);
        _debug && cout << "access res is: " << res2 << "\n";
        if (res != 0 || res2 != 0) {
            err << "exit code: " << res << " errno: " << errno << "path: " << path << "access:" << res2;
            throw(WebAPIException("srmcp falied:", err.str().c_str() ));
        }
        _debug && cout << "returning:" << path;
        return path;
    }
    throw(WebAPIException("Unknown uri type",src_uri.substr(0,8)));
}

// file output
int 
ifdh::addOutputFile(string filename) {
    fstream outlog((datadir()+"/output_files").c_str(), ios_base::app|ios_base::out);
    outlog << filename << "\n";
    outlog.close();
    return 1;
}

int 
ifdh::copyBackOutput(string dest_dir) {
    stringstream cmd;
    string line;
    stringstream err;
    int res;
    string outfiles_name;
    // int baseloc = dest_dir.find("/") + 1;
    outfiles_name = datadir()+"/output_files";
    fstream outlog(outfiles_name.c_str(), ios_base::in);
    if ( outlog.fail()) {
       return 0;
    }

    if (0 == access(dest_dir.c_str(), W_OK) && 0 == getenv("IFDH_FORCE_SRM") && 0 == getenv("IFDH_FORCE_IFGRIDFTP")) {
        // destination is visible, so use cpn
	cmd << "/bin/sh " << cpn_loc;
        while (!outlog.eof() && !outlog.fail()) {
            getline(outlog,line);
            cmd << " " << line;
        }
        cmd << " " << dest_dir;

    } else {

        struct hostent *hostp;
        std::string gftpHost("if-gridftp-");
        gftpHost.append(getexperiment());

        if (0 != (hostp = gethostbyname(gftpHost.c_str())) && 0 == getenv("IFDH_FORCE_SRM") ) {
            //  if experiment specific gridftp host exists, use it...
            
            cmd << "globus_url_copy";
            while (!outlog.eof() && !outlog.fail()) {
		getline(outlog, line);
		cmd << " file:///" << line;
	    }
            cmd << " ftp://" << gftpHost << dest_dir;

        } else {
            // destination is not visible, use srmcp
            
	    cmd << "srmcp";
            while (!outlog.eof() && !outlog.fail()) {
		getline(outlog, line);
		cmd << " file:///" << line;
	    }
	    cmd << " " << bestmanuri << dest_dir;
       }
    }
    res = system(cmd.str().c_str());
    if (res != 0) {
        err << "command: '" << cmd << "'exit code: " << res;
        throw(WebAPIException("copy  falied:", err.str().c_str() ));
    }
    return 1;
}

// logging
int 
ifdh::log( string message ) {
  if (!numsg::getMsg()) {
      numsg::init(getexperiment(),1);
  }
  numsg::getMsg()->printf("ifdh: %s", message.c_str());
  return 0;
}

int 
ifdh::enterState( string state ){
  numsg::getMsg()->start(state.c_str());
  return 1;
}

int 
ifdh::leaveState( string state ){
  numsg::getMsg()->finish(state.c_str());
  return 1;
}

WebAPI * 
do_url_2(int postflag, va_list ap) {
    stringstream url;
    stringstream postdata;
    string urls;
    string postdatas;
    const char *sep = "";
    char * name;
    char * val;

    val = va_arg(ap,char *);
    if (*val == 0) {
        throw(WebAPIException("Environment variable IFDH_BASE_URI not set!",""));
    }
    while (strlen(val)) {
        url << sep << val;
        val = va_arg(ap,char *);
        sep = "/";
    }

    sep = postflag? "" : "?";

    name = va_arg(ap,char *);
    val = va_arg(ap,char *);
    while (strlen(name)) {
        (postflag ? postdata : url)  << sep << name << "=" << val;
        sep = "&";
        name = va_arg(ap,char *);
        val = va_arg(ap,char *);
    }
    urls = url.str();
    postdatas= postdata.str();

    if (ifdh::_debug) cout << "calling WebAPI with url: " << urls << " and postdata: " << postdatas;
    if (ifdh::_debug) cout.flush();

    return new WebAPI(urls, postflag, postdatas);
}

int
do_url_int(int postflag, ...) {
    va_list ap;
    int res;
    va_start(ap, postflag);
    WebAPI *wap = do_url_2(postflag, ap);
    res = wap->getStatus() - 200;
    delete wap;
    return res;
}


string
do_url_str(int postflag,...) {
    va_list ap;
    string res("");
    string line;
    va_start(ap, postflag);
    WebAPI *wap = do_url_2(postflag, ap);
    while (!wap->data().eof()) {
      getline(wap->data(), line);
      res = res + line;
    }
    delete wap;
    return res;
}

vector<string>
do_url_lst(int postflag,...) {
    va_list ap;
    string line;
    vector<string> res;
    va_start(ap, postflag);
    WebAPI *wap = do_url_2(postflag, ap);
    while (!wap->data().eof()) {
        getline(wap->data(), line);
        res.push_back(line);
    }
    delete wap;
    return res;
}

//datasets
int 
ifdh::createDefinition( string name, string dims, string user, string group) {
  return do_url_int(1,_baseuri.c_str(),"createDefinition","","name",name.c_str(), "dims", WebAPI::encode(dims).c_str(), "user", user.c_str(),"group", group.c_str(), "","");
}

int 
ifdh::deleteDefinition( string name) {
  return  do_url_int(1,_baseuri.c_str(),"deleteDefinition","","name", name.c_str(),"","");
}

string 
ifdh::describeDefinition( string name) {
  return do_url_str(0,_baseuri.c_str(),"describeDefinition", "", "name", name.c_str(), "","");
}

vector<string> 
ifdh::translateConstraints( string dims) {
  return do_url_lst(0,_baseuri.c_str(),"translateConstraints", "", "dims", WebAPI::encode(dims).c_str(), "format","plain", "","" );
}

// files
vector<string> 
ifdh::locateFile( string name) {
  return do_url_lst(0,_baseuri.c_str(), "locateFile", "", "file", name.c_str(), "", "" );  
}

string ifdh::getMetadata( string name) {
  return  do_url_str(0, _baseuri.c_str(),"getMetadata", "", "name", name.c_str(), "","");
}

//
string 
ifdh::dumpStation( string name, string what ) {
  return do_url_str(0,_baseuri.c_str(),"dumpStation", "", "station", name.c_str(), "dump", what.c_str(), "","");
}

// projects
string ifdh::startProject( string name, string station,  string defname_or_id,  string user,  string group) {
  return do_url_str(1,_baseuri.c_str(),"startProject","","name",name.c_str(),"station",station.c_str(),"defname",defname_or_id.c_str(),"username",user.c_str(),"group",group.c_str(),"","");
}

string 
ifdh::findProject( string name, string station){
   return do_url_str(0,_baseuri.c_str(),"findProject","","name",name.c_str(),"station",station.c_str(),"","");
}

string 
ifdh::establishProcess( string projecturi, string appname, string appversion, string location, string user, string appfamily , string description , int filelimit ) {
  return do_url_str(1,projecturi.c_str(),"establishProcess", "", "appname", appname.c_str(), "appversion", appversion.c_str(), "deliverylocation", location.c_str(), "username", user.c_str(), "appfamily", appfamily.c_str(), "description", description.c_str(), "", "");
}

string ifdh::getNextFile(string projecturi, string processid){
  return do_url_str(1,projecturi.c_str(),"processes",processid.c_str(),"getNextFile","","","");
}

string ifdh::updateFileStatus(string projecturi, string processid, string filename, string status){
  return do_url_str(1,projecturi.c_str(),"processes",processid.c_str(),"updateFileStatus","","filename", filename.c_str(),"status", status.c_str(), "","");
}

int 
ifdh::endProcess(string projecturi, string processid) {
  cleanup();
  return do_url_int(1,projecturi.c_str(),"processes",processid.c_str(),"endProcess","","","");
}

string 
ifdh::dumpProcess(string projecturi, string processid) {
  return do_url_str(1,projecturi.c_str(),"processes",processid.c_str(),"dumpProcess","","","");
}

int ifdh::setStatus(string projecturi, string processid, string status){
  return do_url_int(1,projecturi.c_str(),"processes",processid.c_str(),"setStatus","","status",status.c_str(),"","");
}

int ifdh::endProject(string projecturi) {
  return do_url_int(1,projecturi.c_str(),"endProject","","","");
}

ifdh::ifdh(std::string baseuri) : _baseuri(baseuri) { ; }

void
ifdh::set_base_uri(std::string baseuri) { 
    _baseuri = baseuri; 
}

}
