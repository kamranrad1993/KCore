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
        vector<char_type> buf;
        size_t increase_length_step;
        size_t initial_size;

    protected:
        int_type underflow() override
        {
            LOG("underflow");
            if (gptr() < pptr())
            {
                setg((char_type *)buf.data(), gptr(), pptr());
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
                    size_t old_size = buf.size();
                    size_t new_size = buf.size() + increase_length_step;
                    buf.resize(new_size);
                    size_t from = (size_t)(&buf[0] + old_size + 1);
                    size_t to = (size_t)(&buf[0] + new_size);
                    LOG("from ", from, " to ", to, " diff ", to - from);
                    from = (size_t)pptr() + 1;
                    to = (size_t)pptr() + increase_length_step;
                    LOG("#from ", from, " to ", to, " diff ", to - from);
                    setp((char_type *)(&buf[0] + old_size + 1), (char_type *)(&buf[0] + new_size));
                    // setp(get_pptr() + 1, get_pptr() + increase_length_step);
                    // setg(get_gptr(), get_gptr(), (char_type *)(buf.data() + buf.size()));
                    // return traits_type::eof();
                }

                *pptr() = traits_type::to_char_type(c);
                // pbump(1);
                return c;
            }

            return traits_type::not_eof(c);
        }

    public:
        streambuf(size_t length, size_t increase_length_step = 128)
        {
            initial_size = length;
            this->increase_length_step = increase_length_step;
            buf = vector<char_type>(length);
            setg((char_type *)buf.data(), (char_type *)buf.data(), (char_type *)buf.data() + length);
            setp((char_type *)buf.data(), (char_type *)buf.data() + length);
        }

        streambuf(void *ptr, size_t length, size_t increase_length_step = 128)
            : streambuf(length, increase_length_step)
        {
            // buf.resize(length);
            memcpy(buf.data(), ptr, length);
        }

        char_type *get_pptr()
        {
            return (char_type *)pptr();
        }

        char_type *get_gptr()
        {
            return (char_type *)gptr();
        }

        size_t get_length()
        {
            return buf.size();
        }

        void clear()
        {
            buf.clear();
            buf.resize(initial_size);
            setg((char_type *)buf.data(), (char_type *)buf.data(), (char_type *)buf.data() + initial_size);
            setp((char_type *)buf.data(), (char_type *)buf.data() + initial_size);
        }
    };

} // namespace KCore
