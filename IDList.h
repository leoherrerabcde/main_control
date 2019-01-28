#ifndef IDLIST_H
#define IDLIST_H

#include <map>
#include <unordered_map>


struct IDElement
{
    std::string m_strID;
    unsigned long m_ulID;
    bool m_bAuthorized;
    std::unordered_map<std::string, std::string> m_listAtribute;
};

class IDList
{
    public:
        IDList();
        virtual ~IDList();

        virtual void init();

        int readList();
        int writeList();
        int setFileName(const std::string& fileName);

        std::string getAtributeValue(const std::string& strID, const std::string strAtribute);
        void setAtributeValue(const std::string& strID, const std::string strAtribute, const std::string& strValue);

    protected:

    private:

        std::string m_listFileName;
        std::unordered_map<unsigned long, IDElement> m_IDElementList;
};

#endif // IDLIST_H
