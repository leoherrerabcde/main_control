#ifndef SCCREMOTESERVER_H
#define SCCREMOTESERVER_H

#include "device.h"
#include "SCCDeviceParams.h"

class SCCRemoteServer : public Device
{
    public:
        SCCRemoteServer(const std::string& deviceName = "", bool bShowData = false);
        virtual ~SCCRemoteServer();

        virtual int init(MainCtrlSettings& settings);

        void getModifiedTable(const std::string& strIDSCC);
        void getTable(const std::string strTable);

    protected:

    private:

        std::string m_getModifiedUrl ;
        std::string m_getTableUrl ;
        std::string m_postConfirmUrl ;
        std::string m_postRegister ;
        std::string m_urlApiRest ;
};

#endif // SCCREMOTESERVER_H
