#pragma once
#include <climits>
#include <hash_map>
#include <iosfwd>
#include <memory>
#include <networking/http_definitions.h>
#include <random>
#include <tools/format_string.h>
#include <tools/istream_tools.h>

namespace KCore
{
    using namespace std;

    enum RESPONSE_HEADER_TYPES
    {
        unknown,
        Accept_Ranges,
        Age,
        ETag,
        Location,
        Proxy_Authenticate,
        Retry_After,
        Server,
        Vary,
        WWW_Authenticate,
        Allow,
        Content_Encoding,
        Content_Language,
        Content_Length,
        Content_Location,
        Content_MD5,
        Content_Range,
        Content_Type,
        Expires,
        Last_Modified,
        extension_header,
        Date,
        Not_Initialized
    };

    const string RESPONSE_HEADER_NAMES[] =
        {
            "unknown",
            "Accept-Ranges",
            "Age",
            "ETag",
            "Location",
            "Proxy-Authenticate",
            "Retry-After",
            "Server",
            "Vary",
            "WWW-Authenticate",
            "Allow",
            "Content-Encoding",
            "Content-Language",
            "Content-Length",
            "Content-Location",
            "Content-MD5",
            "Content-Range",
            "Content-type",
            "Expires",
            "Last-Modified",
            "extension-header",
            "Date"};

    const map<string, RESPONSE_HEADER_TYPES> RESPONSE_HEADER_NAME_TO_TYPE = {
        {"Accept-Ranges", RESPONSE_HEADER_TYPES::Accept_Ranges},
        {"Age", RESPONSE_HEADER_TYPES::Age},
        {"ETag", RESPONSE_HEADER_TYPES::ETag},
        {"Location", RESPONSE_HEADER_TYPES::Location},
        {"Proxy-Authenticate", RESPONSE_HEADER_TYPES::Proxy_Authenticate},
        {"Retry-After", RESPONSE_HEADER_TYPES::Retry_After},
        {"Server", RESPONSE_HEADER_TYPES::Server},
        {"Vary", RESPONSE_HEADER_TYPES::Vary},
        {"WWW-Authenticate", RESPONSE_HEADER_TYPES::WWW_Authenticate},
        {"Allow", RESPONSE_HEADER_TYPES::Allow},
        {"Content-Encoding", RESPONSE_HEADER_TYPES::Content_Encoding},
        {"Content-Language", RESPONSE_HEADER_TYPES::Content_Language},
        {"Content-Length", RESPONSE_HEADER_TYPES::Content_Length},
        {"Content-Location", RESPONSE_HEADER_TYPES::Content_Location},
        {"Content-MD5", RESPONSE_HEADER_TYPES::Content_MD5},
        {"Content-Range", RESPONSE_HEADER_TYPES::Content_Range},
        {"Content-type", RESPONSE_HEADER_TYPES::Content_Type},
        {"Expires", RESPONSE_HEADER_TYPES::Expires},
        {"Last-Modified", RESPONSE_HEADER_TYPES::Last_Modified},
        {"extension-header", RESPONSE_HEADER_TYPES::extension_header},
        {"Date", RESPONSE_HEADER_TYPES::Date}};

    struct IResponseHeader
    {
    public:
        string name;
        string pureValue;
        RESPONSE_HEADER_TYPES cachedType = Not_Initialized;

        IResponseHeader(const string &name, const string &pureValue)
            : name(name), pureValue(pureValue)
        {
        }
        virtual ~IResponseHeader() = default;

        RESPONSE_HEADER_TYPES getType()
        {
            if (cachedType == Not_Initialized)
            {
                const auto it = RESPONSE_HEADER_NAME_TO_TYPE.find(name);
                if (it == RESPONSE_HEADER_NAME_TO_TYPE.end())
                {
                    cachedType = RESPONSE_HEADER_TYPES::unknown;
                }
                else
                {
                    cachedType = it->second;
                }
            }
            return cachedType;
        }

        friend ostream &operator<<(ostream &out, const IResponseHeader &instance)
        {
            return out << instance.name << COLON << instance.pureValue;
        }
    };

    struct ResponseAcceptRange : IResponseHeader
    {
    };

    struct ResponseAge : IResponseHeader
    {
    };

