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
    // REGISTER_ALL_SIGNAL_TRACE_BACK();
    // Socket s("www.google.com", 80);
    // Socket s("localhost", 65432);
    Socket *s = new Socket("127.0.0.1", 8000);
    int connection_result = s->connect();
    if (connection_result < 0)
    {
        delete s;
        // LOG_ERROR_EXIT(5, 1, "cant connect: ", connection_result);

        return connection_result;
    }

    LOG("connection result : ", connection_result);

    char *msg_1 = "msg_1";
    char *msg_2 = "msg_2";
    s->async_send((void *)msg_1, strlen(msg_1));
    sleep(1);
    s->send((void *)msg_2, strlen(msg_2));
    sleep(1);
    s->disconnect();
    delete s;
    LOG("++++++++++++++++++++");
    return 0;
}