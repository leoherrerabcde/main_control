#ifndef SCCFILEMANAGER_H
#define SCCFILEMANAGER_H

#include <string>
#include <sstream>

class SCCFileManager
{
    public:
        SCCFileManager(const std::string& filename);
        virtual ~SCCFileManager();

        bool readFile(std::string& container);
        static bool readFile(const std::string& filename, std::string& container);
        static bool readFile(const std::string& filepath, const std::string& filename, std::string& container);

        bool writeFile(const std::string& container);
        static bool writeFile(const std::string& filename, const std::string& container);
        static bool writeFile(const std::string& filepath, const std::string& filename, const std::string& container);

        bool deleteFile(std::string& container);
        static bool deleteFile(const std::string& filename, std::string& container);
        static bool deleteFile(const std::string& filepath, const std::string& filename, std::string& container);

        bool copyFile(const std::string& fileSrc);
        static bool copyFile(const std::string& fileSrc, const std::string& fileDst);

        std::string getFileName() {return std::string(m_ssFile.str());}

        std::string getTempFileName();

        SCCFileManager& operator<<(/*SCCFileManager& fm,*/ const std::string& str)
        {
            if (!m_ssFile.str().length())
                m_ssFile << str;
            else
                m_ssFile << "/";
            return *this;
        }

    protected:

    private:

        std::stringstream m_ssFile;
};

#endif // SCCFILEMANAGER_H
