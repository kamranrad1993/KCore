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

using namespace std;
using namespace KCore;

int main(int argc, char **argv)
{
    // REGISTER_ALL_SIGNAL_TRACE_BACK();

    SimpleHttpRequest srq;
    // RequestAddress add = {"1906714720.rsc.cdn77.org", "img/cdn77-test-563kb.jpg"};
    RequestAddress add = {"127.0.0.1", "/"};
    // RequestDefinition requestDefinition(REQUEST_METHOD_GET, "img/cdn77-test-563kb.jpg", HTTP_VERSION_1_1);
    RequestDefinition requestDefinition(REQUEST_METHOD_GET, "/", HTTP_VERSION_1_1);
    HttpRequest req(requestDefinition, add);
    req.headers.push_back(REQUEST_CONNECTION_CLOSE);
    shared_ptr<HttpResponse> response = srq.send(add, req);
    // uintptr_t pos = reinterpret_cast<uintptr_t>(response->contentBuf.get());
    // LOG(pos);
    LOG(*response.get());

    return 0;
}