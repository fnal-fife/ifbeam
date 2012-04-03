#include "ifbeam.h"
#include <memory>

// ART bits...
namespace art {
  class ActivityRegistry;   // declaration only
  class ModuleDescription;  // declaration only
  class Run;		    // declaration only
}
namespace fhicl {
  class ParameterSet;       // declaration only
}

namespace ifbeam_ns {

class ifbeam_art  {
       
public:
        
        std::auto_ptr<BeamFolder> getBeamFolder(std::string bundle_name, std::string url, double time_width);

        // ART constructor...
        ifbeam_art( fhicl::ParameterSet const & cfg, art::ActivityRegistry &r);
};

}

#ifdef DEFINE_ART_SERVICE
DEFINE_ART_SERVICE(ifbeam_ns::ifbeam_art);
#endif

// this is redundant, given the ifbeam include,  but just to be clear

using namespace ifbeam_ns;
