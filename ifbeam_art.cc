#include "ifbeam_art.h"

ifbeam_art::ifbeam_art( fhicl::ParameterSet const & cfg, art::ActivityRegistry &r) {
     ;
}
        
std::auto_ptr<BeamFolder>
ifbeam_art::getBeamFolder(std::string bundle_name, std::string url, double time_width) {
    std::auto_ptr<BeamFolder> res(new BeamFolder(bundle_name, url, time_width));
    return res;
}
