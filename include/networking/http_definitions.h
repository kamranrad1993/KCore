#pragma once

#include <iostream>
#include <string>
#include <tools/istream_tools.h>

namespace KCore
{
    using namespace std;

#define SPACE           " "
#define END_LINE        "\r\n"
#define END_HEADER      "\r\n\r\n"
#define COLON           ":"
#define COLON_SPACE     ": "

    struct HttpVersion
    {
        string value;

        friend ostream &operator<<(ostream &out, const HttpVersion &instance)
        {
            return out << instance.value;
        }

        friend istream &operator>>(istream &in, HttpVersion &instance)
        {
            IStreamSplitter::readUntil(in, instance.value, SPACE);
            return in;
        }
    };
#define HTTP_VERSION_1_1 \
    (HttpVersion) { "HTTP/1.1" }
}