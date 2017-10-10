#include "configmgr.h"
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

#define MODULID_MASK 0xFF

ConfigMgr::ConfigMgr()
{
}

bool ConfigMgr::getAndCheckId(const QJsonObject& json, unsigned int& id)
{
    if (checkData(json, "Id", QJsonValue::Double) == false) {
        return false;
    }

    id = json["Id"].toInt();
    // Check range
    if ((id <= Config::ModulType::MT_NULL) || (id >= Config::ModulType::MT_MAX)) {
        cds_error("ConfigMgr::getAndCheckId - Id is out of range");
        return false;
    }
    return true;
}

bool ConfigMgr::getAndCheckProperties(const QJsonObject& json, QJsonArray& arr)
{
    if (json.contains("properties") == false) {
        cds_error("ConfigMgr::getAndCheckProperties - properties missing");
        return false;
    }
    if (json["properties"].isArray() == false) {
        cds_error("ConfigMgr::getAndCheckProperties - properties is not array");
        return false;
    }
    arr = json["properties"].toArray();
    return true;
}

bool ConfigMgr::checkData(const QJsonObject& json, const char* name, QJsonValue::Type type)
{
    if (json.contains(name) == false) {
        cds_debug("ConfigMgr::checkValue - {} does not contain in object", name);
        return false;
    }
    switch (type) {
    case QJsonValue::Bool:
        if (json[name].isBool() == false) {
            cds_error("ConfigMgr::checkValue - {} format problem - bool is required", name);
            return false;
        }
        break;

    case QJsonValue::Double:
        if (json[name].isDouble() == false) {
            cds_error("ConfigMgr::checkValue - {} format problem - double is required", name);
            return false;
        }
        break;

    case QJsonValue::String:
        if (json[name].isString() == false) {
            cds_error("ConfigMgr::checkValue - {} format problem - string is required", name);
            return false;
        }
        break;

    case QJsonValue::Array:
        if (json[name].isArray() == false) {
            cds_error("ConfigMgr::checkValue - {} format problem - array is required", name);
            return false;
        }
        break;

    case QJsonValue::Object:
        if (json[name].isObject() == false) {
            cds_error("ConfigMgr::checkValue - {} format problem - object is required", name);
            return false;
        }
        break;
    default:
        cds_error("ConfigMgr::checkValue - {} format problem - object is required", name);
        return false;
    }

    return true;
}

bool ConfigMgr::getAndCheckProperty(const QJsonObject& json, ParameterContent& param)
{
    // Id
    if (checkData(json, "Id", QJsonValue::Double) == false) {
        return false;
    }
    param._id = json["Id"].toInt();

    // Type
    if (checkData(json, "Type", QJsonValue::Double) == false) {
        return false;
    }
    param._type = json["Type"].toInt();

    // Flags
    if (checkData(json, "Flags", QJsonValue::Double) == false) {
        return false;
    }
    param._flags = json["Flags"].toInt();

    // Name
    if (checkData(json, "Name", QJsonValue::String) == false) {
        return false;
    }
    param._name = json["Name"].toString();

    // Default
    if (checkData(json, "Default", QJsonValue::String) == false) {
        return false;
    }
    param._default = json["Default"].toString();

    // LimitId
    if (checkData(json, "LimitId", QJsonValue::Double) == true) {
        param._limitId = json["LimitId"].toInt();
    }
    // MinValue
    if (checkData(json, "MinValue", QJsonValue::String) == true) {
        param._minValue = json["MinValue"].toString();
    }
    // MaxValue
    if (checkData(json, "MaxValue", QJsonValue::String) == true) {
        param._maxValue = json["MaxValue"].toString();
    }

    return true;
}

bool ConfigMgr::getAndCheckIdType(const QJsonObject& json, LimitContent& limit)
{
    // Id
    if (checkData(json, "Id", QJsonValue::Double) == false) {
        return false;
    }
    limit._id = json["Id"].toInt();

    // Type
    if (checkData(json, "Type", QJsonValue::Double) == false) {
        return false;
    }
    limit._type = json["Type"].toInt();

    return true;
}

