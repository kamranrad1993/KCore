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
#include <tools/memory_streambuf.h>

using namespace std;
using namespace KCore;

string read_stream(iostream &str, size_t start, size_t last)
{
    string result;
    istream_iterator<char> begin(str);
    istream_iterator<char> end;
    for (istream_iterator<char> it = begin; it != end; ++it)
    {
        // LOG(str.tellp(), str.tellg());
        if (str.tellg() >= last)
        {
            LOG("end", str.tellg());
            break;
        }
        else if (str.tellg() >= start)
        {
            LOG("it",*it);
            result += *it;
        }
        else
        {
            LOG("skip", str.tellg());
        }
    }
    return result;
}

int main(int argc, char **argv)
{
    // KCore::streambuf buf;
    KCore::memory_streambuf buf(128);
    iostream iostr(&buf);
    // LOG(iostr.tellp(), iostr.tellg());
    iostr << "h i";
    // LOG(iostr.tellp(), iostr.tellg());
    iostr << "123";
    // LOG(iostr.tellp(), iostr.tellg());
    iostr << "456";
    // LOG(iostr.tellp(), iostr.tellg());
    iostr << "789";
    // LOG(iostr.tellp(), iostr.tellg());

    iostr.unsetf(ios_base::skipws);

    // // LOG(buf.get_gptr());
    // iostr.seekg(1);
    // // iostr.seekp(0);
    // LOG(read_stream(iostr, 0, 3));
    // LOG(read_stream(iostr, 0, 5));

    // iostr.seekp(3);
    // iostr.seekg(3);
    LOG(iostr.tellp(), " ====== ", iostr.tellg());
    istream_iterator<char> begin(iostr);
    istream_iterator<char> end;
    string str;
    int counter = 0;
    for (istream_iterator<char> it = begin; it != end; ++it)
    {
        str += *it;
        counter++;
        if (counter >= 2)
            break;
    }
    LOG(str);
    LOG(iostr.tellp(), " ====== ", iostr.tellg());

    istream_iterator<char> begin2(iostr);
    istream_iterator<char> end2;
    str.clear();
    for (istream_iterator<char> it = begin2; it != end2; ++it)
    {
        if (counter >= 5)
            break;
        str += *it;
        counter++;
    }
    LOG(str);
    LOG(iostr.tellp(), " ====== ", iostr.tellg());

    return 0;
}
