namespace wda_singleton {

class wda_init_cleanup_singleton {

public:
    static wda_init_cleanup_singleton *get_wda_init_cleanup_singleton();
private:
    wda_init_cleanup_singleton();
    ~wda_init_cleanup_singleton();
};

}
