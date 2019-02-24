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

        static bool deleteFile(const std::string& filename, std::string& container);
        static bool deleteFile(const std::string& filepath, const std::string& filename, std::string& container);

        static bool copyFile(const std::string& filesrc, const std::string& filedst);

    protected:

    private:

        std::streamstring m_ssFile;
};

#endif // SCCFILEMANAGER_H
