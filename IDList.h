#ifndef IDLIST_H
#define IDLIST_H


#include "MainCtrlSettings.h"
#include "../jsonParser/JsonParser.h"


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
        IDList(const std::string& typeTable = "");
        virtual ~IDList();

        virtual void init(MainCtrlSettings& settings, const std::string& typeTable = "");

        virtual bool readTable();
        virtual bool writeTable(const std::string& strData);
        //virtual int setFileName(const std::string& fileName);

        virtual std::string getAtributeValue(const std::string& strID, const std::string strAtribute = "");
        virtual void setAtributeValue(const std::string& strID, const std::string strAtribute, const std::string& strValue);

        virtual bool isValidID(const std::string& strId);

        virtual std::string getTableType() {return m_strTableType;}

    protected:

        bool            parseTable();
        bool            parseTable(const std::string& strData);

        std::string     m_strTablePath;
        std::string     m_strTableName;
        std::string     m_strTableTemp;
        std::string     m_strTableType;
        std::string     m_strData;

        bool            m_bTableReady;

    private:

        //std::string m_listFileName;
        //std::unordered_map<unsigned long, IDElement> m_IDElementList;
        JsonParser      m_jsonParser;
        std::string     m_strIdKeyLabel;
};

#endif // IDLIST_H
