#include "palmgr.h"

static bool checkStrWithType(const QString& str, unsigned int type)
{
    auto convertInfo = false;
    switch (type) {
    case Config::ParameterType::PT_BOOL: {
        auto ii = str.toInt(&convertInfo);
        if ((ii == 0) || (ii == 1)) {
            convertInfo = true;
        }
        break;
    }
    case Config::ParameterType::PT_INT:
        str.toUInt(&convertInfo);
        break;
    case Config::ParameterType::PT_DOUBLE:
        str.toDouble(&convertInfo);
        break;
    case Config::ParameterType::PT_STRING:
        convertInfo = true;
        break;

    default:
        cds_error("checkStrWithType - type is not recognized");
        break;
    }

    return convertInfo;
}

bool Parameter::attach(unsigned int handle, Config::IOnChange* onChange)
{
    if (onChange == nullptr)
        return false;

    auto user = _userDef.find(GETUSERID(handle));
    if (user == _userDef.end()) {
        cds_error("Parameter::attach - handle {} was wrong", std::to_string(handle).c_str());
        return false;
    }
    user->second._onChange.insert(onChange);
    return true;
}

bool Parameter::detach(unsigned int handle, Config::IOnChange* onChange)
{
    if (onChange == nullptr)
        return false;

    auto user = _userDef.find(GETUSERID(handle));
    if (user == _userDef.end()) {
        cds_error("Parameter::detach - handle {} was wrong", std::to_string(handle).c_str());
        return false;
    }
    user->second._onChange.erase(onChange);
    return true;
}

bool Parameter::setNewValue(unsigned int userId, const QString& newValue)
{
    auto user = _userDef.find(userId);
    if (user == _userDef.end()) {
        cds_error("Parameter::setNewValue - userId {} was wrong", std::to_string(userId).c_str());
        return false;
    }

    if (user->second._currentValue != newValue) {
        user->second._currentValue = newValue;
        for (auto& it : user->second._onChange) {
            it->onChange(_content._id, user->second._currentValue.toStdString().c_str());
        }
    }
    return true;
}

const char* Parameter::getValue(unsigned int handle) const
{
    auto user = _userDef.find(GETUSERID(handle));
    if (user == _userDef.end()) {
        cds_error("Parameter::GetValue - handle {} was wrong", std::to_string(handle).c_str());
        return nullptr;
    }

    return user->second._currentValue.toStdString().c_str();
}

bool Parameter::registerNewUser(unsigned int userId)
{
    if (_userDef.find(userId) != _userDef.end()) {
        cds_error("Parameter::registerNewUser - userId {} just exist", std::to_string(userId).c_str());
        return false;
    }

    _userDef.insert(std::make_pair(userId, UserItem(_content._default)));
    return true;
}

bool Parameter::unregisterUser(unsigned int userId)
{
    if (_userDef.find(userId) == _userDef.end()) {
        cds_debug(" Parameter::unregisterUse - User id {} does not exist", std::to_string(userId).c_str());
        return false;
    }
    _userDef.erase(userId);
    return true;
}

bool Limit::putLimitToCollection(const QString& value, const QString& name)
{
    if (value.length() == 0) {
        cds_error("Limit::putLimitToCollection - Value is empty");
        return false;
    }
    if (name.length() == 0) {
        cds_error("Limit::putLimitToCollection - Name is empty");
        return false;
    }
    if (checkStrWithType(value, _type) == false) {
        cds_error("Limit::putLimitToCollection - Name is empty");
        return false;
    }
    _limitCollection.push_back(LimitItem(value, name));
    return true;
}

const char* Limit::getName(unsigned int index) const
{
    if (index >= _limitCollection.size()) {
        cds_debug("Limit::GetName - Index {} does not exist", std::to_string(index).c_str());
        return nullptr;
    }
    return _limitCollection[index]._name.toStdString().c_str();
}

const char* Limit::getValue(unsigned int index) const
{
    if (index >= _limitCollection.size()) {
        cds_debug("Limit::GetValue - Index {} does not exist", std::to_string(index).c_str());
        return nullptr;
    }
    return _limitCollection[index]._value.toStdString().c_str();
}

bool Limit::checkValueExist(const QString& value)
{
    auto iterator = std::find_if(_limitCollection.begin(), _limitCollection.end(), [&value](LimitItem& item) -> bool {
        if (item._value == value) {
            return true;
        }
        return false;
    });
    if (iterator == _limitCollection.end())
        return false;

    return true;
}

ParametersAndLimitsManager::ParametersAndLimitsManager(unsigned int ModuleId)
    : _moduleId(ModuleId)
    , _counter(0)
{
}

void ParametersAndLimitsManager::putLimits(const Limit& newLimit)
{
    _limitsCollection.push_back(newLimit);
}

unsigned int ParametersAndLimitsManager::getIdentifier() const
{
    return _moduleId;
}

unsigned long ParametersAndLimitsManager::getParameterCount() const
{
    return _parametersCollection.size();
}

