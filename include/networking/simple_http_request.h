#pragma once

#include <asio.hpp>
#include <curl/curl.h>
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
    using asio::ip::tcp;

    class SimpleHttpRequest
    {

    private:
        asio::io_context io_context;
        tcp::resolver resolver;
        // CURL *curl;

        // shared_ptr<HttpResponse> sendHttp_1_1(RequestAddress &address, HttpRequest &request)
        // {
        //     curl_easy_setopt(curl, CURLOPT_URL, address.str().c_str());
        //     CURLcode res = curl_easy_perform(curl);
        //     if (res != CURLE_OK)
        //         fprintf(stderr, "curl_easy_perform() failed: %s\n",
        //                 curl_easy_strerror(res));

        //     curl_easy_cleanup(curl);
        // }

    public:
        SimpleHttpRequest() : io_context(), resolver(io_context)
        {
            // curl = curl_easy_init();
        }

        shared_ptr<streambuf> send(RequestAddress &address, HttpRequest &request)
        {
            Socket s(address.host, address.port);
            int connect_result = s.connect();
            if (connect_result < 0)
            {
                LOG_ERROR_EXIT(5, 1, "connect error");
            }

            KCore::streambuf req_buf(128,128);
            iostream req_stream(&req_buf);
            req_stream<<request;

            s.send(req_buf.get_gptr(), req_buf.get_length());
            return s.receive();


            // HttpResponse *response = new HttpResponse();
            // s.receive(). >> *response;

            // shared_ptr<HttpResponse> ptr(response);
            // return ptr;
        }

        // shared_ptr<HttpResponse> send(RequestAddress &address, HttpRequest &request)
        // {

        //     tcp::resolver::results_type endpoints = resolver.resolve(address.host, "http");
        //     tcp::socket socket(io_context);
        //     auto endpoint = asio::connect(socket, endpoints);

        //     asio::streambuf requestBuffer;
        //     std::ostream request_stream(&requestBuffer);
        //     request_stream << request;

        //     asio::write(socket, requestBuffer);

        //     size_t readSize = 0;
        //     asio::streambuf responseBuf;
        //     asio::error_code error;
        //     try
        //     {
        //         while (asio::read(socket, responseBuf, asio::transfer_at_least(1), error))
        //         {
        //         }
        //     }
        //     catch (std::ios_base::failure &e)
        //     {
        //         LOG(e.what());
        //         if (error != asio::error::eof)
        //             throw e;
        //     }
        //     string strBuf = string(asio::buffer_cast<const char *>(responseBuf.data()));
        //     stringstream sStr(strBuf);

        //     HttpResponse *response = new HttpResponse();
        //     sStr >> *response;

        //     // uintptr_t pos = reinterpret_cast<uintptr_t>(response->contentBuf.get());
        //     // LOG(pos);
        //     // LOG("++++++++++++++++++");
        //     // LOG(*response);
        //     // LOG("++++++++++++++++++");
        //     shared_ptr<HttpResponse> ptr(response);
        //     return ptr;
        // }
    };
}
