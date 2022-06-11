#pragma once
#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>

namespace KCore
{
    using namespace std;

    template <typename CharT, typename TraitsT = std::char_traits<CharT>>
    class VectorWrapBuf : public std::basic_streambuf<CharT, TraitsT>
    {
    private:
        const char *const begin_;
        const char *const end_;
        const char *current_;
        const ulong len_;

    public:
        VectorWrapBuf(std::vector<CharT> &vec)
            : begin_(vec.data()),
              end_(vec.data() + vec.size() * sizeof(char)),
              current_(vec.data()),
              len_(vec.size())
        {
            // this->setg(vec.data(), vec.data(), vec.data() + vec.size());
        }

        typename TraitsT::int_type underflow()
        {
            if (current_ == end_)
            {
                return TraitsT::eof();
            }
            return TraitsT::to_int_type(*current_); // HERE!
        }

        typename TraitsT::int_type uflow()
        {
            if (current_ == end_)
            {
                return TraitsT::eof();
            }
            return TraitsT::to_int_type(*current_++); // HERE!
        }

        typename TraitsT::int_type pbackfail(typename TraitsT::int_type ch)
        {
            if (current_ == begin_ || (ch != TraitsT::eof() && ch != current_[-1]))
            {
                return TraitsT::eof();
            }
            return TraitsT::to_int_type(*--current_); // HERE!
        }

        streampos seekoff(streamoff off, ios_base::seekdir seekdir, ios_base::openmode openmode = ios_base::in | ios_base::out)
        {
            if (seekdir == ios_base::beg)
            {
                return  reinterpret_cast<uintptr_t>(begin_) + off;
            }
            else if (seekdir == ios_base::cur)
            {
                return reinterpret_cast<uintptr_t>(current_) + off;
            }
            else if (seekdir == ios_base::end)
            {
                return reinterpret_cast<uintptr_t>(end_) - off;
            }

            return -1;
        }

        streampos seekpos(streampos streampos, ios_base::openmode openmode = ios_base::in | ios_base::out)
        {
            if (streampos > len_ || streampos < 0)
            {
                return -1;
            }

            return reinterpret_cast<uintptr_t>(begin_) + len_;
        }

        std::streamsize showmanyc()
        {
            return end_ - current_;
        }
    };

    class IStreamSplitter
    {
    private:
        static bool sequenceCheckKeepPos(istream &is, string &sequence)
        {
            uint pos = is.tellg();
            uint sequencePos = 0;

            istream_iterator<char> begin(is);
            istream_iterator<char> end;
            for (istream_iterator<char> it = begin; it != end; ++it)
            {
                if (sequencePos >= sequence.size())
                {
                    is.seekg(pos);
                    return true;
                }

                if (*it == sequence[sequencePos])
                {
                    sequencePos++;
                }
                else
                {
                    is.seekg(pos);
                    return false;
                }
            }
            return false;
        }

    public:
        static ulong readUntil(istream &is, string &result, const char &delimiter, bool ignoreWS = false)
        {
            if (ignoreWS)
                is.setf(ios_base::skipws);
            else
                is.unsetf(ios_base::skipws);

            result.clear();
            istream_iterator<char> begin(is);
            istream_iterator<char> end;
            for (istream_iterator<char> it = begin; it != end; ++it)
            {
                if (*it == delimiter)
                {
                    break;
                }
                result += *it;
            }

            return result.size();
        }

        static ulong readUntil(istream &is, string &result, const string &delimiter, bool ignoreWS = false)
        {
            if (ignoreWS)
                is.setf(ios_base::skipws);
            else
                is.unsetf(ios_base::skipws);

            ASSERT(delimiter.size() > 0, "Delimiter Length is Zero");
            result.clear();
            char firstPart = delimiter[0];
            string lastPart = string(delimiter.begin() + 1, delimiter.end());

            istream_iterator<char> begin(is);
            istream_iterator<char> end;
            for (istream_iterator<char> it = begin; it != end; ++it)
            {
                if (*it == firstPart && sequenceCheckKeepPos(is, lastPart))
                {
                    it++;
                    break;
                }
                else
                {
                    result += *it;
                }
            }

            return result.size();
        }

        static ulong readToEnd(istream &is, string &result, bool ignoreWS = false)
        {
            if (ignoreWS)
                is.setf(ios_base::skipws);
            else
                is.unsetf(ios_base::skipws);

            result.clear();
            istream_iterator<char> begin(is);
            istream_iterator<char> end;
            for (istream_iterator<char> it = begin; it != end; it++)
            {
                result += *it;
            }
            return result.size();
        }

        static ulong readToEnd(shared_ptr<istream> is, string &result, bool ignoreWS = false)
        {
            if (ignoreWS)
                is->setf(ios_base::skipws);
            else
                is->unsetf(ios_base::skipws);

            result.clear();
            istream_iterator<char> begin(*is.get());
            istream_iterator<char> end;
            for (istream_iterator<char> it = begin; it != end; it++)
            {
                result += *it;
            }
            return result.size();
        }

        static shared_ptr<istream> subStream(istream &is, ulong start, ulong length)
        {
            istream_iterator<char> begin(is);
            istream_iterator<char> end;
            ulong counter = 0;
            vector<char> *result = new vector<char>();
            result->reserve(length);

            for (istream_iterator<char> it = begin; it != end; it++)
            {
                counter++;
                if (counter > length)
                {
                    break;
                }
                result->push_back(*it);
            }

            VectorWrapBuf<char> *resultBuf = new VectorWrapBuf(*result);
            istream *resultStream = new istream(resultBuf);
            shared_ptr<istream> ptr(resultStream);
            return ptr;
        }
    };

    struct SubStreamBuf : public streambuf
    {
        SubStreamBuf(SubStreamBuf &&) = default;
        explicit SubStreamBuf(streambuf *sbuf, streampos pos, streampos len) : m_sbuf(sbuf),
                                                                               m_pos(pos),
                                                                               m_len(len),
                                                                               m_read(0)
        {
            m_sbuf->pubseekpos(pos);

            setbuf(nullptr, 0);
        }

        SubStreamBuf &operator=(const SubStreamBuf &other)
        {
            m_sbuf = other.m_sbuf;
            m_pos = other.m_pos;
            m_len = other.m_len;
            m_read = other.m_read;

            m_sbuf->pubseekpos(m_pos);
            setbuf(nullptr, 0);
            return *this;
        }

    protected:
        int underflow()
        {
            if (m_read >= m_len)
            {
                return traits_type::eof();
            }

            return m_sbuf->sgetc();
        }

        int uflow()
        {
            if (m_read >= m_len)
            {
                return traits_type::eof();
            }

            m_read += 1;

            return m_sbuf->sbumpc();
        }

        streampos seekoff(streamoff off, ios_base::seekdir seekdir, ios_base::openmode openmode = ios_base::in | ios_base::out)
        {
            if (seekdir == ios_base::beg)
            {
                off += m_pos;
            }
            else if (seekdir == ios_base::cur)
            {
                off += m_pos + m_read;
            }
            else if (seekdir == ios_base::end)
            {
                off += m_pos + m_len;
            }

            return m_sbuf->pubseekpos(off, openmode) - m_pos;
        }

        streampos seekpos(streampos streampos, ios_base::openmode openmode = ios_base::in | ios_base::out)
        {
            streampos += m_pos;

            if (streampos > m_pos + m_len)
            {
                return -1;
            }

            return m_sbuf->pubseekpos(streampos, openmode) - m_pos;
        }

    private:
        streambuf *m_sbuf;
        streampos m_pos;
        streamsize m_len;
        streampos m_read;
    };
}