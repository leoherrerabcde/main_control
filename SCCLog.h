#ifndef SCCLOG_H
#define SCCLOG_H

#include <string>
#include <ostream>


#define CR_CHAR     '\r'
#define LF_CHAR     '\n'
#define NULL_CHAR   '\0'



class SCCLog : public std::ostream
{
    public:
        SCCLog(std::ostream& _out) : out(_out) {};
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
        return *this;
    }

    SCCLog& operator<<(std::ostream& (*s)(std::ostream&))
    {
        out << s;
        return *this;
    }


    protected:
        std::ostream& out;

    private:
        void print(const std::string& msg);
};

#endif // SCCLOG_H
