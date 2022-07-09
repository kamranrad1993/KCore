#pragma once

#include <cstring>
#include <debuging/logging.h>
#include <iosfwd>
#include <iostream>
#include <vector>

namespace KCore
{
    using namespace std;
    class streambuf : public std::streambuf
    {
    private:
        char_type *buf = nullptr;
        size_t current_pptr = 0;
        size_t current_gptr = 0;

    protected:
        int_type underflow() override
        {
            if (gptr() < pptr())
            {
                setg(buf, gptr(), pptr());
                return traits_type::to_int_type(*gptr());
            }
            else
            {
                return traits_type::eof();
            }
        }

        int_type overflow(int_type c) override
        {
            if (!traits_type::eq_int_type(c, traits_type::eof()))
            {
                if (pptr() == epptr())
                {
                    return traits_type::eof();
                }

                *pptr() = traits_type::to_char_type(c);
                // pbump(1);
                return c;
            }

            return traits_type::not_eof(c);
        }

        streamsize xsputn(const char_type *s, streamsize n) override
        {
            if (buf == nullptr)
            {
                buf = (char_type *)malloc(n + 2);
            }
            else
            {
                void *new_buf = malloc(current_pptr + n + 1);
                memcpy(new_buf, buf, current_pptr);
                delete buf;
                buf = (char_type *)new_buf;
            }

            memcpy((void *)(buf + current_pptr), s, n);
            current_pptr += n;
            setp(buf, buf + current_pptr);
            setg(buf, buf, buf + current_pptr);

            return n;
        }

    public:
        streambuf()
        {
        }

        streambuf(size_t length) //TODO may be wrong . must keep length
        {
            buf = (char_type *)malloc(length + 2);
            current_pptr = 0;
            setp(buf, buf + current_pptr);
            setg(buf, buf, buf + current_pptr);
        }

        streambuf(void *ptr, size_t length)
        {
            buf = (char_type *)malloc(length + 2);
            memcpy(buf, ptr, length);
            current_pptr = length + 1;
            setp(buf, buf + current_pptr);
            setg(buf, buf, buf + current_pptr);
        }

        char_type *get_pptr()
        {
            return (char_type *)(buf + current_pptr);
        }

        char_type *get_gptr()
        {
            return (char_type *)gptr();
        }

        size_t get_length()
        {
            return current_pptr;
        }

        void clear()
        {
            current_pptr = 0;
        }
    };

} // namespace KCore
