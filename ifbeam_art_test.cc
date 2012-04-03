#include "ifbeam_art.h"

namespace art {
  class ActivityRegistry {
       int _dummy;
  };
}
namespace fhicl {
  class ParameterSet {
       int _dummy;
  };
}

main() {
    fhicl::ParameterSet cfg; 
    art::ActivityRegistry r;
    double ehmgpr;
    double when = 1323722800.0;

    ifbeam_art ia(cfg, r);
    BeamFolder::_debug = 1;
    std::auto_ptr<BeamFolder> pbf( ia.getBeamFolder("NuMI_Physics", "http://dbweb3.fnal.gov:8080/ifbeam",3600));

    pbf->GetNamedData(when,"E:HMGPR",&ehmgpr);
}
