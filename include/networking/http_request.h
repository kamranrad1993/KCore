#pragma once

#include <asio.hpp>
#include <climits>
#include <hash_map>
#include <iosfwd>
#include <memory>
#include <networking/http_definitions.h>
#include <random>
#include <tools/format_string.h>
#include <tools/istream_tools.h>
#include <vector>
#include <strstream>

namespace KCore
{
    using namespace std;
    using asio::ip::tcp;

    struct RequestHeader
    {
    public:
        string key;
        string value;

        RequestHeader(const string &headerKey, const string &headerValue)
            : value(headerValue), key(headerKey) {}
        virtual ~RequestHeader() = default;
        friend ostream &operator<<(ostream &out, const RequestHeader &instance)
        {
            return out << instance.key << COLON << instance.value << END_LINE;
        }
    };

    struct RequestMethod
    {
        string value;

        friend ostream &operator<<(ostream &out, const RequestMethod &instance)
        {
            return out << instance.value;
        }

        friend istream &operator>>(std::istream &in, RequestMethod &instance)
        {
            IStreamSplitter::readUntil(in, instance.value, SPACE);
            return in;
        }
    };
#define REQUEST_METHOD_GET \
    (RequestMethod) { "GET" }
#define REQUEST_METHOD_POST \
    (RequestMethod) { "POST" }
#define REQUEST_METHOD_PUT \
    (RequestMethod) { "PUT" }
#define REQUEST_METHOD_HEAD \
    (RequestMethod) { "HEAD" }
#define REQUEST_METHOD_DELETE \
    (RequestMethod) { "DELETE" }
#define REQUEST_METHOD_PATCH \
    (RequestMethod) { "PATCH" }
#define REQUEST_METHOD_OPTIONS \
    (RequestMethod) { "OPTIONS" }

    struct RequestAccept : public RequestHeader
    {
        RequestAccept(string value) : RequestHeader("Accept", value) {}
    };
#define REQUEST_ACCEPT_PLAIN_TEXT \
    (RequestAccept) { "text/plain" }
#define REQUEST_ACCEPT_HTML \
    (RequestAccept) { "text/html" }
#define REQUEST_ACCEPT_JSON \
    (RequestAccept) { "application/json" }

    struct RequestAcceptCharset : public RequestHeader
    {
        RequestAcceptCharset(string value) : RequestHeader("Accept-Charset", value) {}
    };
#define REQUEST_ACCEPT_CHARSET_ASCII \
    (RequestAcceptCharset) { "acsii" }
#define REQUEST_ACCEPT_CHARSET_WINdows_1252 \
    (RequestAcceptCharset) { "windows-1252" }
#define REQUEST_ACCEPT_CHARSET_ISO_8859_1 \
    (RequestAcceptCharset) { "iso-8859-1" }
#define REQUEST_ACCEPT_CHARSET_utf_8 \
    (RequestAcceptCharset) { "utf-8" }

    struct RequestAcceptEncoding : public RequestHeader
    {
        RequestAcceptEncoding(string value) : RequestHeader("Accept-Encoding", value) {}
    };
#define REQUEST_ACCEPT_ENCODING_GZIP \
    (RequestAcceptEncoding) { "gzip" }
#define REQUEST_ACCEPT_ENCODING_COMPRESS \
    (RequestAcceptEncoding) { "compress" }
#define REQUEST_ACCEPT_ENCODING_DEFLATGE \
    (RequestAcceptEncoding) { "deflate" }
#define REQUEST_ACCEPT_ENCODING_BR \
    (RequestAcceptEncoding) { "br" }
#define REQUEST_ACCEPT_ENCODING_IDENTITY \
    (RequestAcceptEncoding) { "identity" }
#define REQUEST_ACCEPT_ENCODING_ALL \
    (RequestAcceptEncoding) { "*" }

    struct RequestAcceptLanguage : public RequestHeader
    {
        RequestAcceptLanguage(string value) : RequestHeader("Accept-Language", value) {}
    };
#define REQUEST_ACCEPT_LANGAGE_EN \
    (RequestAcceptLanguage) { "en" }
#define REQUEST_ACCEPT_LANGAGE_FA \
    (RequestAcceptLanguage) { "fa" }

    struct RequestAcceptRanges : public RequestHeader
    {
        RequestAcceptRanges(string value) : RequestHeader("Accept-Ranges", value) {}
    };
#define REQUEST_ACCEPT_RANGES_BYTES \
    (AcceptRanges) { "bytes" }
#define REQUEST_ACCEPT_RANGES_NONE \
    (AcceptRanges) { "none" }

    struct RequestAuthorization : public RequestHeader
    {
        RequestAuthorization(string value) : RequestHeader("Authorization", value) {}
    };

    struct RequestConnection : public RequestHeader
    {
        RequestConnection(string value) : RequestHeader("Connection", value) {}
    };
#define REQUEST_CONNECTION_CLOSE \
    (RequestConnection) { "close" }
#define REQUEST_CONNECTION_KEEP_ALIVE \
    (RequestConnection) { "keep-alive" }

    struct RequestContentEncoding : public RequestHeader
    {
        RequestContentEncoding(string value) : RequestHeader("Content-Encoding", value) {}
    };
#define REQUEST_CONTENT_ENCODING_GZIP \
    (RequestContentEncoding) { "gzip" }
#define REQUEST_CONTENT_ENCODING_COMPRESS \
    (RequestContentEncoding) { "compress" }
#define REQUEST_CONTENT_ENCODING_DEFLATGE \
    (RequestContentEncoding) { "deflate" }
#define REQUEST_CONTENT_ENCODING_BR \
    (RequestContentEncoding) { "br" }
#define REQUEST_CONTENT_ENCODING_IDENTITY \
    (RequestContentEncoding) { "identity" }

