#ifndef SCCFUELTRANSACTION_H
#define SCCFUELTRANSACTION_H

#include "device.h"
#include "SCCFileManager.h"
#include "../commPort/SCCRealTime.h"

#include <chrono>

#define INVALID_REGISTER_NUM    -1
#define LOWER_REGISTER_NUM      1

class SCCFuelTransaction : public Device
{
    public:
        SCCFuelTransaction(const std::string& deviceName = "", bool bShowdata = false);
        virtual ~SCCFuelTransaction();

        virtual int init(MainCtrlSettings& settings);

        bool initTransaction(const double dCurrentFlowAcum);
        bool addUser(const std::string& strID);
        bool addVehicle(const std::string& strID);
        void addFlowMeterBegin(const double dCurrentFlowAcum);
        void addFlowMeterEnd(std::stringstream& ss, const double dCurrentFlowAcum);
        void addTimeIni(std::stringstream& ss);
        void addTimeEnd(std::stringstream& ss);
        void addRegisterNumber(std::stringstream& ss, int regNum);
        bool finishTransaction(const double dCurrentFlowAcum);

    protected:

        int             getLastRegisterNumber();
        int             getLastRegisterNumber(const std::string& strPath);
        int             getRegisterNumber(const std::string& strFileName);
        std::string     regNumber2String(const int regNumber);
        void            clearRegData() {m_ssRegData.str(std::string());}
        bool            appendRegData();


    private:

        std::string     m_strFileExtension;
        SCCFileManager  m_filemanRegister;
        std::string     m_strNewRegsPath;
        std::string     m_strHistoRegsPath;
        int             m_iConsecutiveNumber;
        std::string     m_strRegisterPath;
        std::string     m_strRegisterName;
        int             m_iConseNumLength;
        int             m_iUpperRegNum;
        std::stringstream   m_ssRegData;

};

#endif // SCCFUELTRANSACTION_H