ParametersAndLimitsManager* ConfigMgr::findModule(unsigned int moduleId)
{
    auto iterator = std::find_if(
        _palMgrContainer.begin(), _palMgrContainer.end(), [&moduleId](ParametersAndLimitsManager& Item) -> bool {
            if (Item.getIdentifier() == moduleId) {
                return true;
            }
            return false;
        });
    if (iterator == _palMgrContainer.end())
        return nullptr;

    return &(*iterator);
}

ParametersAndLimitsManager* ConfigMgr::addModule(unsigned int moduleId)
{
    auto result = findModule(moduleId);
    if (result != nullptr)
        return result;

    _palMgrContainer.push_back(ParametersAndLimitsManager(moduleId));
    return &_palMgrContainer.back();
}

unsigned int ConfigMgr::registerModule(unsigned int moduleType, Config::IConfig2* conf){
    if (conf == nullptr) {
        cds_error("ConfigMgr::registerModule - Pointer to Config2 interface is null.");
        return 0;
    }
    auto palMgr = findModule(moduleType);
    if (palMgr == nullptr) {
        cds_error("ConfigMgr::registerModule - Module type {} does not exist", std::to_string(moduleType).c_str());
        return 0;
    }
    auto handle = palMgr->registerNewUser();
    conf = (handle != 0) ? this : nullptr;

    return handle;
}

unsigned int ConfigMgr::getParamCount(unsigned int handle)
{
    auto palMgr = findModule(GETMODULEID(handle));
    if (palMgr == nullptr) {
        cds_error("ConfigMgr::getParamCount - handle {} was wrong", std::to_string(handle).c_str());
        return 0;
    }
    return palMgr->getParameterCount();
}

Config::IParam* ConfigMgr::getParam(unsigned int handle, unsigned int parameterIndex)
{
    auto palMgr = findModule(GETMODULEID(handle));
    if (palMgr == nullptr) {
        cds_error("ConfigMgr::getParam - handle {} was wrong", std::to_string(handle).c_str());
        return nullptr;
    }
    return palMgr->getParameter(parameterIndex);
}

Config::IParam* ConfigMgr::getParamById(unsigned int handle, unsigned int parameterId)
{
    auto palMgr = findModule(GETMODULEID(handle));
    if (palMgr == nullptr) {
        cds_error("ConfigMgr::getParamById - handle {} was wrong", std::to_string(handle).c_str());
        return nullptr;
    }
    return palMgr->getParameterById(parameterId);
}

bool ConfigMgr::validateAndApply(unsigned int handle, unsigned int parameterId, const char* newValue)
{
    auto palMgr = findModule(GETMODULEID(handle));
    if (palMgr == nullptr) {
        cds_error("ConfigMgr::validateAndApply - handle {} was wrong", std::to_string(handle).c_str());
        return false;
    }
    return palMgr->validateAndApply(handle, parameterId, newValue);
}

unsigned int ConfigMgr::getLimitCount(unsigned int handle)
{
    auto palMgr = findModule(GETMODULEID(handle));
    if (palMgr == nullptr) {
        cds_error("ConfigMgr::getLimitCount - handle {} was wrong", std::to_string(handle).c_str());
        return 0;
    }
    return palMgr->getLimitCount();
}

Config::ILimit* ConfigMgr::getLimit(unsigned int handle, unsigned int limitIndex)
{
    auto palMgr = findModule(GETMODULEID(handle));
    if (palMgr == nullptr) {
        cds_error("ConfigMgr::getLimit - handle {} was wrong", std::to_string(handle).c_str());
        return nullptr;
    }
    return palMgr->getLimit(limitIndex);
}

Config::ILimit* ConfigMgr::getLimitById(unsigned int handle, unsigned int limitId)
{
    auto palMgr = findModule(GETMODULEID(handle));
    if (palMgr == nullptr) {
        cds_error("ConfigMgr::getLimitId - handle {} was wrong", std::to_string(handle).c_str());
        return nullptr;
    }
    return palMgr->getLimitById(limitId);
}

