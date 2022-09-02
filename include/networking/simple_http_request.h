#pragma once

#include <debuging/logging.h>
#include <exception>
#include <fstream>
#include <iostream>
#include <mutex>
#include <networking/http_request.h>
#include <networking/http_response.h>
#include <networking/socket/socket.h>
#include <string>
#include <strstream>
#include <tools/istream_tools.h>
#include <tools/streambuf.h>

namespace KCore
{
    using namespace std;

    class SimpleHttpRequest
    {

    public:
        SimpleHttpRequest()
        {
            
        }

        shared_ptr<HttpResponse> send(RequestAddress &address, HttpRequest &request)
        {
            Socket s(address.host, address.port);
            int connect_result = s.connect();
            if (connect_result < 0)
            {
                LOG_ERROR_EXIT(5, 1, "connect error");
            }

            KCore::streambuf req_buf;
            iostream req_stream(&req_buf);
            req_stream<<request;

            s.send(req_buf.get_gptr(), req_buf.get_length());

            HttpResponse *response = new HttpResponse();
            auto result = s.receive();
            istream istr(result.get());
            istr >> *response;

            shared_ptr<HttpResponse> ptr(response);
            return ptr;
        }
    };
}
