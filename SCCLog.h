#ifndef SCCLOG_H
#define SCCLOG_H

#include <string>
#include <ostream>

#include "SCCFileManager.h"

#define CR_CHAR     '\r'
#define LF_CHAR     '\n'
#define NULL_CHAR   '\0'



class SCCLog : public std::ostream
{
    public:
        SCCLog(std::ostream& _out, const std::string& logPath = "", const size_t fileMaxSize = 0)
            : out(_out), m_strLogPath(logPath), m_iLogSize(0), m_iLogMaxSize(fileMaxSize)
        {
            if (m_iLogMaxSize && m_strLogPath != "")
            {
                m_bLogEn = true;
                m_bLogOpened = true;
            }
            else
            {
                m_bLogEn = false;
                m_bLogOpened = false;
            }
        }
        virtual ~SCCLog();

        /*template<class T>
        SCCLog& operator<<(const T& v)
        {
            out << v;
            return *this;
        }*/
    /*friend SCCLog& operator<<(SCCLog& _os, std::StandardEndLine endLineManip)
    {
        endLineManip(_os.out);
        return _os;
    }

    template<class T>
    friend SCCLog& operator<<(SCCLog& os, const T& s)
    {
        os.out << s;
        return os;
    }*/
    template<typename T>
    SCCLog& operator<<(const T& s)
    {
        out << s;
        /*if (!m_bLogEn)
            m_iLogSize = */
        return *this;
    }

    SCCLog& operator<<(std::ostream& (*s)(std::ostream&))
    {
        out << s;
        return *this;
    }


    protected:
        std::ostream&   out;
        std::string     m_strLogPath;
        std::string     m_strLogFile;
        SCCFileManager  m_fmLogFile;
        bool            m_bLogEn;
        bool            m_bLogOpened;
        size_t          m_iLogSize;
        size_t          m_iLogMaxSize;

    private:
        void print(const std::string& msg);
};

#endif // SCCLOG_H
