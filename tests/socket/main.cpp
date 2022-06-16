#include <climits>
#include <debuging/call_stace_trace.h>
#include <debuging/logging.h>
#include <debuging/signal_handler.h>
#include <fstream>
#include <iostream>
#include <networking/socket/socket.h>
#include <random>
#include <tools/istream_tools.h>

using namespace std;
using namespace KCore;

int main(int argc, char **argv)
{
    REGISTER_ALL_SIGNAL_TRACE_BACK();
    // Socket s("www.google.com", 80);
    // Socket s("localhost", 65432);
    Socket s("127.0.0.1", 65432);
    s.on_receive = [](void *data, size_t len)
    {
        LOG(len, "  ", (char *)data);
    };
    s.on_disconnect = [](int code)
    {
        LOG("disconnected ", code);
    };
    LOG("connection result : ", s.connect());

    char* msg_1 = "msg_1";
    char* msg_2 = "msg_2";
    s.async_send((void*)msg_1, strlen(msg_1));
    sleep(1);
    s.send((void*)msg_2, strlen(msg_2));
    sleep(10);
    s.disconnect();

    return 0;
}