#pragma once

#include <asio.hpp>
#include <debuging/logging.h>
#include <exception>
#include <fstream>
#include <iostream>
#include <networking/http_request.h>
#include <networking/http_response.h>
#include <tools/istream_tools.h>

namespace KCore
{
    using namespace std;
    using asio::ip::tcp;

    class SimpleHttpRequest
    {

    private:
#define RESPONSE_BUFFER_SIZE 4096
        asio::io_context io_context;
        tcp::resolver resolver;

    public:
        SimpleHttpRequest() : io_context(), resolver(io_context)
        {
        }

        shared_ptr<HttpResponse> send(RequestAddress &address, HttpRequest &request)
        {
            tcp::resolver::results_type endpoints = resolver.resolve(address.host, "http");
            tcp::socket socket(io_context);
            auto endpoint = asio::connect(socket, endpoints);

            asio::streambuf requestBuffer;
            std::ostream request_stream(&requestBuffer);
            request_stream << request;

            asio::write(socket, requestBuffer);

            size_t readSize = 0;
            asio::streambuf responseBuf;
            asio::error_code error;
            try
            {
                while (asio::read(socket, responseBuf, asio::transfer_at_least(1), error))
                {
                }
            }
            catch (std::ios_base::failure &e)
            {
                LOG(e.what());
                if (error != asio::error::eof)
                    throw e;
            }
            string strBuf = string(asio::buffer_cast<const char *>(responseBuf.data()));
            stringstream sStr(strBuf);

            HttpResponse *response = new HttpResponse();
            sStr >> *response;

            // uintptr_t pos = reinterpret_cast<uintptr_t>(response->contentBuf.get());
            // LOG(pos);
            // LOG("++++++++++++++++++");
            // LOG(*response);
            // LOG("++++++++++++++++++");
            shared_ptr<HttpResponse> ptr(response);
            return ptr;
        }
    };
}