bool ConfigMgr::parserConfig(const QJsonObject& json)
{
    // Id
    //===
    unsigned int id;
    if (getAndCheckId(json, id) == false)
        return false;

    auto palm = addModule(id);
    if (palm == nullptr) {
        cds_error("ConfigMgr::parserConfig - pal manager not exist in container");
        return false;
    }

    // properties
    //===========
    if (checkData(json, "properties", QJsonValue::Array) == false) {
        return false;
    }
    auto properTab = json["properties"].toArray();
    for (auto ii = 0; ii < properTab.size(); ++ii) {
        if (properTab[ii].isObject() == false) {
            cds_error("ConfigMgr::parserConfig - proper is not an object");
            return false;
        }
        ParameterContent param;
        if (getAndCheckProperty(properTab[ii].toObject(), param) == false) {
            cds_error("ConfigMgr::parserConfig - parser Object problem");
            return false;
        }
        if (palm->putParameter(param) == false) {
            cds_error("ConfigMgr::parserConfig - Problem with put new parameter occured.");
            return false;
        }
    }

    // limits
    //=======
    if (checkData(json, "limits", QJsonValue::Array) == true) {
        auto limTab = json["limits"].toArray();
        for (auto ii = 0; ii < limTab.size(); ++ii) {
            if (limTab[ii].isObject() == false) {
                cds_error("ConfigMgr::parserConfig - proper is not an object");
                return false;
            }
            LimitContent limitContent;
            if (getAndCheckIdType(limTab[ii].toObject(), limitContent) == false) {
                return false;
            }
            if (checkData(limTab[ii].toObject(), "Enum", QJsonValue::Array) == false) {
                cds_error("ConfigMgr::parserConfig - Limit does not enumTab size is 0.");
                return false;
            }
            auto enumTab = limTab[ii].toObject()["Enum"].toArray();
            if (enumTab.size() == 0) {
                cds_error("ConfigMgr::parserConfig - enumTab size is 0.");
                return false;
            }

            Limit limitItem(limitContent);
            for (auto jj = 0; jj < enumTab.size(); ++jj) {
                if (checkData(enumTab[jj].toObject(), "Name", QJsonValue::String) == false) {
                    cds_error("ConfigMgr::parserConfig - Limit item not consist of Name.");
                    return false;
                }
                if (checkData(enumTab[jj].toObject(), "Value", QJsonValue::String) == false) {
                    cds_error("ConfigMgr::parserConfig - Limit item not consist of Name.");
                    return false;
                }
                if (limitItem.putLimitToCollection(
                        enumTab[jj].toObject()["Value"].toString(), enumTab[jj].toObject()["Name"].toString())
                    == false) {
                    cds_error("ConfigMgr::parserConfig - Problem with put parameter item to the colection.");
                    return false;
                }
            }
            // limit item is ready to add to the collection
            palm->putLimits(limitItem);
        }
    }
    return true;
}

bool ConfigMgr::convertConfigFile(const QJsonObject& json)
{
    if ((json.contains("modul") == false) || (json["modul"].isArray() == false)) {
        cds_error("ConfigMgr::convertConfigFile - Module not exist or not array");
        return false;
    }
    auto arr = json["modul"].toArray();
    if (arr.count() >= Config::ModulType::MT_MAX) {
        cds_error("ConfigMgr::convertConfigFile - Array count is more then MT_MAX");
        return false;
    }
    for (auto ii = 0; ii < arr.count(); ++ii) {
        if (parserConfig(arr[ii].toObject()) == false) {
            cds_error(
                "ConfigMgr::convertConfigFile - Problem with modul configuration {}.", std::to_string(ii).c_str());
            return false;
        }
    }
    cds_info("ConfigMgr::convertConfigFile - Parameters and Limits were parsered correctly.");

    // Validate parameters with limit data
    for (size_t ii = 0; ii < _palMgrContainer.size(); ++ii) {
        if (_palMgrContainer[ii].validate() == false) {
            cds_error("ConfigMgr::convertConfigFile - Problem with validate parameters with limit data for {} module",
                std::to_string(ii).c_str());
            return false;
        }
    }

    cds_info("ConfigMgr::convertConfigFile - Validate parameters with limit data is finished correctly");
    return true;
}

bool ConfigMgr::init()
{
    QDir dir("../components//projectconfig");
    QFile file(dir.absoluteFilePath("config.cds"));

    if (file.open(QIODevice::ReadOnly) == false) {
        cds_error("Could not open configuration file");
        return false;
    }

    QByteArray wholeFile = file.readAll();
    QJsonDocument jsonFile(QJsonDocument::fromJson(wholeFile));
    QJsonObject json = jsonFile.object();

    return convertConfigFile(json);
}
