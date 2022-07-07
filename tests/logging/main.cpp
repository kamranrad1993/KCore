#include <debuging/logging.h>
#include <debuging/call_stace_trace.h>
#include <iostream>
#include <debuging/signal_handler.h>

using namespace KCore;

void f2()
{
    LOG("1");
    LOG_TRACE("2");
    LOG_ERROR_BACK_TRACE(5, "3");
    LOG_ERROR_EXIT(5, 0, "4");
}

void f1()
{
    f2();
}

int main(int argc, char **argv)
{
    // REGISTER_ALL_SIGNAL_TRACE_BACK();
    ASSERT(false, "Assert Test 1");
    ASSERT(true, "Assert Test 1");


    f1();
    LOG_ERROR_EXIT(5, 3, "4");
    raise(2);

    LOG("end");

    return EXIT_SUCCESS;
}