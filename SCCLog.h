#ifndef SCCLOG_H
#define SCCLOG_H

#include <string>

class SCCLog
{
    public:
        SCCLog();
        virtual ~SCCLog();

        static void print(const std::string& msg);

    protected:

    private:
};

#endif // SCCLOG_H
