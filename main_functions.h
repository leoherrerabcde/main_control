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


void verifyDeviceService(std::unordered_map<std::string,Device*> & dvcList);
void proccesNewConnection(CSocket& sckServer, MainCtrlSettings& settings, std::list<CSocket*>& socketList);
void processDataNewClients(std::list<CSocket*>& socketNewList,
                       std::list<CSocket*>& socketList,
                       std::unordered_map<std::string,Device*>& dvcList,
                       std::vector<Device*>& onTheFlyDvcList,
                       std::unordered_map<std::string,CSocket*>& socketMap);

void processDataClients(std::list<CSocket*>& socketList,
                        std::unordered_map<std::string,Device*>& dvcList);
void sendAliveMessage(std::list<CSocket*>& socketList,
                        std::unordered_map<std::string,Device*>& dvcList);
void sendRequestTable(std::list<CSocket*>& socketList,
                        std::unordered_map<std::string,Device*>& dvcList);

void sendData(const std::string& strDvc, std::unordered_map<std::string,CSocket*>& socketMap, const std::string& msg);

void fuelTransactionTimeOut();


#endif // MAIN_FUNCTIONS_H_INCLUDED
