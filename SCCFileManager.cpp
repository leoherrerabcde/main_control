#include "SCCFileManager.h"
#include "SCCLog.h"

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <cstring>


SCCFileManager::SCCFileManager(const std::string& filename, bool bShowdata)
{
    //ctor
    m_ssFile << filename;
}

SCCFileManager::~SCCFileManager()
{
    //dtor
}

bool SCCFileManager::readFile(const std::string& filepath, const std::string& filename, std::string& container)
{
    std::string fileName(filepath);
    fileName += "/";
    fileName += filename;

    bool res = readFile(fileName, container);

    return res;
}

bool SCCFileManager::readFile(const std::string& filename, std::string& container)
{
    std::ifstream f;

    if (filename == "")
        return false;

    f.open(filename.c_str(), std::ios::in);

    if (!f.is_open())
        return false;

    f.seekg(0, std::ifstream::end);
    size_t fileSize = f.tellg();
    if (fileSize == 0)
        return false;

    f.seekg(0,std::ifstream::beg);

    char buffer[fileSize+1];

    f.read(buffer, fileSize);
    f.close();

    buffer[fileSize] = NULL_CHAR;

    container = buffer;
    return true;
}

bool SCCFileManager::readFile(std::string& container)
{
    bool res = readFile(getFileName(), container);

    return res;
}

bool SCCFileManager::writeFile(const std::string& filepath, const std::string& filename, const std::string& container, std::ios::openmode mode)
{
    std::string fileName(filepath);
    fileName += "/";
    fileName += filename;

    bool res = writeFile(fileName, container, mode);

    return res;
}

bool SCCFileManager::writeFile(const std::string& filename, const std::string& container, std::ios::openmode mode)
{
    std::ofstream f;

    if (filename == "")
        return false;

    f.open(filename.c_str(), mode);

    if (!f.is_open())
        return false;

    f.write(container.c_str(), container.length());

    bool res = f.good();

    f.close();

    return res;
}

bool SCCFileManager::writeFile(const std::string& container, std::ios::openmode mode)
{
    std::string filename = m_ssFile.str();

    return writeFile(filename, container, mode);
}

bool SCCFileManager::appendToFile(const std::string& container)
{
    std::string filename = m_ssFile.str();

    return appendToFile(filename, container);
}

bool SCCFileManager::appendToFile(const std::string& filename, const std::string& container)
{
    return writeFile(filename, container, std::ios::app);
}

bool SCCFileManager::appendToFile(const std::string& filepath, const std::string& filename, const std::string& container)
{
    std::string fileName(filepath);
    fileName += "/";
    fileName += filename;

    bool res = appendToFile(fileName, container);

    return res;
}

std::string SCCFileManager::getTempFileName()
{
    std::string strFile("tmp_");
    strFile += "1234";
    strFile += ".json";
    return strFile;
}

bool SCCFileManager::isFileExist()
{
    std::string strFile(m_ssFile.str());
    return isFileExist(strFile);
}

bool SCCFileManager::isFileExist(const std::string& filename)
{
    struct stat buffer;
    return (stat (filename.c_str(), &buffer) == 0);
}

bool SCCFileManager::isFolder()
{
    std::string strFile(m_ssFile.str());
    return isFolder(strFile);
}

bool SCCFileManager::isFolder(const std::string& filename)
{
    struct stat stat_buf;
    if (stat (filename.c_str(), &stat_buf) != 0)
        return false;
    return (S_ISDIR(stat_buf.st_mode));
}

bool SCCFileManager::moveFile(const std::string& fileDst)
{
    std::string strFile(m_ssFile.str());
    return moveFile(strFile, fileDst);
}

bool SCCFileManager::moveFile(const std::string& fileSrc, const std::string& fileDst)
{
    int res = rename(fileSrc.c_str(), fileDst.c_str());
    return (res == 0);
}

void SCCFileManager::getFileList(std::list<std::string>& listFile)
{
    std::string strFile(m_ssFile.str());
    getFileList(strFile, listFile);
}

void SCCFileManager::getFileList(const std::string& path, std::list<std::string>& listFile)
{
    DIR* dir = opendir(path.c_str());
    if (dir == NULL)
        return;
    struct dirent *ent;

    while ((ent = readdir(dir)) != NULL)
    {
        if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0)
        {
            std::string strTmp(path);
            strTmp += "/";
            strTmp += ent->d_name;
            if (!isFolder(strTmp))
                listFile.push_back(ent->d_name);
        }
    }
    closedir(dir);
}

bool SCCFileManager::copyFile(const std::string& fileDst)
{
    return copyFile(fileDst, getFileName());
}

bool SCCFileManager::copyFile(const std::string& fileDst, const std::string& fileSrc)
{
    std::ifstream src(fileSrc, std::ios::binary);
    std::ofstream dst(fileDst, std::ios::binary);
    dst << src.rdbuf();
    return true;
}

bool SCCFileManager::deleteFile()
{
    return deleteFile(getFileName());
}

bool SCCFileManager::deleteFile(const std::string& filename)
{
    if (!isFileExist(filename))
        return true;

    return (remove(filename.c_str()) == 0);
}

bool SCCFileManager::deleteFile(const std::string& filepath, const std::string& filename)
{
    std::string fileName(filepath);
    fileName += "/";
    fileName += filename;

    bool res = deleteFile(fileName);

    return res;
}
