#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include <QString>

namespace Config {

enum ParameterType { PT_NULL = 0, PT_BOOL, PT_STRING, PT_INT, PT_DOUBLE, PT_MAX };
enum ModulType { MT_NULL = 0, MT_CANDEVICE, MT_CANRAWSENDER, MT_CANRAWVIEWER, MT_MAX };

/// parametre flags available values
#define PFLAG_READONLY 0x00000001
#define PFLAG_SUPPORTED_MASK 0x00000001

/// \class IOnChange
/// \brief IOnChange interface class provides methods to send information about parameter change
class IOnChange {
public:
    /// \brief This method sends information about current parameter
    /// \param[out] parameterId Id of parameter
    /// \param[out] value Current parameter value
    virtual void onChange(unsigned int parameterId, const char* value) = 0;
};

/// \class IParam
/// \brief IParam interface class provides methods to handle single parameter attributes like Id, type, name or Id of
/// limit set, if exists.
class IParam {
public:
    ///	Returns Parameter unique (through module) Id number
    ///	\return Parameter Id
    virtual unsigned int getId() const = 0;
    ///	Returns Parameter name
    ///	\return	String containing parameter name
    virtual const char* getName() const = 0;
    /// Returns parameter type
    ///	\return	parameter type unsigned byte value
    virtual unsigned char getType() const = 0;
    ///	Returns parameter Flags set
    ///	\return	parameter flags value
    virtual unsigned int getFlags() const = 0;
    /// Returns unique Id of Limits set for parameter
    ///	\return	unique Id of limits set (NULL means not used)
    virtual unsigned int getLimitId() const = 0;
    /// Returns parameter default value (from default configurations file)
    ///	\return	string containing parameter default value
    virtual const char* getDefault() const = 0;
    ///	Returns parameter current value
    /// \param[in] handle Id of called modul
    ///	\return	string containing parameter current value
    virtual const char* GetValue(unsigned int handle) const = 0;
    ///	Returns parameter maximum value, if specified
    ///	\return	string containing parameter maximum value
    virtual const char* getMaxValue() const = 0;
    ///	Returns parameter minimum value, if specified
    ///	\return	string containing parameter minimum value
    virtual const char* getMinValue() const = 0;
    /// \brief Attach observer for parameter changed
    /// \param[in] handle Id of called modul
    /// \param[in] onChange Pointer to IOnChange interface
    /// \return true if interface was added or exist in collection, false otherwise
    virtual bool attach(unsigned int handle, IOnChange* onChange) = 0;
    /// \brief Detach observer for parameter changed
    /// \param[in] handle Id of called modul
    /// \param[in] onChange Pointer to IOnChange interface
    /// \return true if interface was removed or not exist in collection, false otherwise
    virtual bool detach(unsigned int handle, IOnChange* onChange) = 0;
};

/// \class ILimit
/// \brief ILimit interface class provides methods to handle Limits set attributes like Id, type, name and number of
/// possible values
class ILimit {
public:
    /// Returns Limit unique Id number
    /// \return Limit unique (through module) Id number
    virtual unsigned int getId() const = 0;
    /// Returns Limit type
    /// \return Limit type value
    virtual unsigned char getType() const = 0;
    /// Returns number of Limit possible values
    /// \return Number of possible values
    virtual unsigned int getCount() const = 0;
    /// Returns Name corresponding with values of given index
    /// \param[in] index Index of value name to get (0 to count-1)
    /// \return String containing Name of value with given index
    virtual const char* getName(unsigned int index) const = 0;
    /// Returns value of given index
    /// \param[in] index Index of value to get (0 to count-1)
    /// \return String containing value with given index
    virtual const char* getValue(unsigned int index) const = 0;
};

/// \class IConfig2
/// \brief The class is a configuration clear interface to be used to get parameters and limits description
class IConfig2 {
public:
    /// Returns number of Parameters used by given module
    /// \param[in] handle Id of called modul
    /// \return Number of used parameters
    virtual unsigned int getParamCount(unsigned int handle) = 0;

    /// Returns IParam interface of specified Parameter
    /// \param[in] handle Id of called modul
    /// \param[in] parameterIndex Index of selected parameter
    /// \return Pointer to interface of specified parameter
    virtual IParam* getParam(unsigned int handle, unsigned int parameterIndex) = 0;

    /// Returns IParam interface of specified Parameter
    /// \param[in] handle Id of called modul
    /// \param[in] parameterId Specific paramater id
    /// \return Pointer to interface of specified parameter
    virtual IParam* getParamById(unsigned int handle, unsigned int parameterId) = 0;

    /// Validate and apply of new value of parameter
    /// \param[in] handle Id of called modul
    /// \param[in] parameterId Specific paramater id
    /// \param[in] newValue New value of parameter
    /// \return true if parameter is validate and apply correctly, otherwise false
    virtual bool validateAndApply(unsigned int handle, unsigned int parameterId, const char* newValue) = 0;

    /// Returns number of Parameter's Limits used by given module
    /// \param[in] handle Id of called modul
    /// \return Number of used parameter's limits
    virtual unsigned int getLimitCount(unsigned int handle) = 0;

    /// Returns IParam interface of specified parameter's Limit
    /// \param[in] handle Id of called modul
    /// \param[in] limitIndex Index of Parameter's Limit to get
    /// \return Pointer to interface of specified parameter's limit
    virtual ILimit* getLimit(unsigned int handle, unsigned int limitIndex) = 0;

    /// Returns IParam interface of specified parameter's Limit
    /// \param[in] handle Id of called modul
    /// \param[in] limitId Specific limit id
    /// \return Pointer to interface of specified parameter's limit
    virtual ILimit* getLimitById(unsigned int handle, unsigned int limitId) = 0;
};

/// \class IConfig
/// \brief Iconfig interface class provides method to register modul to config manager
class IConfig {
public:
    /// Register modul to config manager
    /// \param[in] moduleType Index of module type
    /// \param[out] conf Pointer to IConfig interface
    /// \return Identifier of registration modul, 0 if something was wrong
    virtual unsigned int registerModule(unsigned int moduleType, Config::IConfig2* conf) = 0;
};

}
#endif // CONFIGURATION_H
