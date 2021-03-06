#ifndef SCCFUELTRANSACTION_H
#define SCCFUELTRANSACTION_H

#include "device.h"
#include "SCCFileManager.h"
#include "../commPort/SCCRealTime.h"

#include <chrono>
#include <list>

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
        bool addDriver(const std::string& strID);
        bool addVehicle(const std::string& strID);
        void addUser(std::stringstream& ss, const std::string& strID);
        void addDriver(std::stringstream& ss, const std::string& strID);
        void addVehicle(std::stringstream& ss, const std::string& strID);
        void addDispensadorId(std::stringstream& ss);
        void addFlowMeterBegin(const double dCurrentFlowAcum);
        void addFlowMeterEnd(std::stringstream& ss, const double dCurrentFlowAcum);
        void addTimeIni(std::stringstream& ss);
        void addTimeEnd(std::stringstream& ss);
        void addOdometer(std::stringstream& ss);
        void addHorometer(std::stringstream& ss);
        void addTransactionType(std::stringstream& ss);
        void addLiters(std::stringstream& ss);
        void addRegisterNumber(std::stringstream& ss, int regNum);
        bool finishTransaction(const double dCurrentFlowAcum);
        std::string getRegisterPath() {return m_strRegisterPath;}
        std::string getRegisterNewPath() {return m_strNewRegsPath;}
        void getNewRegisterJson(std::string& strJson);
        bool getRegisterList(std::list<std::string>& regList);
        std::list<std::string>& getMemberList();
        std::string getRegHistoPath();

        void setIdDispensador(const std::string& strId) {m_ID_Dispensador=strId;}
        std::string getIdDispensador() {return m_ID_Dispensador;}

        void setOdometer(const std::string& strValue) {m_strOdometer=strValue;}
        std::string getOdometer() {return m_strOdometer;}

        void setHorometer(const std::string& strValue) {m_strHorometer=strValue;}
        std::string getHorometer() {return m_strHorometer;}

        std::string  getTransactionType() {return m_strTransactionType;}
        std::string  getLiters() {return m_strLiters;}

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
        std::string     m_ID_Dispensador;
        std::string     m_strOdometer;
        std::string     m_strHorometer;
        std::string     m_strTransactionType;
        std::string     m_strLiters;
};

#endif // SCCFUELTRANSACTION_H
