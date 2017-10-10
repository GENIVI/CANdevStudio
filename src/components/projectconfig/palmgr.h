#ifndef PALMGR_H
#define PALMGR_H
#include "configuration.h"
#include "log.h"
#include <map>
#include <set>

#define GETUSERID(handle) ((unsigned int)(handle >> 8))
#define GETMODULEID(handle) ((unsigned int)(handle & 0xFF))
#define CREATEHANDLE(counter,id) ((unsigned int)((counter << 8) | id))

/// \Structure ParameterContent
/// \The structure groups the all parameters content
struct ParameterContent {
    ParameterContent()
    {
        _id = 0;
        _type = 0;
        _flags = 0;
        _limitId = 0;
        _name.clear();
        _default.clear();
        _currentValue.clear();
        _minValue.clear();
        _maxValue.clear();
    }
    ///  _id  Unique parameter Id number
    unsigned int _id;
    /// _type Parameter type value (description by PT_xxx types)
    unsigned char _type;
    /// _flags Parameter flags
    unsigned int _flags;
    /// _limitId Unique Id of limits set or 0 (not required)
    unsigned int _limitId;
    /// _name Name of parameter
    QString _name;
    /// _default Default value of parameter
    QString _default;
    /// _currentValue Current value of parameter
    QString _currentValue;
    /// _minValue Low range on parameter (not required)
    QString _minValue;
    /// _maxValue High range of parameter (not required)
    QString _maxValue;
};

/// \brief Structure LimitContent
/// \The structure describe limit content header
struct LimitContent {
    /// _id Unique Limit Id number
    unsigned int _id;
    /// type Parameter type value (description by PT_xxx types)
    unsigned char _type;
};

/// \class Parameter
/// \The class delivers and describe single parameter attributes
class Parameter : public Config::IParam {
public:
    /// \brief Constructor
    Parameter(const ParameterContent& param)
        : _content(param)
    {
    }
    /// \brief destructor
    virtual ~Parameter() = default;

public:
    //===Config::IParam interface - begin ===
    /// \brief Method returns parameter unique (through module) Id number
    /// \return Parameter Id
    unsigned int getId() const override
    {
        return _content._id;
    }

    /// \brief Method returns parameter name
    /// \return String containing parameter name
    const char* getName() const override
    {
        return _content._name.toStdString().c_str();
    }

    /// \brief Returns parameter type
    /// \return Parameter type unsigned char value
    unsigned char getType() const override
    {
        return _content._type;
    }

    /// \brief Returns 32-Bit parameter Flags set
    /// \return Parameter flags unsigned int value
    unsigned int getFlags() const override
    {
        return _content._flags;
    }

    /// \brief Returns unique Id of Limits set for parameter
    /// \return Unique Id of limits set (NULL means not used)
    unsigned int getLimitId() const override
    {
        return _content._limitId;
    }

    /// \brief Returns parameter default value
    /// \return String containing parameter default value
    const char* getDefault() const override
    {
        return _content._default.toStdString().c_str();
    }

    /// \brief Returns parameter current value
    /// \param[in] handle Id of called modul
    /// \return string containing parameter current value
    const char* GetValue(unsigned int handle) const override;

    /// \brief Returns parameter maximum value, if specified
    /// \return String containing parameter minimum value
    const char* getMinValue() const override
    {
        return _content._minValue.toStdString().c_str();
    }

    /// \brief Returns parameter minimum value, if specified
    /// \return String containing parameter maximum value
    const char* getMaxValue() const override
    {
        return _content._maxValue.toStdString().c_str();
    }
    /// \brief Attach observer for parameter changed
    /// \param[in] handle Id of called modul
    /// \param[in] onChange Pointer to IOnChange interface
    /// \return true if interface was added or exist in collection, false otherwise
    bool attach(unsigned int handle, Config::IOnChange* onChange) override;

    /// \brief Detach observer for parameter changed
    /// \param[in] handle Id of called modul
    /// \param[in] onChange Pointer to IOnChange interface
    /// \return true if interface was removed or not exist in collection, false otherwise
    bool detach(unsigned int handle, Config::IOnChange* onChange) override;
    //=== Config::IParam interface - end ===

