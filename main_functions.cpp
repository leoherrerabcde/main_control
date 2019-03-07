
#include "main_functions.h"

extern bool    gl_bVerbose;

bool proccesNewConnection(CSocket& sckServer, MainCtrlSettings& settings, std::list<CSocket*>& socketList)
{
    //static bool bStartUp = true;
    if (sckServer.getState() == sckHostResolved)
    {
        CSocket* newClient = sckServer.getSocket();
        newClient->setBufferSize(settings.sckBufferSize);
        newClient->runRcvLoop();
        socketList.push_back(newClient);
        sckServer.listen();
        return true;
    }
    return false;
}

void processDataNewClients(std::list<CSocket*>& socketNewList,
                       std::list<CSocket*>& socketList,
                       std::unordered_map<std::string,Device*>& dvcList,
                       std::vector<Device*>& onTheFlyDvcList,
                       std::unordered_map<std::string,CSocket*>& socketMap)
{
    for (auto itSck = socketNewList.begin(); itSck != socketNewList.end();)
    {
        Device pDvc;
        if (pDvc.processDataReceived((*itSck)->getData()))
        {
            if (pDvc.name() != "")
            {
                (*itSck)->setIDClient(pDvc.name());
                socketMap.insert(std::make_pair(pDvc.name(), *itSck));
                auto it = dvcList.find((*itSck)->getIDClient());
                if (it != dvcList.end())
                {
                    Device* pDevice = it->second;
                    pDevice->setServicePID(pDvc.getServicePID());
                    pDevice->addData(pDvc.getData());
                }
                else
                {
                    Device* pDevice = new Device(pDvc);
                    dvcList.insert(std::make_pair(pDevice->name(), pDevice));
                    onTheFlyDvcList.push_back(pDevice);
                }
                //std::stringstream ss;
                globalLog << "New Device connected: " << pDvc.name() << std::endl;
                //SCCLog::print(ss.str());
                socketList.push_back(*itSck);
                itSck = socketNewList.erase(itSck);
                continue;
            }
        }
        ++itSck;
    }
}

void processDataClients(std::list<CSocket*>& sockeList,
                        std::unordered_map<std::string,Device*>& dvcList)
{
    for (auto itSck : sockeList)
    {
        auto it = dvcList.find(itSck->getIDClient());
        if (it != dvcList.end())
        {
            Device* pDevice = it->second;
            std::string msg = itSck->getData();
            if (msg != "")
            {
                /*if (gl_bVerbose)
                    std::cout << pDevice->name() << ": " << msg << std::endl;*/
                pDevice->processDataReceived(msg);
            }
            /*else
                pDevice->processDataReceived(itSck->getData());*/
        }
    }
}

void sendAliveMessage(std::list<CSocket*>& socketList,
                        std::unordered_map<std::string,Device*>& dvcList)
{
    for (auto it = socketList.begin(); it != socketList.end();)
    {
        CSocket* itSck = *it;
        auto itDvc = dvcList.find(itSck->getIDClient());
        if (itDvc != dvcList.end())
        {
            Device* pDevice = itDvc->second;
            if (!pDevice->isServiceAlive())
            {
                SCCCreateMessage sccAliveMsg;
                sccAliveMsg.addParam(MSG_HEADER_TYPE, MSG_SERV_ALIVE_HEADER);
                sccAliveMsg.addParam(MSG_SERV_ALIVE_COUNT, std::to_string(pDevice->incAliveCounter()));
                std::string msg = sccAliveMsg.makeMessage();
                if (!itSck->isConnected())
                {
                    it = socketList.erase(it);
                    pDevice->disconnect();
                    continue;
                }
                if (!itSck->sendData(msg))
                {
                    it = socketList.erase(it);
                    pDevice->disconnect();
                    continue;
                }
            }
            else
            {
                /* Socket connected but no device detected */
                itSck->disconnect();
                it = socketList.erase(it);
                continue;
            }
        }
        ++it;
    }
}


bool verifyDeviceService(std::unordered_map<std::string,Device*> & dvcList)
{
    for (auto itDvc : dvcList)
    {
        Device* pDvc = itDvc.second;
        if (pDvc->getServicePID() == 0)
            pDvc->launchService();
        return true;
    }
    return false;
}

void sendRequestTable(std::list<CSocket*>& socketList,
                        std::unordered_map<std::string,Device*>& dvcList)
{
}

void sendData(const std::string& strDvc, std::unordered_map<std::string,CSocket*>& socketMap, const std::string& msg)
{
}

void fuelTransactionTimeOut()
{
}
