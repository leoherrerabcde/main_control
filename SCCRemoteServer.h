#ifndef SCCREMOTESERVER_H
#define SCCREMOTESERVER_H

#include "device.h"
#include "SCCDeviceParams.h"

#include <list>
#include <vector>

class SCCRemoteServer : public Device
{
    public:
        SCCRemoteServer(const std::string& deviceName = "", bool bShowData = false);
        virtual ~SCCRemoteServer();

        virtual int init(MainCtrlSettings& settings);

        void getModifiedTable(const std::string& strIDSCC);
        void getTable(const std::string strTable);

        bool isWaitingResponse() {return m_bWaitResponse;}
        void setWaitingResponse() {m_bWaitResponse = true;}
        void clearWaitingResponse() {m_bWaitResponse=false;}
        void startConnection(const std::list<std::string>& strMemberList);
        bool isTableListEmpty() {return (m_TableList.size() == 0);}
        bool isRegisterListEmpty() {return (m_RegisterList.size() == 0);}
        void setRegisterPath(const std::string& strPath) {m_strRegisterPath = strPath;}
        bool getNextTableRequest(std::string& strBody);
        bool getNextRegisterRequest(std::string& strBody);
        std::list<std::string>& getRegisterList() {return m_RegisterList;}

        virtual bool processDataReceived(const std::string& msg = "");
        std::string getUrlPostMethod();
        std::string getUrlGetMethod(int index);
        void setUrl(std::string& strUrl) {m_urlApiRest = strUrl;}
        int getCurrentTableIndex() {return m_TableIndex;}

        bool isTableBody() {return !m_TableBody.empty();}
        std::string& getBodyFromTable(int index) {return m_TableBody[index];}
        void clearTableBody() {m_TableBody.clear();}

        void moveRegisterListToHistoryFolder();
        void moveFileToHistoFolder(const std::string& strFile);
        void setDestinationPath(const std::string& destPath) {m_registerDestination = destPath;}

        void setNumRegisterSent(int numRegs) {m_iNumRegisterSent = numRegs;}
        int getNumRegisterSent() {return m_iNumRegisterSent;}

        bool isRetryError() {bool res(m_bRetryError); m_bRetryError = false; return res;}
        void setRetryError() {m_bRetryError = true;}
        void clearRetryError() {m_bRetryError = false;}

    protected:

        void removeRegisters();

    private:

        std::string m_getModifiedUrl ;
        std::vector<std::string> m_getTableUrl ;
        std::string m_postConfirmUrl ;
        std::string m_postRegister ;
        std::string m_urlApiRest ;

        bool        m_bWaitResponse;
        std::list<std::string>  m_TableList;
        std::list<std::string>  m_RegisterList;
        std::list<std::string>  m_MemberList;
        std::string m_strRegisterPath;
        int         m_iNumRegisterSent;
        int         m_TableIndex;
        std::string m_strIDDevice;
        std::vector<std::string> m_TableBody;
        std::string m_registerDestination;
        bool m_bRetryError;
};

#endif // SCCREMOTESERVER_H
