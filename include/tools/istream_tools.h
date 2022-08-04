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
            // uint pos = is.tellg();
            uint sequencePos = 0;

            istream_iterator<char> begin(is);
            istream_iterator<char> end;
            uint counter = 0;
            for (istream_iterator<char> it = begin; it != end; ++it)
            {
                counter++;
                if (sequencePos >= sequence.size())
                {
                    is.seekg(-counter, ios_base::cur);
                    return true;
                }

                if (*it == sequence[sequencePos])
                {
                    sequencePos++;
                }
                else
                {
                    is.seekg(-counter, ios_base::cur);
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

            KCore::streambuf *buf = (KCore::streambuf *)is.rdbuf();

            // char c = is.peek();
            // while (c != '\0')
            // {
            //     // pos = is.tellg();
            //     if (c == firstPart && sequenceCheckKeepPos(is, lastPart))
            //     {
            //         break;
            //     }
            //     else
            //     {
            //         // is.seekg(1, ios_base::cur);
            //         is.get();
            //         LOG(c);
            //         result += c;
            //     }
            //     c = is.peek();
            // }

            auto t = __addressof(is);
            char v;
            *t >> v;

            istream_iterator<char> begin(is);
            istream_iterator<char> end;

            for (istream_iterator<char> it = begin; it != end; ++it)
            {
                LOG(*it, is.tellg());
                if (*it == firstPart && sequenceCheckKeepPos(is, lastPart))
                {
                    break;
                }
                else
                {
                    result += *it;
                }
            }
            is.unget();
            LOG("+++++++++++++++++++++++++++++++++++++++++++++++");
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
            vector<char> result;
            result.reserve(length);

            for (istream_iterator<char> it = begin; it != end; it++)
            {
                counter++;
                if (counter > length)
                {
                    break;
                }
                result.push_back(*it);
            }

        KCore:
            streambuf *resultBuf = new KCore::streambuf(result.data(), result.size());
            istream *resultStream = new istream(resultBuf);
            shared_ptr<istream> ptr(resultStream);
            return ptr;
        }
    };
}