    /// \brief method used to change current value to new one
    /// \param[in] userId Id of user
    /// \param[in] NewValue - new parameter value to set
    /// \return true if new parameter was set, false otherwise
    bool setNewValue(unsigned int userId, const QString& newValue);

    /// \brief method to register new user of parameter
    /// \param[in] userId Id of user
    /// \return true is user was added to parameter, false otherwise
    bool registerNewUser(unsigned int userId);

    /// \brief method to unregister user of parameter
    /// \param[in] userId Id of user
    /// \return true is user was removed from parameter, false otherwise
    bool unregisterUser(unsigned int userId);

private:
    struct UserItem {
        UserItem(const QString& defaultValue) : _currentValue(defaultValue) {}
        // current value of parameter
        QString _currentValue;
        // onChanhe collection
        std::set<Config::IOnChange*> _onChange;
    };

    // Parameter content describe all of possible parameter elements
    ParameterContent _content;
    // Maps of all registration users
    std::map < unsigned int, UserItem> _userDef;
};

///	\class Limit
///	\brief This class covers a collection of limits and all functions needed to put and get
/// limit and its attributes
class Limit : public Config::ILimit {
public:
    /// \brief Constructor
    Limit(LimitContent& content)
        : _id(content._id)
        , _type(content._type){};

    /// \brief Destructor
    virtual ~Limit() = default;

    //=== Config::ILimitEnum - begin ===
    /// \brief Get limit enum unique Id number
    /// \return LimitEnum unique (through module) Id number
    unsigned int getId() const override
    {
        return _id;
    }

    /// \brief Get limit enum type
    /// \return Limit enum type unsigned char value
    unsigned char getType() const override
    {
        return _type;
    }

    /// \brief Returns number of limit enum possible values
    /// \return Number of possible values
    unsigned int getCount() const override
    {
        return _limitCollection.size();
    }

    /// \brief Returns Name corresponding with values of given index
    /// \param[in] index Index of value name to get (0 to COUNT-1)
    /// \return String containing Name of value with given index
    const char* getName(unsigned int index) const override;

    /// \brief Returns value of given index
    /// \param[in] Index Index of value to get (0 to COUNT-1)
    /// \return String containing value with given index
    const char* getValue(unsigned int index) const override;
    //=== Config::ILimit - end ===

    /// \brief Put the new limit record into collection
    /// \param[in] value - first limit description
    /// \param[in] name - second limit description
    /// \return true if recored was added to the collection, otherwise return false
    bool putLimitToCollection(const QString& value, const QString& name);

    /// \brief Check if value is in limit collection
    /// \param[in] value - Reference to value string
    /// \return true if recored with string exist, false otherwise
    bool checkValueExist(const QString& value);

private:
    /// Structure keeps description limit: value and name
    struct LimitItem {
        LimitItem(const QString value, const QString name)
            : _value(value)
            , _name(name){};
        QString _value;
        QString _name;
    };
    /// Unique limit Id number
    unsigned int _id;
    /// Limit type value (description by PT_xxx types)
    unsigned char _type;
    /// Collection of limits
    std::vector<LimitItem> _limitCollection;
};

///	\class ParametersAndLimitsManager
///	\brief This class covers a collection of parameters and limits and all it has functions
/// needed to put and get them
class ParametersAndLimitsManager {
public:
    /// \brief Constructor
    /// \param[in] modulId Owners module id
    ParametersAndLimitsManager(unsigned int moduleId);

    /// \brief destructor
    ~ParametersAndLimitsManager(){};

public:
    /// \brief Returns modul identifier
    /// \return Modul id
    unsigned int getIdentifier() const;

