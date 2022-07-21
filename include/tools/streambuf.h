#pragma once

#include <algorithm>
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
        size_t current_size = 0;

    protected:
        streambuf *setbuf(char *s, streamsize n) override
        {
            buf = (char_type *)malloc(n + 2);
            memcpy(buf, s, n);
            current_pptr = n + 1;
            setp(buf, buf + current_pptr);
            setg(buf, buf, buf + current_pptr);
            current_gptr = 0;
            current_size = n + 2;
            return this;
        }

        streampos seekoff(streamoff off, ios_base::seekdir way,
                          ios_base::openmode which = ios_base::in | ios_base::out) override
        {

            string str_way = "cur";
            string str_which = "cur";
            switch (way)
            {
            case ios_base::cur:
                str_way = "cur";
                break;
            case ios_base::beg:
                str_way = "beg";
                break;
            case ios_base::end:
                str_way = "end";
                break;
            }
            if (which & ios_base::in)
            {
                str_which = "in";
            }
            else if (which & ios_base::out)
            {
                str_which = "out";
            }
            LOG("seekoff", off, str_way, str_which);

            if (which & ios_base::in && which & ios_base::out)
            {
                throw std::runtime_error("streambuf::seekoff: invalid openmode");
            }

            switch (way)
            {
            case ios_base::cur:
                if (which & ios_base::in)
                {
                    current_gptr += off;
                    current_gptr = clamp<size_t>(current_gptr, 0, current_size - 1);
                    setg(buf, buf + current_gptr, buf + current_pptr);
                    return streampos(current_gptr);
                }
                else if (which & ios_base::out)
                {
                    current_pptr += off;
                    current_pptr = clamp<size_t>(current_pptr, 0, current_size - 1);
                    setp(buf + current_pptr, buf + current_size - 1);
                    return streampos(current_pptr);
                }
                else
                {
                    throw std::runtime_error("streambuf::seekoff: invalid openmode");
                }
                break;
            case ios_base::beg:
                if (which & ios_base::in)
                {
                    current_gptr = off;
                    current_gptr = clamp<size_t>(current_gptr, 0, current_size);
                    setg(buf, buf + current_gptr, buf + current_pptr);
                    return streampos(current_gptr);
                }
                if (which & ios_base::out)
                {
                    current_pptr = off;
                    current_pptr = clamp<size_t>(current_pptr, 0, current_size - 1);
                    setp(buf + current_pptr, buf + current_size - 1);
                    return streampos(current_pptr);
                }
                else
                {
                    throw std::runtime_error("streambuf::seekoff: invalid openmode");
                }
                break;
            case ios_base::end:
                if (which & ios_base::in)
                {
                    current_gptr = current_size - off - 1;
                    current_gptr = clamp<size_t>(current_gptr, 0, current_size - 1);
                    setg(buf, buf + current_gptr, buf + current_pptr);
                    return streampos(current_gptr);
                }
                if (which & ios_base::out)
                {
                    current_pptr = current_size - off - 1;
                    current_pptr = clamp<size_t>(current_pptr, 0, current_size - 1);
                    setp(buf + current_pptr, buf + current_size - 1);
                    return streampos(current_pptr);
                }
                else
                {
                    throw std::runtime_error("streambuf::seekoff: invalid openmode");
                }
                break;
            default:
                // return pos_type(off_type(-1));
                throw std::runtime_error("streambuf::seekoff: invalid seekdir");
            }
            return pos_type(off_type(-1));
        }

        streampos seekpos(streampos sp, ios_base::openmode which = ios_base::in | ios_base::out) override
        {
            return seekoff(off_type(sp), ios_base::beg, which);
            // // LOG("seekpos ", sp, which);
            // if (which & ios_base::in && which & ios_base::out == ios_base::out)
            // {
            //     throw std::runtime_error("streambuf::seekpos: invalid openmode");
            // }
            // if (which & ios_base::in)
            // {
            //     current_pptr = sp;
            //     current_pptr = clamp<size_t>(current_pptr, 0, current_size);
            //     setp(buf + current_pptr, buf + current_size - 1);
            //     return streampos(current_pptr);
            // }
            // if (which & ios_base::out == ios_base::out)
            // {
            //     current_gptr = sp;
            //     current_gptr = clamp<size_t>(current_gptr, 0, current_size);
            //     setg(buf, buf + current_gptr, buf + current_size - 1);
            //     return streampos(current_gptr);
            // }
            // return streampos(-1);
        }

        // int sync() override
        // {
        //     LOG("sync");
        //     return 0;
        // }

        // streamsize showmanyc() override
        // {
        //     LOG("showmanyc");
        //     return 0;
        // }

        // int uflow() override
        // {
        //     LOG("uflow");
        //     return 0;
        // }

        // int pbackfail(int c = EOF) override
        // {
        //     LOG("pbackfail");
        //     return 0;
        // }

        int_type underflow() override
        {
            LOG_TRACE("underflow");
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
            LOG_TRACE("overflow");
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
                current_size = n + 2;
            }
            else
            {
                void *new_buf = malloc(current_pptr + n + 1);
                current_size = current_pptr + n + 1;
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

        // streamsize xsgetn(char *s, streamsize n) override
        // {
        //     LOG("xsgetn");
        //     return 0;
        // }

    public:
        streambuf()
        {
        }

        streambuf(size_t length) // TODO may be wrong . must keep length
        {
            buf = (char_type *)malloc(length + 2);
            current_pptr = 0;
            setp(buf, buf + current_pptr);
            setg(buf, buf, buf + current_pptr);
            current_size = length + 2;
        }

        streambuf(void *ptr, size_t length)
        {
            buf = (char_type *)malloc(length + 2);
            memcpy(buf, ptr, length);
            current_pptr = length + 1;
            setp(buf, buf + current_pptr);
            setg(buf, buf, buf + current_pptr);
            current_size = length + 2;
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

        ~streambuf()
        {
            // LOG("~streambuf");
            if (buf != nullptr)
            {
                free(buf);
            }
        }
    };

} // namespace KCore
