#include <wda.h>
#include <wda_singleton.h>

namespace wda_singleton {

wda_init_cleanup_singleton *
wda_init_cleanup_singleton::get_wda_init_cleanup_singleton() {
    static wda_init_cleanup_singleton actual_singleton;
    return &actual_singleton;
}

wda_init_cleanup_singleton::wda_init_cleanup_singleton() {
   wda_global_init();
}

wda_init_cleanup_singleton::~wda_init_cleanup_singleton() {
   wda_global_cleanup();
}

}
