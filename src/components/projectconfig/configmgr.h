#ifndef CONFIGMGR_H
#define CONFIGMGR_H
#include "configuration.h"
#include "log.h"
#include "palmgr.h"
#include <QtCore/QJsonValue>

class QJsonObject;
class QJsonArray;

class ConfigMgr : public Config::IConfig, public Config::IConfig2 {
public:
    ConfigMgr();

    virtual ~ConfigMgr() = default;
    /// \brief Init method that start process of convert configuration file
    /// \return true if converted was finished successful, false otherwise
    bool init();

    /// IConfig interface
    /// \see  IConfig interface
    unsigned int registerModule(unsigned int moduleType, Config::IConfig2* conf) override;

    /// IConfig2 interface
    /// \see  IConfig2 interface
    unsigned int getParamCount(unsigned int handle) override;
    /// \see  IConfig2 interface
    Config::IParam* getParam(unsigned int handle, unsigned int parameterIndex) override;
    /// \see  IConfig2 interface
    Config::IParam* getParamById(unsigned int handle, unsigned int parameterId) override;
    /// \see  IConfig2 interface
    bool validateAndApply(unsigned int handle, unsigned int parameterId, const char* newValue) override;
    /// \see  IConfig2 interface
    unsigned int getLimitCount(unsigned int handle) override;
    /// \see  IConfig2 interface
    Config::ILimit* getLimit(unsigned int handle, unsigned int limitIndex) override;
    /// \see  IConfig2 interface
    Config::ILimit* getLimitById(unsigned int handle, unsigned int limitId) override;

private:
    /// \brief Convert config file to internal structure
    /// \param[in] json Reference to json object
    /// \return true if converted was finished successful, false otherwise
    bool convertConfigFile(const QJsonObject& json);
    /// \brief Parser json object one of module type
    /// \param[in] json Reference to json object
    /// \return true if parsered was finished successful, false otherwise
    bool parserConfig(const QJsonObject& json);   
    /// \brief Get and check id
    /// \param[in] json Reference to json object
    /// \param[in] id Reference to identifier
    /// \return true if converted was finished successful, false otherwise
    bool getAndCheckId(const QJsonObject& json, unsigned int& id);
    /// \brief Check json data format
    /// \param[in] json Reference to json object
    /// \param[in] name Name of parameter
    /// \param[in] type Type of parameter
    /// \return true if parameter was processing successful, false otherwise
    bool checkData(const QJsonObject& json, const char* name, QJsonValue::Type type);
    /// \brief Get and check properties strucure
    /// \param[in] json Reference to json object
    /// \param[in] arr Reference to array properties
    /// \return true if properties was converted successful, false otherwise
    bool getAndCheckProperties(const QJsonObject& json, QJsonArray& arr);
    /// \brief Get and check property strucure
    /// \param[in] json Reference to json object
    /// \param[in] param Reference to parameter content
    /// \return true if property was converted successful, false otherwise
    bool getAndCheckProperty(const QJsonObject& json, ParameterContent& param);
    /// \brief Get and check id and type of limit structure
    /// \param[in] json Reference to json object
    /// \param[in] limit Reference to limit content
    /// \return true if parameters ware converted successful, false otherwise
    bool getAndCheckIdType(const QJsonObject& json, LimitContent& limit);
    /// \brief Find palMgr in colection
    /// \param[in] moduleId Module identifier
    /// \return Pointer to palMgr if was found, nullptr otherwise
    ParametersAndLimitsManager* findModule(unsigned int moduleId);
    /// \brief Find palMgr in colection
    /// \param[in] moduleId Module identifier
    /// \return Pointer to palMgr if was added correctly, nullptr otherwise
    ParametersAndLimitsManager* addModule(unsigned int moduleId);

    /// Parameters and limits container
    std::vector<ParametersAndLimitsManager> _palMgrContainer;
};
#endif // CONFIGMGR_H