Parameter* ParametersAndLimitsManager::getParameter(unsigned long index) const
{
    if (index >= _parametersCollection.size()) {
        cds_debug("ParametersAndLimitsManager::GetParameter - Parametr index {}, not exist for modul {}",
            std::to_string(index).c_str(), std::to_string(getIdentifier()).c_str());
        return nullptr;
    }
    return &_parametersCollection[index];
}

Parameter* ParametersAndLimitsManager::getParameterById(unsigned long id) const
{
    auto iterator
        = std::find_if(_parametersCollection.begin(), _parametersCollection.end(), [&id](Parameter& Item) -> bool {
              if (Item.getId() == id) {
                  return true;
              }
              return false;
          });
    if (iterator == _parametersCollection.end())
        return nullptr;

    return &(*iterator);
}

bool ParametersAndLimitsManager::validateAndApply(unsigned int handle, unsigned int id, const char* newValue)
{
    if ((_moduleId != GETMODULEID(handle))  || (_counter < GETUSERID(handle)))
    {
            cds_debug("ParametersAndLimitsManager::validateAndApply - invalid handle {}", std::to_string(handle).c_str());
            return false;
        }

    auto param = getParameterById(id);
    if (param == nullptr) {
        cds_debug("ParametersAndLimitsManager::validateAndApply - id {} does not exist", std::to_string(id).c_str());
        return false;
    }
    if (newValue == nullptr) {
        cds_debug("ParametersAndLimitsManager::validateAndApply - newValue is nullptr");
        return false;
    }
    QString value(newValue);
    if (value.length() == 0) {
        cds_debug("ParametersAndLimitsManager::validateAndApply - newValue length is 0");
        return false;
    }
    if (checkValueInRange(
            QString::fromUtf8(param->getMinValue()), QString::fromUtf8(param->getMaxValue()), value, param->getType())
        == false) {
        cds_debug("ParametersAndLimitsManager::validateAndApply - newValue does not in range");
        return false;
    }
    if (param->getLimitId() > 0) {
        if (checkValueInLimit(param->getLimitId(), value) == false) {
            cds_debug("ParametersAndLimitsManager::validateAndApply - newValue is out of limit");
            return false;
        }
    }
    if (param->setNewValue(GETUSERID(handle), value) == false) {
        cds_info("ParametersAndLimitsManager::validateAndApply - problem with set new value");
        return false;
    }

    cds_info("ParametersAndLimitsManager::validateAndApply - newValue is validate and apply correctly");
    return true;
}

unsigned long ParametersAndLimitsManager::getLimitCount() const
{
    return _limitsCollection.size();
}

Limit* ParametersAndLimitsManager::getLimit(unsigned long index) const
{
    if (index >= _limitsCollection.size()) {
        cds_debug("ParametersAndLimitsManager::GetLimit - Limit index {}, not exist for modul {}",
            std::to_string(index).c_str(), std::to_string(getIdentifier()).c_str());
        return nullptr;
    }
    return &_limitsCollection[index];
}

Limit* ParametersAndLimitsManager::getLimitById(unsigned long id) const
{
    auto iterator = std::find_if(_limitsCollection.begin(), _limitsCollection.end(), [&id](Limit& Item) -> bool {
        if (Item.getId() == id) {
            return true;
        }
        return false;
    });
    if (iterator == _limitsCollection.end())
        return nullptr;

    return &(*iterator);
}

bool ParametersAndLimitsManager::checkValueInLimit(unsigned long id, const QString& value) const
{
    auto lim = getLimitById(id);
    if (lim == nullptr) {
        cds_warn("ParametersAndLimitsManager::checkValueInLimit - Limit id {} not exist", std::to_string(id).c_str());
        return true;
    }
    if (lim->checkValueExist(value) == false) {
        cds_warn("ParametersAndLimitsManager::checkValueInLimit - value {} does not exist in the limit collectiont",
            value.toStdString().c_str());
        return false;
    }
    return true;
}

bool ParametersAndLimitsManager::findId(unsigned int id)
{
    auto iterator
        = std::find_if(_parametersCollection.begin(), _parametersCollection.end(), [&id](Parameter& Item) -> bool {
              if (Item.getId() == id) {
                  return true;
              }
              return false;
          });
    if (iterator == _parametersCollection.end())
        return false;

    return true;
}

