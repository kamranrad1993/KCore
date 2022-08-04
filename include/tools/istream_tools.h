#pragma once
#include <base/types.h>
#include <iostream>
#include <iterator>
#include <sstream>
#include <sys/types.h>
#include <tools/streambuf.h>
#include <vector>

namespace KCore
{
    using namespace std;
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
                    is.seekg(pos, ios_base::beg);
                    return true;
                }

                if (*it == sequence[sequencePos])
                {
                    sequencePos++;
                }
                else
                {
                    is.seekg(pos, ios_base::beg);
                    return false;
                }
            }
            return false;
        }

    public:
        static ulong readUntil(istream &is, string &result, const char &delimiter, bool ignore_ws = false)
        {
            if (ignore_ws)
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

        static ulong readUntil(istream &is, string &result, const string &delimiter, bool ignore_ws = false)
        {
            if (delimiter.length() == 1)
            {
                return readUntil(is, result, delimiter[0], ignore_ws);
            }

            if (ignore_ws)
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
                    for (int i = 0; i < lastPart.length(); i++)
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

        static ulong readToEnd(istream &is, string &result, bool ignore_ws = false)
        {
            if (ignore_ws)
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

        static ulong readToEnd(shared_ptr<istream> is, string &result, bool ignore_ws = false)
        {
            if (ignore_ws)
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
            KCore::streambuf *buf = reinterpret_cast<KCore::streambuf *>(is.rdbuf());
            void *ptr = (void *)buf->get_ptr() + start;
            KCore::streambuf *result_buf = new KCore::streambuf(ptr, length);
            istream *result_stream = new istream(result_buf);
            shared_ptr<istream> result_ptr(result_stream);
            return result_ptr;
        }
    };
}