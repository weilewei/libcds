#include <cds/init.h>       // for cds::Initialize and cds::Terminate
#include <cds/gc/hp.h>      // for cds::HP (Hazard Pointer) SMR
#include <hpx/hpx_main.hpp>

int main(int argc, char** argv)
{
    // Initialize libcds
    cds::Initialize();

    {
        // Initialize Hazard Pointer singleton
        cds::gc::HP hpGC;

        // If main thread uses lock-free containers
        // the main thread should be attached to libcds infrastructure
        cds::threading::Manager::attachThread();

        // Now you can use HP-based containers in the main thread
        //...
    }

    // Terminate libcds
    cds::Terminate();
}