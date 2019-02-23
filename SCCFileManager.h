#ifndef SCCFILEMANAGER_H
#define SCCFILEMANAGER_H

#include <string>

class SCCFileManager
{
    public:
        SCCFileManager();
        virtual ~SCCFileManager();

        static bool readFile(const std::string& filename, std::string& container);
        static bool readFile(const std::string& filepath, const std::string& filename, std::string& container);
        static bool writeFile(const std::string& filename, std::string& container);
        static bool writeFile(const std::string& filepath, const std::string& filename, std::string& container);

    protected:

    private:
};

#endif // SCCFILEMANAGER_H
