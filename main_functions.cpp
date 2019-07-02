
#include "main_functions.h"

extern bool    gl_bVerbose;

static std::list<std::string> st_DeviceOrder =
{
    DEVICE_REST_SERVICE,
    DEVICE_RFID_BOMBERO,
    DEVICE_RFID_BOQUILLA,
    DEVICE_FLOWMETER,
};

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

bool processDataNewClients(std::list<CSocket*>& socketNewList,
                       std::list<CSocket*>& socketList,
                       std::unordered_map<std::string,Device*>& dvcList,
                       std::vector<Device*>& onTheFlyDvcList,
                       std::unordered_map<std::string,CSocket*>& socketMap,
                       std::list<int>& portList)
{
    bool ret = false;
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
                globalLog << "Com Port: " << pDvc.getComPort() << std::endl;
                globalLog << "PID: " << pDvc.getServicePID() << std::endl;
                //SCCLog::print(ss.str());
                Device::removeComPort(portList, pDvc.getComPort());
                socketList.push_back(*itSck);
                itSck = socketNewList.erase(itSck);
                ret = true;
                continue;
            }
        }
        ++itSck;
    }
    return ret;
}

void processDataClients(std::list<CSocket*>& sockeList,
                        std::unordered_map<std::string,Device*>& dvcList,
                        SCCAlive& keepAlive)
{
    for (auto itSck : sockeList)
    {
        auto it = dvcList.find(itSck->getIDClient());
        if (it != dvcList.end())
        {
            Device* pDevice = it->second;
            if (itSck->getSocketState() == SocketState::sckDisconnected)
            {
                pDevice->disconnect();
                continue;
            }
            std::string msg = itSck->getData();
            if (msg != "")
            {
                /*if (gl_bVerbose)
                    std::cout << pDevice->name() << ": " << msg << std::endl;*/
                keepAlive.resetTimer(pDevice->getTimerHandler());
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


bool verifyDeviceService(std::unordered_map<std::string,Device*> & dvcList, std::list<int>& portList, bool bDisable)
{
    //return false;
    while (!st_DeviceOrder.empty() && !bDisable)
    {
        auto itDvc = dvcList.find(st_DeviceOrder.front());
        st_DeviceOrder.pop_front();
        if (itDvc == dvcList.end())
            continue;
        Device* pDvc = itDvc->second;
        if (pDvc->getServicePID() == 0)
        {
            if (pDvc->getComPort() != -1)
                pDvc->launchService(portList);
            else
                pDvc->launchService();
            return true;
        }
    }
    /*for (auto itDvc : dvcList)
    {
        Device* pDvc = itDvc.second;
        if (pDvc->getServicePID() == 0)
        {
            if (pDvc->getComPort() != -1)
                pDvc->launchService(portList);
            else
                pDvc->launchService();
            return true;
        }
    }*/
    return false;
}

void sendRequestTable(std::list<CSocket*>& socketList,
                        std::unordered_map<std::string,Device*>& dvcList)
{
}

void sendData(const std::string& strDvc,
    std::unordered_map<std::string,
    CSocket*>& socketMap, std::unordered_map<std::string,Device*>& dvcList,
    const std::string& msg)
{
    auto it = socketMap.find(strDvc);
    if (it == socketMap.end())
        return;
    CSocket* pSck = it->second;
    if (pSck->getSocketState() == SocketState::sckConnected)
        pSck->sendData(msg);
    else if (pSck->getSocketState() == SocketState::sckClosed)
    {
        socketMap.erase(it);
        auto itDev = dvcList.find(strDvc);
        if (itDev != dvcList.end())
        {
            Device* pDvc = itDev->second;
            pDvc->disconnect();
        }
    }
}

void fuelTransactionTimeOut()
{
}

bool verifyDeviceTimer(std::list<CSocket*>& socketList, std::unordered_map<std::string,Device*> & dvcList, SCCAlive& keepAlive)
{
    for (auto it = socketList.begin(); it != socketList.end();)
    {
        CSocket* itSck = *it;
        auto itDvc = dvcList.find(itSck->getIDClient());
        if (itDvc != dvcList.end())
        {
            Device* pDevice = itDvc->second;
            if (!itSck->isConnected())
            {
                pDevice->disconnect();
                itSck->disconnect();
                it = socketList.erase(it);
                continue;
            }
            else
            {
                int tmrDvc = pDevice->getTimerHandler();
                if (tmrDvc)
                {
                    if (keepAlive.isTimerEvent(pDevice->getTimerHandler()))
                    {
                        pDevice->disconnect();
                        itSck->disconnect();
                        it = socketList.erase(it);
                        continue;
                    }
                }
            }
            ++it;
        }
        else
        {
            if (itSck->isConnected())
                itSck->disconnect();
            it = socketList.erase(it);
        }
    }
    return false;
}

