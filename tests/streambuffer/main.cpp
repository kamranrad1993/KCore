#include <climits>
#include <debuging/call_stace_trace.h>
#include <debuging/logging.h>
#include <debuging/signal_handler.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <networking/http_request.h>
#include <networking/http_response.h>
#include <networking/simple_http_request.h>
#include <random>
#include <tools/istream_tools.h>
#include <tools/streambuf.h>

using namespace std;
using namespace KCore;

int main(int argc, char **argv)
{
    KCore::streambuf buf;
    iostream iostr(&buf);
    iostr<<"hi";
    iostr<<"123";
    iostr<<"456";
    iostr<<"789";

    LOG(buf.get_gptr());

    return 0;
}
