#pragma once
#include <functional>
#include <stdio.h>
#include <string>
#include <unistd.h>
// #include <sys/types.h>
#include <cstring>
#include <debuging/logging.h>
#include <future>
#include <mutex>
#include <stdlib.h>
#include <thread>
#include <tools/streambuf.h>
#include <tools/memory_streambuf.h>
#include <vector>
#ifdef LINUX_PLATFORM
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#endif
#ifdef WINDOWS_PLATFORM
#include <iphlpapi.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#endif
namespace KCore
{
    using namespace std;

    // https://www.ibm.com/docs/en/zos/2.5.0?topic=services-network-application-example
    class Socket
    {
    private:
#ifdef LINUX_PLATFORM
        vector<sockaddr_in> serv_addrs;
        int socket_instance;
#endif
#ifdef WINDOWS_PLATFORM
        vector<addrinfo> serv_addrs;
        SOCKET socket_instance = INVALID_SOCKET;
#endif
    public:
        size_t buffer_size = 256;

    public:
        Socket(string address, uint16_t port)
            : Socket(&address[0], address.length(), port)
        {
        }

        Socket(const char *address, int len, uint16_t port)
        {
#ifdef LINUX_PLATFORM
            socket_instance = socket(AF_INET, SOCK_STREAM, 0);
            hostent *host = gethostbyname(address);

            char **ptr = host->h_addr_list;
            for (char *addr = *host->h_addr_list; addr; addr = *++ptr)
            {
                sockaddr_in serv_addr;
                serv_addr.sin_family = AF_INET;
                serv_addr.sin_port = htons(port);
                serv_addr.sin_addr.s_addr = inet_addr(inet_ntoa(*((struct in_addr *)addr)));

                serv_addrs.push_back(serv_addr);
            }

            // serv_addr.sin_family = AF_INET;
            // serv_addr.sin_addr.s_addr = inet_addr(address);
            // serv_addr.sin_port = htons(port);
#endif
#ifdef WINDOWS_PLATFORM
            buffer = vector<char>(buffer_size, 0);
            WSADATA wsaData;
            int result;
            result = WSAStartup(MAKEWORD(2, 2), &wsaData);
            if (result != 0)
            {
                LOG_ERROR_EXIT(5, result, "WSAStartup failed with error: ", result);
            }

            struct addrinfo *addr_result = NULL, hints;

            ZeroMemory(&hints, sizeof(hints));
            hints.ai_family = AF_UNSPEC;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_protocol = IPPROTO_TCP;

            result = getaddrinfo(address, to_string(port).c_str(), &hints, &addr_result);
            if (result != 0)
            {
                LOG_ERROR_EXIT(5, result, "getaddrinfo failed with error: ", result);
            }
            serv_addrs.push_back(*addr_result);
            socket_instance = socket(addr_result->ai_family, addr_result->ai_socktype, addr_result->ai_protocol);
            if (socket_instance == INVALID_SOCKET)
            {
                LOG_ERROR_EXIT(5, WSAGetLastError(), "socket failed with error: ", WSAGetLastError());
            }
#endif
        }

        int connect()
        {
#ifdef LINUX_PLATFORM
            for (sockaddr_in serv_addr : serv_addrs)
            {
                int result = ::connect(socket_instance, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
                if (result >= 0)
                {
                    return result;
                }
            }
#endif
#ifdef WINDOWS_PLATFORM
            for (addrinfo serv_addr : serv_addrs)
            {
                int result = ::connect(socket_instance, serv_addr.ai_addr, (int)serv_addr.ai_addrlen);
                if (result >= 0)
                {
                    LOG(serv_addr.ai_addr);
                    loop_thread = new thread([this]()
                                             { this->loop(); });
                    return result;
                }
            }
#endif
            return -1;
        }

        future<int> async_connect()
        {
            future<int> fobj = async(launch::async, [this]()
                                     { return this->connect(); });
            return fobj;
        }

        void send(void *data, size_t len)
        {
#ifdef LINUX_PLATFORM
            ::send(socket_instance, data, len, 0);
#endif
#ifdef WINDOWS_PLATFORM
            ::send(socket_instance, (char *)data, len, 0);
#endif
        }

        future<void> async_send(void *data, size_t len)
        {
            future<void> fobj = async(launch::async, [this, data, len]()
                                      { this->send(data, len); });
            return fobj;
        }

        shared_ptr<streambuf> receive()
        {
            streambuf *result_buf = new streambuf();
            iostream result_stream(result_buf);
            streambuf buf(buffer_size);

            int available = recv(socket_instance, buf.get_pptr(), buffer_size, MSG_PEEK);
            while (available > 0)
            {
                available = recv(socket_instance, buf.get_pptr(), available, 0);
                result_stream.write(buf.get_gptr(), available);
                // string s(buf.get_gptr(), buf.get_length());
                // LOG(s);
                buf.clear();
            }

            shared_ptr<streambuf> result(result_buf);
            return result;
        }

        future<shared_ptr<streambuf>> async_receive()
        {
            future<shared_ptr<streambuf>> fobj = async(launch::async, [this]()
                                                       { return this->receive(); });
            return fobj;
        }

        void disconnect()
        {
            ::close(socket_instance);
        }

        future<void> async_disconnect()
        {
            future<void> fobj = async(launch::async, [this]()
                                      { this->disconnect(); });
            return fobj;
        }

        ~Socket()
        {
        }
    };
} // namespace KCore