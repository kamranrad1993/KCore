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
    class streambuf : public std::basic_streambuf<char>
    {
    private:
        typedef std::basic_streambuf<char> BASE;

        char_type *buf = nullptr;
        streamoff current_size = 0;

    protected:
        streambuf *setbuf(char *s, streamsize n) override
        {
            buf = (char_type *)malloc(n);
            memcpy(buf, s, n);
            setp(buf, buf + n);
            setg(buf, buf, buf + n);
            current_size = n;
            return this;
        }

        streampos seekoff(streamoff off, ios_base::seekdir way,
                          ios_base::openmode which = ios_base::in | ios_base::out) override
        {
            if (which & ios_base::in && which & ios_base::out)
            {
                return pos_type(-1);
                // throw std::runtime_error("streambuf::seekoff: invalid openmode");
            }

            streamoff noff;
            switch (way)
            {
            case ios_base::beg:
                noff = 0;
                break;
            case ios_base::cur:
                if (which & ios_base::in)
                    noff = this->gptr() - this->eback();
                else
                    noff = this->pptr() - this->pbase();
                break;
            case ios_base::end:
                noff = (streamoff)this->buf + this->current_size;
                break;
            default:
                return pos_type(-1);
            }

            noff += off;
            if (noff < 0 || this->current_size < noff)
                return pos_type(-1);
            if (noff != 0)
            {
                if ((which & ios_base::in) && this->gptr() == nullptr)
                    return pos_type(-1);
                if ((which & ios_base::out) && this->pptr() == nullptr)
                    return pos_type(-1);
            }
            if (which & ios_base::in)
                this->setg(this->eback(), this->eback() + noff, this->eback() + this->current_size);
            if (which & ios_base::out)
            {
                this->setp(this->pbase(), this->epptr());
                this->pbump(noff);
            }
            return pos_type(noff);
        }

        streampos seekpos(streampos sp, ios_base::openmode which = ios_base::in | ios_base::out) override
        {
            return seekoff(off_type(sp), ios_base::beg, which);
        }

        int sync() override
        {
            return BASE::sync();
        }

        streamsize showmanyc() override
        {
            return BASE::showmanyc();
        }

        int uflow() override
        {
            return BASE::uflow();
        }

        int pbackfail(int c = EOF) override
        {
            return BASE::pbackfail();
        }

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
                pbump(1);
                return c;
            }

            return traits_type::not_eof(c);
        }

        streamsize xsputn(const char_type *s, streamsize n) override
        {
            streamoff size = epptr() - pbase();
            if (buf == nullptr)
            {
                buf = (char_type *)malloc(n);
                current_size = n ;
                setp(buf, buf + size);
                setg(buf, buf, buf + size);
            }
            else
            {
                void *new_buf = malloc(current_size + n);
                current_size = size + n;
                memcpy(new_buf, buf, size);
                delete buf;
                buf = (char_type *)new_buf;
            }

            memcpy((void *)(buf + size), s, n);
            size += n;
            setp(buf, buf + size);
            setg(buf, buf, buf + size);
            this->pbump(size);

            return n;
        }

        streamsize xsgetn(char *s, streamsize n) override
        {
            return BASE::xsgetn(s, n);
        }

    public:
        streambuf()
        {
        }

        streambuf(size_t length) // TODO may be wrong . must keep length
        {
            buf = (char_type *)malloc(length);
            setp(buf, buf + length);
            setg(buf, buf, buf + length);
            current_size = length ;
        }

        streambuf(void *ptr, size_t length)
        {
            buf = (char_type *)malloc(length);
            memcpy(buf, ptr, length);
            setp(buf, buf + length);
            setg(buf, buf, buf + length);
            current_size = length;
        }

        char_type *get_pptr()
        {
            return (char_type *)pptr();
        }

        char_type *get_gptr()
        {
            return (char_type *)gptr();
        }

        char_type * get_ptr()
        {
            return buf;
        }

        streamoff get_length()
        {
            streamoff size = pptr() - pbase();
            return size;
        }

        void clear()
        {
            streamoff size = epptr() - pbase();
            setp(buf, buf + size);
        }

        ~streambuf()
        {
            if (buf != nullptr)
            {
                free(buf);
            }
        }
    };

} // namespace KCore
