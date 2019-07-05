#ifndef MAIN_FUNCTIONS_H_INCLUDED
#define MAIN_FUNCTIONS_H_INCLUDED


#include <iostream>
#include <unistd.h>
#include <unordered_map>
#include <list>


#include "lucesestado.h"
#include "commgsm.h"
#include "electrovalvctrl.h"
#include "rfidboquilla.h"
#include "rfiduser.h"
#include "mainstate.h"
#include "IDList.h"
#include "MainCtrlSettings.h"
#include "CSocket.h"
#include "SCCLog.h"
#include "SCCAlive.h"
#include "SCCRemoteServer.h"
#include "SCCFlowmeter.h"
#include "SCCDeviceNames.h"
#include "SCCCreateMessage.h"
#include "SCCFuelTransaction.h"


std::string verifyDeviceService(std::list<std::string>& deviceList, std::unordered_map<std::string,Device*> & dvcList, std::list<int>& portList, bool bDisable = false);
bool verifyDeviceTimer(std::list<CSocket*>& socketList, std::unordered_map<std::string,Device*> & dvcList, std::list<std::string>& deviceList, SCCAlive& keepAlive);
bool proccesNewConnection(CSocket& sckServer, MainCtrlSettings& settings, std::list<CSocket*>& socketList);
bool processDataNewClients(std::list<CSocket*>& socketNewList,
                       std::list<CSocket*>& socketList,
                       std::unordered_map<std::string,Device*>& dvcList,
                       std::vector<Device*>& onTheFlyDvcList,
                       std::unordered_map<std::string,CSocket*>& socketMap,
                       std::list<int>& portList);

void processDataClients(std::list<CSocket*>& socketList,
                        std::unordered_map<std::string,Device*>& dvcList,
                        SCCAlive& keepAlive);
void sendAliveMessage(std::list<CSocket*>& socketList,
                        std::unordered_map<std::string,Device*>& dvcList);
void sendRequestTable(std::list<CSocket*>& socketList,
                        std::unordered_map<std::string,Device*>& dvcList);

void sendData(const std::string& strDvc,
                std::unordered_map<std::string,CSocket*>& socketMap,
                std::unordered_map<std::string,Device*>& dvcList,
                const std::string& msg);

void fuelTransactionTimeOut();

std::string aliveMsg(std::unordered_map<std::string,Device*> & dvcList, std::unordered_map<std::string, CSocket*>& socketMap);


#endif // MAIN_FUNCTIONS_H_INCLUDED