    struct RequestContentLanguage : public RequestHeader
    {
        RequestContentLanguage(string value) : RequestHeader("Content-Language", value) {}
    };
#define REQUEST_CONTENT_LANGAGE_EN \
    (RequestContentLanguage) { "en" }
#define REQUEST_CONTENT_LANGAGE_FA \
    (RequestContentLanguage) { "en" }

    struct RequestContentLength : public RequestHeader
    {
        RequestContentLength(string value) : RequestHeader("Content-Length", value) {}
    };

    struct RequestContentLocation : public RequestHeader
    {
        RequestContentLocation(string value) : RequestHeader("Content-Location", value) {}
    };

    struct RequestContentMD5 : public RequestHeader
    {
        RequestContentMD5(string value) : RequestHeader("Content-MD5", value) {}
    };

    struct RequestContentLenght : public RequestHeader
    {
        RequestContentLenght(string value) : RequestHeader("Content-Length", value) {}
    };

    struct RequestContentRange : public RequestHeader
    {
        RequestContentRange(ulong start, ulong end, ulong total)
            : RequestHeader("Content-Range", to_string(start) + "-" + to_string(end) + "/" + to_string(total))
        {
        }
    };

    struct RequestContentType : public RequestHeader
    {
        RequestContentType(string value) : RequestHeader("Content-type", value) {}
    };
#define REQUEST_CONTENT_TYPE_PLAIN_TEXT \
    (RequestContentType) { "text/plain" }
#define REQUEST_CONTENT_TYPE_HTML \
    (RequestContentType) { "text/html" }
#define REQUEST_CONTENT_TYPE_JSON \
    (RequestContentType) { "application/json" }

    struct RequestUserAgent : public RequestHeader
    {
        RequestUserAgent(string value) : RequestHeader("User-Agent", value) {}
    };
#define REQUEST_USER_AGENT_CHROME_WINDOWS \
    (RequestUserAgent) { "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/92.0.4515.131 Safari/537.36" }
#define REQUEST_USER_AGENT_CHROME_MACOS \
    (RequestUserAgent) { "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/92.0.4515.107 Safari/537.36" }
#define REQUEST_USER_AGENT_CHROME_LINUX \
    (RequestUserAgent) { "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/92.0.4515.107 Safari/537.36" }
#define REQUEST_USER_AGENT_FIREFOX_WINDOWS \
    (RequestUserAgent) { "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:90.0) Gecko/20100101 Firefox/90.0" }
#define REQUEST_USER_AGENT_FIREFOX_MACOS \
    (RequestUserAgent) { "Mozilla/5.0 (Macintosh; Intel Mac OS X 10.15; rv:90.0) Gecko/20100101 Firefox/90.0" }
#define REQUEST_USER_AGENT_FIREFOX_LINUX \
    (RequestUserAgent) { "Mozilla/5.0 (X11; Linux x86_64; rv:90.0) Gecko/20100101 Firefox/90.0" }

    struct RequestAddress
    {
        string host;
        uint16_t port;
        string path;

        friend ostream &operator<<(ostream &out, const RequestAddress &instance)
        {
            return out << instance.host << "/" << instance.path;
        }

        string str()
        {
            return host + "/" + path;
        }
    };

    struct RequestDefinition
    {
        RequestMethod method;
        string requestPath;
        HttpVersion httpVersion;

        RequestDefinition(RequestMethod method, string requestPath, HttpVersion httpVersion)
            : method(method), requestPath(requestPath), httpVersion(httpVersion)
        {
        }

        friend ostream &operator<<(ostream &out, const RequestDefinition &instance)
        {
            return out << instance.method << SPACE
                       << instance.requestPath << SPACE
                       << instance.httpVersion << END_LINE;
        }

        friend istream &operator>>(istream &in, RequestDefinition &instance)
        {
            // instance = RequestDefinition();
            in >> instance.method >> instance.requestPath >> instance.httpVersion;
            return in;
        }
    };

    struct RequestBoundary
    {
        string value;
        friend ostream &operator<<(ostream &out, const RequestBoundary &instance)
        {
            return out << instance.value;
        }
    };
    static default_random_engine RANDOM_GENERATOR;
    inline RequestBoundary makeBoundary()
    {
        uniform_int_distribution<ulong> distribution(0, ULONG_MAX);
        ulong result = distribution(RANDOM_GENERATOR);
        return (RequestBoundary){"--boundary--" + to_string(result)};
    }

    struct HttpRequest
    {
        RequestDefinition requestDefinition;
        RequestAddress address;
        map<string, string> formData;
        vector<RequestHeader> headers;

        HttpRequest(RequestDefinition &definition, RequestAddress &address)
            : requestDefinition(definition), address(address)
        {
        }

        friend ostream &operator<<(ostream &out, const HttpRequest &instance)
        {
            out << instance.requestDefinition
                << "Host: " << instance.address.host << END_LINE;
            for (RequestHeader header : instance.headers)
            {
                out << header << END_LINE;
            }
            out << END_LINE;
            return out;
        }

        // shared_ptr<void*>getDataPtr(size_t& length)
        // {
        //     strstreambuf buf();
        //     for(RequestHeader header: this->headers)
        //     {
        //         buf << header << END_LINE;
        //     }
        //     buf << END_LINE;
        //     // return make_shared(str.rdbuf());
        // }
    };

}