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
    // REGISTER_ALL_SIGNAL_TRACE_BACK();

    SimpleHttpRequest srq;
    // RequestAddress add = {"1906714720.rsc.cdn77.org", "img/cdn77-test-563kb.jpg"};
    RequestAddress add = {"127.0.0.1", 8000, "/Downloads/"};
    // RequestAddress add = {"google.com", 80, "/"};
    RequestDefinition requestDefinition(REQUEST_METHOD_GET, "/Downloads/", HTTP_VERSION_1_1);
    HttpRequest req(requestDefinition, add);
    shared_ptr<HttpResponse> result = srq.send(add, req);
    LOG(result);

    return 0;
}
