#include <functional>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
// #include <sys/types.h>
#include <arpa/inet.h>
#include <cstring>
#include <debuging/logging.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <thread>
#include <vector>

namespace KCore
{
    using namespace std;

    //https://www.ibm.com/docs/en/zos/2.5.0?topic=services-network-application-example
    class Socket
    {
    private:
        size_t buffer_size = 256;
        vector<uint8_t> buffer;
        struct sockaddr_in serv_addr;
        int socket_instance;
        bool loop_condition = false;
        thread *loop_thread;

    public:
        function<void()> on_connect;
        function<void(int)> on_disconnect;
        function<void(void *, size_t)> on_receive;

    private:
        void loop()
        {
            loop_condition = true;
            vector<uint8_t> data;
            while (loop_condition)
            {
                size_t len = recv(socket_instance, buffer.data(), buffer_size, 0);
                if (len > 0 && len < buffer_size)
                {
                    data.reserve(data.size() + len);
                    data.insert(data.end(), buffer.begin(), buffer.begin() + len);
                    void *result = malloc(data.size());
                    memcpy(result, data.data(), data.size());
                    on_receive(result, data.size());
                    data.clear();
                    data.shrink_to_fit();
                    buffer.clear();
                }
                else if (len == buffer_size)
                {
                    data.reserve(data.size() + buffer_size);
                    data.insert(data.end(), buffer.begin(), buffer.begin() + len);
                }
                else
                {
                    data.clear();
                    data.shrink_to_fit();
                    buffer.clear();
                    if (on_disconnect)
                        on_disconnect(0);
                    loop_condition = false;
                }
            }
        }

    public:
        Socket(string address, uint16_t port)
            : Socket(&address[0], address.length(), port)
        {
        }

        Socket(const char *address, int len, uint16_t port)
        {
            buffer = vector<uint8_t>(buffer_size, 0);

            socket_instance = socket(AF_INET, SOCK_STREAM, 0);

            // hostent *host = gethostbyname(address);
            // // hostent *host = gethostbyaddr_r(address, len, AF_INET);
            // LOG(host->h_addr_list[0]);
            // serv_addr.sin_family = AF_INET;
            // serv_addr.sin_addr.s_addr = inet_addr(host->h_addr_list[0]);
            // serv_addr.sin_port = htons(port);

            serv_addr.sin_family = AF_INET;
            serv_addr.sin_addr.s_addr = inet_addr(address);
            serv_addr.sin_port = htons(port);
        }

        int connect()
        {
            int result = ::connect(socket_instance, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
            if (result >= 0)
                loop_thread = new thread([this]()
                                         { this->loop(); });
            return result;
            // loop();
        }

        void send(uint8_t *data, size_t len)
        {
            ::send(socket_instance, data, len, 0);
        }

        void disconnect()
        {
            ::close(socket_instance);
        }

        ~Socket()
        {
            loop_condition = false;
            // if(loop_thread != nullptr && loop_thread->joinable())
            // {
            //     loop_thread->detach();
            //     delete loop_thread;
            // }
        }
    };
} // namespace KCore