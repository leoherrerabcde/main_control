#ifndef SCCFILEMANAGER_H
#define SCCFILEMANAGER_H

#include <string>
#include <sstream>
#include <list>


class SCCFileManager
{
    public:
        SCCFileManager(const std::string& filename = "", bool bShowdata = false);
        virtual ~SCCFileManager();

        bool readFile(std::string& container);
        static bool readFile(const std::string& filename, std::string& container);
        static bool readFile(const std::string& filepath, const std::string& filename, std::string& container);

        bool writeFile(const std::string& container, std::ios::openmode mode = std::ios::out);
        static bool writeFile(const std::string& filename, const std::string& container, std::ios::openmode mode = std::ios::out);
        static bool writeFile(const std::string& filepath, const std::string& filename, const std::string& container, std::ios::openmode mode = std::ios::out);

        bool appendToFile(const std::string& container);
        static bool appendToFile(const std::string& filename, const std::string& container);
        static bool appendToFile(const std::string& filepath, const std::string& filename, const std::string& container);

        bool deleteFile();
        static bool deleteFile(const std::string& filename);
        static bool deleteFile(const std::string& filepath, const std::string& filename);

        bool copyFile(const std::string& fileDst);
        static bool copyFile(const std::string& fileSrc, const std::string& fileDst);

        bool moveFile(const std::string& fileDst);
        static bool moveFile(const std::string& fileSrc, const std::string& fileDst);

        bool isFileExist();
        static bool isFileExist(const std::string& filename);

        std::string getFileName() {return std::string(m_ssFile.str());}

        std::string getTempFileName();

        void getFileList(std::list<std::string>& listFile);
        static void getFileList(const std::string& path, std::list<std::string>& listFile);


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