bool ParametersAndLimitsManager::checkValueInRange(
    const QString min, const QString max, const QString value, unsigned int type)
{
    if ((min.length() == 0) ^ (max.length() == 0)) {
        cds_error("ParametersAndLimitsManager::checkValueInRange - Lack of parameters MinValue/MaxValue");
        return false;
    }
    if (min.length() && max.length()) {
        if (checkStrWithType(min, type) == false) {
            cds_error("ParametersAndLimitsManager::checkValueInRange - MinValue does not match the type");
            return false;
        }
        if (checkStrWithType(max, type) == false) {
            cds_error("ParametersAndLimitsManager::checkValueInRange - MaxValue does not match the type");
            return false;
        }
        switch (type) {
        case Config::ParameterType::PT_BOOL:
        case Config::ParameterType::PT_INT: {
            auto minValue = min.toUInt();
            auto maxValue = max.toUInt();
            auto curValue = value.toUInt();
            if (CheckRange<unsigned int>(minValue, maxValue, curValue) == false) {
                cds_error("ParametersAndLimitsManager::checkValueInRange - value in range problem occured!");
                return false;
            }
            break;
        }
        case Config::ParameterType::PT_DOUBLE: {
            auto minValue = min.toDouble();
            auto maxValue = max.toDouble();
            auto curValue = value.toDouble();
            if (CheckRange<double>(minValue, maxValue, curValue) == false) {
                cds_error("ParametersAndLimitsManager::checkValueInRange - value in range problem occured!");
                return false;
            }
            break;
        }
        case Config::ParameterType::PT_STRING:
            break;
        }
    }

    return true;
}

bool ParametersAndLimitsManager::putParameter(const ParameterContent& newParameter)
{
    // Id
    if (findId(newParameter._id) == true) {
        cds_error("ParametersAndLimitsManager::PutParameter - Id already exist");
        return false;
    }
    // Type
    if ((newParameter._type <= Config::ParameterType::PT_NULL)
        || (newParameter._type >= Config::ParameterType::PT_MAX)) {
        cds_error("ParametersAndLimitsManager::PutParameter - Type is unrecognized");
        return false;
    }
    // Flag
    if ((newParameter._flags & (~PFLAG_SUPPORTED_MASK)) > 0) {
        cds_error("ParametersAndLimitsManager::PutParameter - Flag has too many parameters");
        return false;
    }
    // Name
    if (newParameter._name.length() == 0) {
        cds_error("ParametersAndLimitsManager::PutParameter - Name of parameter does not set");
        return false;
    }

    // Default
    if (newParameter._default.length() == 0) {
        cds_error("ParametersAndLimitsManager::PutParameter - Default of parameter does not set");
        return false;
    }
    if (checkStrWithType(newParameter._default, newParameter._type) == false) {
        cds_error("ParametersAndLimitsManager::PutParameter - Default is in wrong type");
        return false;
    }
    // Min/Max/Default
    if (checkValueInRange(newParameter._minValue, newParameter._maxValue, newParameter._default, newParameter._type)
        == false) {
        cds_error("ParametersAndLimitsManager::PutParameter - MinValue/MaxValue is in wrong type or mixmash");
        return false;
    }

    if (newParameter._limitId > 0)
        _requiredLimitIds.insert(newParameter._limitId);

    _parametersCollection.push_back(Parameter(newParameter));
   // _parametersCollection.back().setNewValue(newParameter._default);

    return true;
}

bool ParametersAndLimitsManager::validate()
{
    if (_limitsCollection.size() > _requiredLimitIds.size()) {
        cds_warn("ParametersAndLimitsManager::Validate - Numbers of limits is higher than required in this module");
    }
    if (_limitsCollection.size() < _requiredLimitIds.size()) {
        std::set<unsigned int> missingIds;
        for (auto id : _requiredLimitIds) {
            auto iter
                = std::find_if(_limitsCollection.begin(), _limitsCollection.end(), [&id](const Limit& item) -> bool {
                      if (item.getId() == id)
                          return true;
                      return false;
                  });
            if (iter == _limitsCollection.end()) {
                missingIds.insert((id));
            }
        }
        QString info("Missing limits id: ");
        for (auto id : missingIds) {
            info.append(QString::number(id));
            info.append(" ");
        }
        cds_error("ParametersAndLimitsManager::Validate - Missing some limits to validate {} module",
            std::to_string(_moduleId).c_str());
        cds_error("{}", info.toStdString().c_str());
        return false;
    }

    for (auto lim : _limitsCollection) {
        if (_requiredLimitIds.find(lim.getId()) == _requiredLimitIds.end()) {
            cds_error(
                "ParametersAndLimitsManager::Validate - Limit {} was not found", std::to_string(lim.getId()).c_str());
            return false;
        }
    }

    cds_info(
        "ParametersAndLimitsManager::Validate - Modul {} was validated correctly", std::to_string(_moduleId).c_str());
    return true;
}

unsigned int ParametersAndLimitsManager::registerNewUser()
{
    ++_counter;
    for (auto& iter : _parametersCollection) {
        if (iter.registerNewUser(_counter) == false) {
            cds_error("ParametersAndLimitsManager::registerNewUser - Parameter cannot create new user.");
            return 0;
        }
    }
    return CREATEHANDLE(_counter,_moduleId);
}

bool ParametersAndLimitsManager::unregisterUser(unsigned int handle)
{
    auto userId = GETUSERID(handle);
    for (auto& iter : _parametersCollection) {
        if (iter.unregisterUser(userId) == false) {
            cds_error("ParametersAndLimitsManager::unregisterUser - Parameter cannot unregister user.");
            return false;
        }
    }
    return true;
}
