#pragma once
#include <iostream>
#include <iterator>

namespace KCore
{
    template <class _Tp, class _CharT = char, class _Traits = char_traits<_CharT>, class _Distance = ptrdiff_t>
    class istream_iterator : public std::istream_iterator<_Tp, _CharT, _Traits, _Distance>
    {
    private:
        using BASE = std::istream_iterator<_Tp, _CharT, _Traits, _Distance>;
        typedef basic_istream<_CharT, _Traits> istream_type;

        istream_type* __in_stream_;

    public:
        istream_iterator() : BASE()
        {
        }
        istream_iterator(istream_type &__s) : BASE(__s),
        __in_stream_(&__s)
        {
            
        }

        istream_iterator &operator--()
        {
            // if (!(*__in_stream_ >> __value_))
            //     __in_stream_ = nullptr;
            return *this;
        }
        istream_iterator operator--(int)
        {
            istream_iterator __t(*this);
            --(*this);
            return __t;
        }
    };
}