    /// \brief Get the numbers of parameters
    /// \return Number of parameters used by this manager
    unsigned long getParameterCount() const;
    /// \brief Get one of the parameters interface
    /// \param[in] index Index of parameter in collection ([0]->first,  [coll.size()-1]->last)
    /// \return Pointer to parameter interface to possibility get of IParam interface
    Parameter* getParameter(unsigned long index) const;
    /// \brief Get one of the parameters interface
    /// \param[in] id Id of parameter
    /// \return Pointer to parameter interface to possibility get of IParam interface
    Parameter* getParameterById(unsigned long id) const;
    /// Validate and apply of new value of parameter
    /// \param[in] handle Id of called module
    /// \param[in] id Specific paramater id
    /// \param[in] newValue New value of parameter
    /// \return true if parameter is validate and apply correctly, otherwise false
    bool validateAndApply(unsigned int handle, unsigned int id, const char* newValue);

    /// \brief Get the numbers of limits
    /// \return Number of limits used by this manager
    unsigned long getLimitCount() const;
    /// \brief Get one of the limits interface
    /// \param[in] index Index of limit in collection ([0]->first,  [coll.size()-1]->last)
    /// \return Pointer to limit interface to possibility get of ILimit interface
    Limit* getLimit(unsigned long index) const;
    /// \brief Get one of the limits interface
    /// \param[in] id Id of limit in collection
    /// \return Pointer to limit interface to possibility get of ILimit interface
    Limit* getLimitById(unsigned long id) const;

    /// \brief Method to put parameter to the parameter collection
    /// \param[in] newParameter Parameter container
    /// \return true if new parameter was put to the collection, otherwise return false
    bool putParameter(const ParameterContent& newParameter);
    /// \brief Method to put limit to the limit collection
    /// \param[in] newLimit New limit to the container
    void putLimits(const Limit& newLimit);
    /// \brief This method checks the cohesion between the parameters and limits
    /// \return true if cohesion of data is retained, otherwise return false
    bool validate();

    /// \brief method to register new user of parameters
    /// \return handle to parameters, 0 otherwise
    unsigned int registerNewUser();
    /// \brief method to unregister user of parameters
    /// param[in] handle Identifier of user
    /// \return true is user was unregister of parameters, false otherwise
    bool unregisterUser(unsigned int handle);

private:
    /// \brief This method checks if value is in limit range and prepares information about result
    /// \param[in] value Value to check
    /// \param[in] min - low range on parameter
    /// \param[in] max - high range of parameter
    /// \tparam VALUE_TYPE is a type of input values
    template <typename VALUE_TYPE> bool CheckRange(VALUE_TYPE min, VALUE_TYPE max, VALUE_TYPE value)
    {
        if (min > max) {
            cds_error("checkRange - MinValue is higher then MaxValue");
            return false;
        }
        if (value < min) {
            cds_error("checkRange - Default is lower than min value");
            return false;
        }
        if (value > max) {
            cds_error("checkRange - Default is higher than max value");
            return false;
        }
        return true;
    }

    /// \brief This method checks if parameter id is in collection
    /// \param[in] id Parameter id
    /// \return true if paramater has in the collection, false otherwise
    bool findId(unsigned int id);

    /// \brief This method checks if limit is correctly set and if value is in limit range
    /// \param[in] minValue Low range on parameter
    /// \param[in] maxValue High range of parameter
    /// \param[in] value Text parameter value
    /// \param[in] type Parameter type value (description by PT_xxx types)
    /// \return true if value is in range, false otherwise
    bool checkValueInRange(const QString min, const QString max, const QString value, unsigned int type);

    /// \brief Chek if value is in limit
    /// \param[in] id Id of limit in collection
    /// \param[in] value Reference to value string to check
    /// \return true if valuie is in limit or limit not exist, false otherwise
    bool checkValueInLimit(unsigned long id, const QString& value) const;

private:
    /// Module id
    unsigned int _moduleId;
    /// Users counter;
    unsigned int _counter;
    /// Container to limits collection
    mutable std::vector<Limit> _limitsCollection;
    /// Container to parameters collection
    mutable std::vector<Parameter> _parametersCollection;
    /// Container to check request limitId during validation
    std::set<unsigned int> _requiredLimitIds;
};

#endif // PALMGR_H
