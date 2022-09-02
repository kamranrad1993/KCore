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

void test_insert(iostream &iostr)
{
    iostr << "hi";
    iostr << "123";
    iostr << "456";
    iostr << "789";
}

void test_seek(iostream &iostr)
{
    char c;
    iostr.clear(ios_base::eofbit);

    iostr.seekg(4);
    iostr.get(c);
    LOG("test_seek", c, iostr.tellg());

    iostr.seekg(1);
    iostr.get(c);
    LOG("test_seek", c, iostr.tellg());

    iostr.seekg(4);
    iostr.get(c);
    LOG("test_seek", c, iostr.tellg());
}

void test_iterate(iostream &iostr)
{
    iostr.clear(ios_base::eofbit);
    iostr.seekg(0, ios_base::beg);
    LOG("test_iterate", iostr.tellp(), " ====== ", iostr.tellg());
    istream_iterator<char> begin(iostr);
    istream_iterator<char> end;
    string str;
    for (istream_iterator<char> it = begin; it != end; ++it)
    {
        str += *it;
    }
    LOG("test_iterate", str);

    iostr.clear(ios_base::eofbit);
    iostr.seekg(4, ios_base::beg);
    LOG("test_iterate", iostr.tellp(), " ====== ", iostr.tellg());

    istream_iterator<char> begin2(iostr);
    istream_iterator<char> end2;
    str.clear();
    for (istream_iterator<char> it2 = begin2; it2 != end2; ++it2)
    {
        str += *it2;
    }
    LOG("test_iterate", str);
    LOG("test_iterate", iostr.tellp(), " ====== ", iostr.tellg());
}

int main(int argc, char **argv)
{
    KCore::streambuf buf;
    iostream iostr(&buf);
    iostr.unsetf(ios_base::skipws);

    test_insert(iostr);

    test_seek(iostr);

    test_iterate(iostr);

    return 0;
}
