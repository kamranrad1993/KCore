#include <iostream>
#include <memory.h>

namespace KCore
{
    class memory_streambuf : public std::streambuf
    {
    public:

        explicit memory_streambuf(std::size_t maximum_size = (std::numeric_limits<std::size_t>::max)())
            : max_size_(maximum_size),
              buffer_()
        {
            std::size_t pend = (std::min<std::size_t>)(max_size_, buffer_delta);
            buffer_.resize((std::max<std::size_t>)(pend, 1));
            setg(&buffer_[0], &buffer_[0], &buffer_[0]);
            setp(&buffer_[0], &buffer_[0] + pend);
        }

        std::size_t size() const
        {
            return pptr() - gptr();
        }

        std::size_t max_size() const
        {
            return max_size_;
        }

        std::size_t capacity() const
        {
            return buffer_.capacity();
        }

        char_type* get_gptr()
        {
            return gptr();
        }

        char_type* get_pptr()
        {
            return pptr();
        }

        void commit(std::size_t n)
        {
            n = std::min<std::size_t>(n, epptr() - pptr());
            pbump(static_cast<int>(n));
            setg(eback(), gptr(), pptr());
        }

        void consume(std::size_t n)
        {
            if (egptr() < pptr())
                setg(&buffer_[0], gptr(), pptr());
            if (gptr() + n > pptr())
                n = pptr() - gptr();
            gbump(static_cast<int>(n));
        }

        void clear()
        {
            setg(&buffer_[0], &buffer_[0], &buffer_[0]);
            setp(&buffer_[0], &buffer_[0] + (std::min<std::size_t>)(max_size_, buffer_delta));
        }

    protected:
        enum
        {
            buffer_delta = 128
        };

        int_type underflow()
        {
            if (gptr() < pptr())
            {
                setg(&buffer_[0], gptr(), pptr());
                return traits_type::to_int_type(*gptr());
            }
            else
            {
                return traits_type::eof();
            }
        }

        int_type overflow(int_type c)
        {
            if (!traits_type::eq_int_type(c, traits_type::eof()))
            {
                if (pptr() == epptr())
                {
                    std::size_t buffer_size = pptr() - gptr();
                    if (buffer_size < max_size_ && max_size_ - buffer_size < buffer_delta)
                    {
                        reserve(max_size_ - buffer_size);
                    }
                    else
                    {
                        reserve(buffer_delta);
                    }
                }

                *pptr() = traits_type::to_char_type(c);
                pbump(1);
                return c;
            }

            return traits_type::not_eof(c);
        }

        void reserve(std::size_t n)
        {
            // Get current stream positions as offsets.
            std::size_t gnext = gptr() - &buffer_[0];
            std::size_t pnext = pptr() - &buffer_[0];
            std::size_t pend = epptr() - &buffer_[0];

            // Check if there is already enough space in the put area.
            if (n <= pend - pnext)
            {
                return;
            }

            // Shift existing contents of get area to start of buffer.
            if (gnext > 0)
            {
                pnext -= gnext;
                std::memmove(&buffer_[0], &buffer_[0] + gnext, pnext);
            }

            // Ensure buffer is large enough to hold at least the specified size.
            if (n > pend - pnext)
            {
                if (n <= max_size_ && pnext <= max_size_ - n)
                {
                    pend = pnext + n;
                    buffer_.resize((std::max<std::size_t>)(pend, 1));
                }
                else
                {
                    std::length_error ex("asio::streambuf too long");
                    asio::detail::throw_exception(ex);
                }
            }

            // Update stream positions.
            setg(&buffer_[0], &buffer_[0], &buffer_[0] + pnext);
            setp(&buffer_[0] + pnext, &buffer_[0] + pend);
        }

    private:
        std::size_t max_size_;
        std::vector<char_type> buffer_;

        // Helper function to get the preferred size for reading data.
        friend std::size_t read_size_helper(
            memory_streambuf &sb, std::size_t max_size)
        {
            return std::min<std::size_t>(
                std::max<std::size_t>(512, sb.buffer_.capacity() - sb.size()),
                std::min<std::size_t>(max_size, sb.max_size() - sb.size()));
        }
    };

    // class streambuf2 : public std::streambuf
    // {
    // private:
    //     char_type *buf = nullptr;

    // public:
    //     streambuf2(size_t length) // TODO may be wrong . must keep length
    //     {
    //         buf = (char_type *)malloc(length + 2);
    //         setp(buf, buf + length);
    //         setg(buf, buf, buf + length);
    //     }

    //     streambuf2(void *ptr, size_t length)
    //     {
    //         buf = (char_type *)malloc(length + 2);
    //         memcpy(buf, ptr, length);
    //         setp(buf, buf + length);
    //         setg(buf, buf, buf + length);
    //     }
    // };
}