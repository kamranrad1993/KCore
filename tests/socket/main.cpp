#include <climits>
#include <debuging/call_stace_trace.h>
#include <debuging/logging.h>
#include <debuging/signal_handler.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <networking/socket/socket.h>
#include <random>
#include <tools/istream_tools.h>

using namespace std;
using namespace KCore;

void on_receive(void *data)
{
}

int main(int argc, char **argv)
{
    REGISTER_ALL_SIGNAL_TRACE_BACK();
    // Socket s("localhost", 65432);
    Socket s("127.0.0.1", 65432);
    // s.on_receive = on_receive;
    s.on_receive = [](void *data, size_t len)
    {
        LOG(len, "  ", (char *)data);
    };
    s.on_disconnect = [](int code)
    {
        LOG("disconnected ", code);
    };
    LOG("connection result : ", s.connect());
    sleep(10);
    s.disconnect();

    return 0;
}