    struct ResponseETag : IResponseHeader
    {
    };

    struct ResponseLocation : IResponseHeader
    {
    };

    struct ResponseProxyAuthenticate : IResponseHeader
    {
    };

    struct ResponseRetryAfter : IResponseHeader
    {
    };

    struct ResponseServer : IResponseHeader
    {
    };

    struct ResponseVary : IResponseHeader
    {
    };

    struct ResponseWWWAuthenticate : IResponseHeader
    {
    };

    struct ResponseAllow : IResponseHeader
    {
    };

    struct ResponseContentEncoding : IResponseHeader
    {
    };

    struct ResponseContentLanguage : IResponseHeader
    {
    };

    struct ResponseContentLength : IResponseHeader
    {

        ulong getValue()
        {
            return stoul(pureValue);
        }
    };

    struct ResponseContentLocation : IResponseHeader
    {
    };

    struct ResponseContentMD5 : IResponseHeader
    {
    };

    struct ResponseContentRange : IResponseHeader
    {
    };

    struct ResponseContentType : IResponseHeader
    {
    };

    struct ResponseExpires : IResponseHeader
    {
    };

    struct ResponseLastModified : IResponseHeader
    {
    };

    struct ResponseExtension_Header : IResponseHeader
    {
    };

    struct ServerResponseCode
    {
    private:
    public:
        uint code;
        string definition;

        friend ostream &operator<<(ostream &out, const ServerResponseCode &instance)
        {
            return out << instance.code << SPACE << instance.definition << END_LINE;
        }

        friend istream &operator>>(istream &in, ServerResponseCode &instance)
        {
            string stringCode;
            IStreamSplitter::readUntil(in, stringCode, SPACE);
            instance.code = stoul(stringCode);
            IStreamSplitter::readUntil(in, instance.definition, END_LINE);
            return in;
        }
    };

    struct ResponseStatus
    {
        HttpVersion httpVersion;
        ServerResponseCode responseCode;

        friend ostream &operator<<(ostream &out, const ResponseStatus &instance)
        {
            return out << instance.httpVersion << SPACE << instance.responseCode;
        }

        friend istream &operator>>(istream &in, ResponseStatus &instance)
        {
            in >> instance.httpVersion;
            in >> instance.responseCode;
            return in;
        }
    };

    struct HttpResponse
    {
    public:
        ResponseStatus status;
        map<RESPONSE_HEADER_TYPES, IResponseHeader> headers;
        shared_ptr<istream> contentStream;

        HttpResponse(HttpResponse &&) = default;
        HttpResponse() = default;
        ~HttpResponse() = default;

        string getStringContent() const
        {
            if (contentStream.get() != nullptr)
            {
                string result;
                contentStream->seekg(0);
                IStreamSplitter::readToEnd(contentStream, result);
                return result;
            }
            return "";
        }

        friend ostream &operator<<(ostream &out, const HttpResponse &instance)
        {
            out << instance.status;
            for (auto item : instance.headers)
            {
                out << item.second << END_LINE;
            }
            out << END_LINE << instance.getStringContent();
            return out;
        }

        friend istream &operator>>(istream &in, HttpResponse &instance)
        {
            in >> instance.status;
            string headers;
            ulong headersLen = IStreamSplitter::readUntil(in, headers, END_HEADER);
            stringstream headerStream(headers);

            string headerName;
            string headerValue;
            ulong readCharCount = 0;
            while ((readCharCount = IStreamSplitter::readUntil(headerStream, headerName, COLON_SPACE)) > 0)
            {
                IStreamSplitter::readUntil(headerStream, headerValue, END_LINE);
                IResponseHeader header{headerName, headerValue};
                instance.headers.insert({header.getType(), header});
            }

            auto contentLengthItr = instance.headers.find(RESPONSE_HEADER_TYPES::Content_Length);
            if (contentLengthItr != instance.headers.end())
            {
                ResponseContentLength *contentLength = reinterpret_cast<ResponseContentLength *>(&contentLengthItr->second);
                instance.contentStream = IStreamSplitter::subStream(in, in.tellg(), contentLength->getValue() + 1);
            }

            return in;
        }
    };
} // namespace KCore
