#include <boost/filesystem.hpp>
#include <cxxopts.hpp>
#include <spdlog/fmt/fmt.h>
#include <fstream>

namespace {

std::string str_tolower(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
    return s;
}

std::string str_toupper(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::toupper(c); });
    return s;
}

std::string genCMake(const std::string name)
{
    using namespace fmt::literals;

    return fmt::format(R"(set(COMPONENT_NAME {name})

set(SRC
    {name}.cpp
    {name}_p.cpp
    {name}model.cpp
)

add_library(${{COMPONENT_NAME}} ${{SRC}})
target_link_libraries(${{COMPONENT_NAME}} Qt5::Widgets Qt5::Core Qt5::SerialBus nodes cds-common)
target_include_directories(${{COMPONENT_NAME}} INTERFACE ${{CMAKE_CURRENT_SOURCE_DIR}})

if(WITH_TESTS)
    add_executable(${{COMPONENT_NAME}}_test tests/${{COMPONENT_NAME}}_test.cpp)
    target_link_libraries(${{COMPONENT_NAME}}_test ${{COMPONENT_NAME}} Qt5::Test fakeit)
    add_test(NAME ${{COMPONENT_NAME}}_test COMMAND ${{COMPONENT_NAME}}_test)

    add_executable(${{COMPONENT_NAME}}model_test tests/${{COMPONENT_NAME}}model_test.cpp)
    target_link_libraries(${{COMPONENT_NAME}}model_test ${{COMPONENT_NAME}} Qt5::Test fakeit)
    add_test(NAME ${{COMPONENT_NAME}}model_test COMMAND ${{COMPONENT_NAME}}model_test)
endif()
)",
        "name"_a = name);
}

std::string genComponentHdr(const std::string& name)
{
    using namespace fmt::literals;

    return fmt::format(R"(#ifndef {nameUpper}_H
#define {nameUpper}_H

#include <QWidget>
#include <QtCore/QScopedPointer>
#include <componentinterface.h>
#include <context.h>
#include <memory>

class {name}Private;
class QWidget;
typedef Context<> {name}Ctx;

class {name} : public QObject, public ComponentInterface {{
    Q_OBJECT
    Q_DECLARE_PRIVATE({name})

public:
    {name}();
    explicit {name}({name}Ctx&& ctx);
    ~{name}();

    QWidget* mainWidget() override;
    void setConfig(const QJsonObject& json) override;
    void setConfig(const QWidget& qobject) override;
    QJsonObject getConfig() const override;
    std::shared_ptr<QWidget> getQConfig() const override;
    void configChanged() override;
    bool mainWidgetDocked() const override;
    ComponentInterface::ComponentProperties getSupportedProperties() const override;

signals:
    void mainWidgetDockToggled(QWidget* widget) override;
    void simBcastSnd(const QJsonObject &msg, const QVariant &param = QVariant()) override;

public slots:
    void stopSimulation() override;
    void startSimulation() override;
    void simBcastRcv(const QJsonObject &msg, const QVariant &param) override;

private:
    QScopedPointer<{name}Private> d_ptr;
}};

#endif //{nameUpper}_H
)",
        "name"_a = name, "nameUpper"_a = str_toupper(name));
}

std::string genComponentSrc(const std::string& name)
{
    using namespace fmt::literals;

    return fmt::format(R"(#include "{nameLower}.h"
#include "{nameLower}_p.h"
#include <confighelpers.h>
#include <log.h>

{name}::{name}()
    : d_ptr(new {name}Private(this))
{{
}}

{name}::{name}({name}Ctx&& ctx)
    : d_ptr(new {name}Private(this, std::move(ctx)))
{{
}}

{name}::~{name}()
{{
}}

QWidget* {name}::mainWidget()
{{
    // Component does not have main widget
    return nullptr;
}}

void {name}::setConfig(const QJsonObject& json)
{{
    Q_D({name});

    d->setSettings(json);
}}

void {name}::setConfig(const QWidget& qobject)
{{
    Q_D({name});

    configHelpers::setQConfig(qobject, getSupportedProperties(), d->_props);
}}

QJsonObject {name}::getConfig() const
{{
    return d_ptr->getSettings();
}}

std::shared_ptr<QWidget> {name}::getQConfig() const
{{
    const Q_D({name});

    return configHelpers::getQConfig(getSupportedProperties(), d->_props);
}}

void {name}::configChanged()
{{
}}

bool {name}::mainWidgetDocked() const
{{
    // Widget does not exist. Return always true
    return true;
}}

ComponentInterface::ComponentProperties {name}::getSupportedProperties() const
{{
    return d_ptr->getSupportedProperties();
}}

void {name}::stopSimulation()
{{
    Q_D({name});

    d->_simStarted = false;
}}

void {name}::startSimulation()
{{
    Q_D({name});

    d->_simStarted = true;
}}

void {name}::simBcastRcv(const QJsonObject &msg, const QVariant &param)
{{
    Q_UNUSED(msg);
    Q_UNUSED(param);
}}
)",
        "name"_a = name, "nameLower"_a = str_tolower(name));
}

std::string genPrivateHdr(const std::string& name)
{
    using namespace fmt::literals;

    return fmt::format(R"(#ifndef {nameUpper}_P_H
#define {nameUpper}_P_H

#include "{nameLower}.h"
#include <memory>

class {name};

class {name}Private : public QObject {{
    Q_OBJECT
    Q_DECLARE_PUBLIC({name})

public:
    {name}Private({name}* q, {name}Ctx&& ctx = {name}Ctx());
    ComponentInterface::ComponentProperties getSupportedProperties() const;
    QJsonObject getSettings();
    void setSettings(const QJsonObject& json);

private:
    void initProps();

public:
    bool _simStarted{{ false }};
    {name}Ctx _ctx;
    std::map<QString, QVariant> _props;

private:
    {name}* q_ptr;
    const QString _nameProperty = "name";

    // workaround for clang 3.5
    using cf = ComponentInterface::CustomEditFieldCbk;

    // clang-format off
    ComponentInterface::ComponentProperties _supportedProps = {{
            std::make_tuple(_nameProperty, QVariant::String, true, cf(nullptr))
    }};
    // clang-format on
}};

#endif // {nameUpper}_P_H
)",
        "name"_a = name, "nameUpper"_a = str_toupper(name), "nameLower"_a = str_tolower(name));
}

std::string genPrivateSrc(const std::string& name)
{
    using namespace fmt::literals;

    return fmt::format(R"(#include "{nameLower}_p.h"
#include <log.h>

{name}Private::{name}Private({name} *q, {name}Ctx&& ctx)
    : _ctx(std::move(ctx))
    , q_ptr(q)
{{
    initProps();
}}

void {name}Private::initProps()
{{
    for (const auto& p: _supportedProps) {{
        _props[ComponentInterface::propertyName(p)];
    }}
}}

ComponentInterface::ComponentProperties {name}Private::getSupportedProperties() const
{{
    return _supportedProps;
}}

QJsonObject {name}Private::getSettings()
{{
    QJsonObject json;

    for (const auto& p : _props) {{
        json[p.first] = QJsonValue::fromVariant(p.second);
    }}

    return json;
}}

void {name}Private::setSettings(const QJsonObject& json)
{{
    for (const auto& p : _supportedProps) {{
        const QString& propName = ComponentInterface::propertyName(p);
        if (json.contains(propName))
            _props[propName] = json[propName].toVariant();
    }}
}}
)",
        "name"_a = name, "nameUpper"_a = str_toupper(name), "nameLower"_a = str_tolower(name));
}

std::string genGuiCMake(const std::string name)
{
    using namespace fmt::literals;

    return fmt::format(R"(set(COMPONENT_NAME {name})

set(SRC
    gui/{name}.ui
    gui/{name}guiimpl.h
    {name}.cpp
    {name}_p.cpp
    {name}model.cpp
)

add_library(${{COMPONENT_NAME}} ${{SRC}})
target_link_libraries(${{COMPONENT_NAME}} cds-common)
target_include_directories(${{COMPONENT_NAME}} INTERFACE ${{CMAKE_CURRENT_SOURCE_DIR}})

if(WITH_TESTS)
    add_executable(${{COMPONENT_NAME}}_test tests/${{COMPONENT_NAME}}_test.cpp)
    target_link_libraries(${{COMPONENT_NAME}}_test ${{COMPONENT_NAME}} Qt5::Test fakeit)
    add_test(NAME ${{COMPONENT_NAME}}_test COMMAND ${{COMPONENT_NAME}}_test)

    add_executable(${{COMPONENT_NAME}}model_test tests/${{COMPONENT_NAME}}model_test.cpp)
    target_link_libraries(${{COMPONENT_NAME}}model_test ${{COMPONENT_NAME}} Qt5::Test fakeit)
    add_test(NAME ${{COMPONENT_NAME}}model_test COMMAND ${{COMPONENT_NAME}}model_test)
endif()
)",
        "name"_a = name);
}

std::string genGuiComponentHdr(const std::string& name)
{
    using namespace fmt::literals;

    return fmt::format(R"(#ifndef {nameUpper}_H
#define {nameUpper}_H

#include <QWidget>
#include <QtCore/QScopedPointer>
#include <componentinterface.h>
#include <context.h>
#include <memory>

class {name}Private;
class QWidget;
struct {name}GuiInt;
typedef Context<{name}GuiInt> {name}Ctx;

class {name} : public QObject, public ComponentInterface {{
    Q_OBJECT
    Q_DECLARE_PRIVATE({name})

public:
    {name}();
    explicit {name}({name}Ctx&& ctx);
    ~{name}();

    QWidget* mainWidget() override;
    void setConfig(const QJsonObject& json) override;
    void setConfig(const QWidget& qobject) override;
    QJsonObject getConfig() const override;
    std::shared_ptr<QWidget> getQConfig() const override;
    void configChanged() override;
    bool mainWidgetDocked() const override;
    ComponentInterface::ComponentProperties getSupportedProperties() const override;

signals:
    void mainWidgetDockToggled(QWidget* widget) override;
    void simBcastSnd(const QJsonObject &msg, const QVariant &param = QVariant()) override;

public slots:
    void stopSimulation() override;
    void startSimulation() override;
    void simBcastRcv(const QJsonObject &msg, const QVariant &param) override;

private:
    QScopedPointer<{name}Private> d_ptr;
}};

#endif //{nameUpper}_H
)",
        "name"_a = name, "nameUpper"_a = str_toupper(name));
}

std::string genGuiComponentSrc(const std::string& name)
{
    using namespace fmt::literals;

    return fmt::format(R"(#include "{nameLower}.h"
#include "{nameLower}_p.h"
#include <confighelpers.h>
#include <log.h>

{name}::{name}()
    : d_ptr(new {name}Private(this))
{{
}}

{name}::{name}({name}Ctx&& ctx)
    : d_ptr(new {name}Private(this, std::move(ctx)))
{{
}}

{name}::~{name}()
{{
}}

QWidget* {name}::mainWidget()
{{
    Q_D({name});

    return d->_ui.mainWidget();
}}

void {name}::setConfig(const QJsonObject& json)
{{
    Q_D({name});

    d->setSettings(json);
}}

void {name}::setConfig(const QWidget& qobject)
{{
    Q_D({name});

    configHelpers::setQConfig(qobject, getSupportedProperties(), d->_props);
}}

QJsonObject {name}::getConfig() const
{{
    return d_ptr->getSettings();
}}

std::shared_ptr<QWidget> {name}::getQConfig() const
{{
    const Q_D({name});

    return configHelpers::getQConfig(getSupportedProperties(), d->_props);
}}

void {name}::configChanged()
{{
}}

bool {name}::mainWidgetDocked() const
{{
    return d_ptr->_docked;
}}

ComponentInterface::ComponentProperties {name}::getSupportedProperties() const
{{
    return d_ptr->getSupportedProperties();
}}

void {name}::stopSimulation()
{{
    Q_D({name});

    d->_simStarted = false;
}}

void {name}::startSimulation()
{{
    Q_D({name});

    d->_simStarted = true;
}}

void {name}::simBcastRcv(const QJsonObject &msg, const QVariant &param)
{{
    Q_UNUSED(msg);
    Q_UNUSED(param);
}}
)",
        "name"_a = name, "nameLower"_a = str_tolower(name));
}

std::string genGuiPrivateHdr(const std::string& name)
{
    using namespace fmt::literals;

    return fmt::format(R"(#ifndef {nameUpper}_P_H
#define {nameUpper}_P_H

#include "gui/{nameLower}guiimpl.h"
#include "{nameLower}.h"
#include <memory>

class {name};

class {name}Private : public QObject {{
    Q_OBJECT
    Q_DECLARE_PUBLIC({name})

public:
    {name}Private({name}* q, {name}Ctx&& ctx = {name}Ctx(new {name}GuiImpl));
    ComponentInterface::ComponentProperties getSupportedProperties() const;
    QJsonObject getSettings();
    void setSettings(const QJsonObject& json);

private:
    void initProps();

public:
    bool _simStarted{{ false }};
    {name}Ctx _ctx;
    {name}GuiInt& _ui;
    bool _docked{{ true }};
    std::map<QString, QVariant> _props;

private:
    {name}* q_ptr;
    const QString _nameProperty = "name";

    // workaround for clang 3.5
    using cf = ComponentInterface::CustomEditFieldCbk;

    // clang-format off
    ComponentInterface::ComponentProperties _supportedProps = {{
            std::make_tuple(_nameProperty, QVariant::String, true, cf(nullptr))
    }};
    // clang-format on
}};

#endif // {nameUpper}_P_H
)",
        "name"_a = name, "nameUpper"_a = str_toupper(name), "nameLower"_a = str_tolower(name));
}

std::string genGuiPrivateSrc(const std::string& name)
{
    using namespace fmt::literals;

    return fmt::format(R"(#include "{nameLower}_p.h"
#include <log.h>

{name}Private::{name}Private({name} *q, {name}Ctx&& ctx)
    : _ctx(std::move(ctx))
    , _ui(_ctx.get<{name}GuiInt>())
    , q_ptr(q)
{{
    initProps();
}}

void {name}Private::initProps()
{{
    for (const auto& p: _supportedProps)
    {{
        _props[ComponentInterface::propertyName(p)];
    }}
}}

ComponentInterface::ComponentProperties {name}Private::getSupportedProperties() const
{{
    return _supportedProps;
}}

QJsonObject {name}Private::getSettings()
{{
    QJsonObject json;

    for (const auto& p : _props) {{
        json[p.first] = QJsonValue::fromVariant(p.second);
    }}

    return json;
}}

void {name}Private::setSettings(const QJsonObject& json)
{{
    for (const auto& p : _supportedProps) {{
        const QString& propName = ComponentInterface::propertyName(p);
        if (json.contains(propName))
            _props[propName] = json[propName].toVariant();
    }}
}}
)",
        "name"_a = name, "nameLower"_a = str_tolower(name));
}

std::string genGuiInt(const std::string& name)
{
    using namespace fmt::literals;

    return fmt::format(R"(#ifndef {nameUpper}GUIINT_H
#define {nameUpper}GUIINT_H

#include <Qt>
#include <functional>

class QWidget;

struct {name}GuiInt {{
    virtual ~{name}GuiInt() {{}}
    virtual QWidget* mainWidget() = 0;
}};

#endif // {nameUpper}GUIINT_H
)",
        "name"_a = name, "nameUpper"_a = str_toupper(name));
}

std::string genGuiImpl(const std::string& name)
{
    using namespace fmt::literals;

    return fmt::format(R"(#ifndef {nameUpper}GUIIMPL_H
#define {nameUpper}GUIIMPL_H

#include "ui_{nameLower}.h"
#include "{nameLower}guiint.h"
#include <QWidget>

struct {name}GuiImpl : public {name}GuiInt {{
    {name}GuiImpl()
        : _ui(new Ui::{name}Private)
        , _widget(new QWidget)
    {{
        _ui->setupUi(_widget);
    }}

    virtual QWidget* mainWidget()
    {{
        return _widget;
    }}

private:
    Ui::{name}Private* _ui;
    QWidget* _widget;
}};

#endif // {nameUpper}GUIIMPL_H
)",
        "name"_a = name, "nameUpper"_a = str_toupper(name), "nameLower"_a = str_tolower(name));
}

std::string genGui(const std::string& name)
{
    using namespace fmt::literals;

    return fmt::format(R"(<ui version="4.0" >
 <author></author>
 <comment></comment>
 <exportmacro></exportmacro>
 <class>{name}Private</class>
 <widget class="QWidget" name="{name}" >
  <property name="geometry" >
   <rect>
    <x>0</x>
    <y>0</y>
    <width>400</width>
    <height>300</height>
   </rect>
  </property>
  <property name="windowTitle" >
   <string>{name}</string>
  </property>
 </widget>
 <pixmapfunction></pixmapfunction>
 <connections/>
</ui>
)",
        "name"_a = name);
}

std::string genDataModelHdr(const std::string& name)
{
    using namespace fmt::literals;

    return fmt::format(R"(#ifndef {nameUpper}MODEL_H
#define {nameUpper}MODEL_H

#include "componentmodel.h"
#include "nodepainter.h"
#include <QtCore/QObject>
#include <{nameLower}.h>

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::PortIndex;
using QtNodes::PortType;

enum class Direction;

class {name}Model : public ComponentModel<{name}, {name}Model> {{
    Q_OBJECT

public:
    {name}Model();

    unsigned int nPorts(PortType portType) const override;
    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;
    std::shared_ptr<NodeData> outData(PortIndex port) override;
    void setInData(std::shared_ptr<NodeData> nodeData, PortIndex port) override;
    QtNodes::NodePainterDelegate* painterDelegate() const override;

public slots:

signals:
    void requestRedraw();

private:
    std::unique_ptr<NodePainter> _painter;
}};

#endif // {nameUpper}MODEL_H
)",
        "name"_a = name, "nameUpper"_a = str_toupper(name), "nameLower"_a = str_tolower(name));
}

std::string genDataModelSrc(const std::string& name)
{
    using namespace fmt::literals;

    return fmt::format(R"(#include "{nameLower}model.h"
#include "{nameLower}plugin.h"
#include <log.h>

namespace {{

// clang-format off
const std::map<PortType, std::vector<NodeDataType>> portMappings = {{
    {{ PortType::In, 
        {{
            //{{CanRawData{{}}.type() }}
        }}
    }},
    {{ PortType::Out, 
        {{
            //{{CanRawData{{}}.type() }}
        }}
    }}
}};
// clang-format on

}} // namespace

{name}Model::{name}Model()
    : ComponentModel("{name}")
    , _painter(std::make_unique<NodePainter>({name}Plugin::PluginType::sectionColor()))
{{
    _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    _label->setFixedSize(75, 25);
    _label->setAttribute(Qt::WA_TranslucentBackground);
}}

QtNodes::NodePainterDelegate* {name}Model::painterDelegate() const
{{
    return _painter.get();
}}

unsigned int {name}Model::nPorts(PortType portType) const
{{
    return portMappings.at(portType).size();
}}

NodeDataType {name}Model::dataType(PortType portType, PortIndex ndx) const
{{
    if (portMappings.at(portType).size() > static_cast<uint32_t>(ndx)) {{
        return portMappings.at(portType)[ndx];
    }}

    cds_error("No port mapping for ndx: {{}}", ndx);
    return {{ }};
}}

std::shared_ptr<NodeData> {name}Model::outData(PortIndex)
{{
    // example
    // return std::make_shared<CanRawData>(_frame, _direction, _status);

    return {{ }};
}}

void {name}Model::setInData(std::shared_ptr<NodeData> nodeData, PortIndex)
{{
    // example
    // if (nodeData) {{
    //     auto d = std::dynamic_pointer_cast<CanRawData>(nodeData);
    //     assert(nullptr != d);
    //     emit sendFrame(d->frame());
    // }} else {{
    //     cds_warn("Incorrect nodeData");
    // }}
    (void) nodeData;
}}
)",
        "name"_a = name, "nameUpper"_a = str_toupper(name), "nameLower"_a = str_tolower(name));
}

std::string genTests(const std::string& name, bool gui)
{
    using namespace fmt::literals;

    return fmt::format(R"(#include <QtWidgets/QApplication>
#include <{nameLower}.h>
#define CATCH_CONFIG_RUNNER
#include "log.h"
#include <QCanBusFrame>
#include <QSignalSpy>
#include <catch.hpp>
#include <fakeit.hpp>

std::shared_ptr<spdlog::logger> kDefaultLogger;
// needed for QSignalSpy cause according to qtbug 49623 comments
// automatic detection of types is "flawed" in moc
Q_DECLARE_METATYPE(QCanBusFrame);

TEST_CASE("Stubbed methods", "[{nameLower}]")
{{
    {name} c;

    REQUIRE(c.mainWidget() {gui} nullptr);
    REQUIRE(c.mainWidgetDocked() == true);
}}

TEST_CASE("setConfig - qobj", "[{nameLower}]")
{{
    {name} c;
    QWidget obj;

    obj.setProperty("name", "Test Name");

    c.setConfig(obj);
}}

TEST_CASE("setConfig - json", "[{nameLower}]")
{{
    {name} c;
    QJsonObject obj;

    obj["name"] = "Test Name";

    c.setConfig(obj);
}}

TEST_CASE("getConfig", "[{nameLower}]")
{{
    {name} c;

    auto abc = c.getConfig();
}}

TEST_CASE("getQConfig", "[{nameLower}]")
{{
    {name} c;

    auto abc = c.getQConfig();
}}

TEST_CASE("configChanged", "[{nameLower}]")
{{
    {name} c;

    c.configChanged();
}}

TEST_CASE("getSupportedProperties", "[{nameLower}]")
{{
    {name} c;

    auto props = c.getSupportedProperties();

    REQUIRE(props.size() == 1);

    REQUIRE(ComponentInterface::propertyName(props[0]) == "name");
    REQUIRE(ComponentInterface::propertyType(props[0]) == QVariant::String);
    REQUIRE(ComponentInterface::propertyEditability(props[0]) == true);
    REQUIRE(ComponentInterface::propertyField(props[0]) == nullptr);
}}

int main(int argc, char* argv[])
{{
    bool haveDebug = std::getenv("CDS_DEBUG") != nullptr;
    kDefaultLogger = spdlog::stdout_color_mt("cds");
    if (haveDebug) {{
        kDefaultLogger->set_level(spdlog::level::debug);
    }}
    cds_debug("Starting unit tests");
    qRegisterMetaType<QCanBusFrame>(); // required by QSignalSpy
    QApplication a(argc, argv); // QApplication must exist when constructing QWidgets TODO check QTest
    return Catch::Session().run(argc, argv);
}}
)",
        "name"_a = name, "nameUpper"_a = str_toupper(name), "nameLower"_a = str_tolower(name), "gui"_a = gui? "!=" : "==");
}

std::string genTestsModel(const std::string& name)
{
    using namespace fmt::literals;

    return fmt::format(R"(#include <QtWidgets/QApplication>
#include <{nameLower}model.h>
#define CATCH_CONFIG_RUNNER
#include "log.h"
#include <QCanBusFrame>
#include <QSignalSpy>
#include <catch.hpp>
#include <fakeit.hpp>

std::shared_ptr<spdlog::logger> kDefaultLogger;
// needed for QSignalSpy cause according to qtbug 49623 comments
// automatic detection of types is "flawed" in moc
Q_DECLARE_METATYPE(QCanBusFrame);

TEST_CASE("Test basic functionality", "[{nameLower}Model]")
{{
    using namespace fakeit;
    {name}Model cm;
    REQUIRE(cm.caption() == "{name}");
    REQUIRE(cm.name() == "{name}");
    REQUIRE(cm.resizable() == false);
    REQUIRE(cm.hasSeparateThread() == false);
    REQUIRE(dynamic_cast<{name}Model*>(cm.clone().get()) != nullptr);
    REQUIRE(dynamic_cast<QLabel*>(cm.embeddedWidget()) != nullptr);
}}

TEST_CASE("painterDelegate", "[{nameLower}Model]")
{{
    {name}Model cm;
    REQUIRE(cm.painterDelegate() != nullptr);
}}

TEST_CASE("nPorts", "[{nameLower}Model]")
{{
    {name}Model cm;

    REQUIRE(cm.nPorts(QtNodes::PortType::Out) == 0);
    REQUIRE(cm.nPorts(QtNodes::PortType::In) == 0);
}}

TEST_CASE("dataType", "[{nameLower}Model]")
{{
    {name}Model cm;

    NodeDataType ndt;

    // ndt = cm.dataType(QtNodes::PortType::Out, 0);
    // REQUIRE(ndt.id == "rawframe");
    // REQUIRE(ndt.name == "RAW");

    // ndt = cm.dataType(QtNodes::PortType::Out, 1);
    // REQUIRE(ndt.id == "");
    // REQUIRE(ndt.name == "");

    // ndt = cm.dataType(QtNodes::PortType::In, 0);
    // REQUIRE(ndt.id == "");
    // REQUIRE(ndt.name == "");
}}

TEST_CASE("outData", "[{nameLower}Model]")
{{
    {name}Model cm;

    auto nd = cm.outData(0);
    REQUIRE(!nd);
}}

TEST_CASE("setInData", "[{nameLower}Model]")
{{
    {name}Model cm;

    cm.setInData({{}}, 1);
}}

int main(int argc, char* argv[])
{{
    bool haveDebug = std::getenv("CDS_DEBUG") != nullptr;
    kDefaultLogger = spdlog::stdout_color_mt("cds");
    if (haveDebug) {{
        kDefaultLogger->set_level(spdlog::level::debug);
    }}
    cds_debug("Starting unit tests");
    qRegisterMetaType<QCanBusFrame>(); // required by QSignalSpy
    QApplication a(argc, argv); // QApplication must exist when constructing QWidgets TODO check QTest
    return Catch::Session().run(argc, argv);
}}
)",
        "name"_a = name, "nameUpper"_a = str_toupper(name), "nameLower"_a = str_tolower(name));
}

std::string genPluginHdr(const std::string& name)
{
    using namespace fmt::literals;

    return fmt::format(R"(#ifndef {nameUpper}PLUGIN_H
#define {nameUpper}PLUGIN_H

#include "plugin_type.h"
#include "{nameLower}model.h"

// Note that max typestring length is limited to 128 chars. 64 causes VS2015 internal error.
using MiscPlugin = PluginBase<typestring_is("Misc Layer"), 0x555fc3, 57>;

struct {name}Plugin {{
    using Model = {name}Model;
    static constexpr const char* name = "{name}";
    using PluginType = MiscPlugin;
}};

#endif // {nameUpper}PLUGIN_H
)",
        "name"_a = name, "nameUpper"_a = str_toupper(name), "nameLower"_a = str_tolower(name));
}
void writeToFile(const boost::filesystem::path& filename, const std::string& content)
{
    std::ofstream file(filename.string());
    file << content;
    file.close();
}

} // namespace

int main(int argc, char* argv[])
{
    cxxopts::Options options(argv[0], "CANdevStudio component template generator");

    // clang-format off
    options.add_options()
    ("n, name", "Component name", cxxopts::value<std::string>(), "name")
    ("o, output", "Output directory. Shall point to src/components directory", cxxopts::value<std::string>(), "path")
    ("g, no-gui", "Component without GUI", cxxopts::value<bool>())
    ("h, help", "Show help message");
    // clang-format on

    const auto&& result = options.parse(argc, argv);

    if (result.count("h")) {
        std::cout << options.help({ "" }) << std::endl;
        return EXIT_FAILURE;
    }

    if (result.count("n") == 0 || result.count("o") == 0) {
        std::cerr << options.help({ "" }) << std::endl;
        return EXIT_FAILURE;
    }

    auto componentName = result["n"].as<std::string>();
    auto componentsPath = result["o"].as<std::string>();
    auto componentNameLower = str_tolower(componentName);

    if (!boost::filesystem::exists(boost::filesystem::path(componentsPath))) {
        std::cerr << "component dir does not exist" << std::endl;

        return EXIT_FAILURE;
    }

    boost::filesystem::path componentDir(componentsPath + "/" + str_tolower(componentName));
    boost::filesystem::path testsDir(componentDir / "tests");

    if (boost::filesystem::exists(componentDir)) {
        boost::filesystem::remove_all(componentDir);   
    }

    if (!boost::filesystem::create_directory(componentDir)) {
        std::cerr << "Failed to create output directory" << std::endl;

        return EXIT_FAILURE;
    }

    if (!boost::filesystem::create_directory(testsDir)) {
        std::cerr << "Failed to create output directory" << std::endl;

        return EXIT_FAILURE;
    }

    bool withGui = !result.count("no-gui");
    if (result.count("no-gui")) {
        writeToFile(componentDir / "CMakeLists.txt", genCMake(componentNameLower));
        writeToFile(componentDir / (componentNameLower + ".h"), genComponentHdr(componentName));
        writeToFile(componentDir / (componentNameLower + ".cpp"), genComponentSrc(componentName));
        writeToFile(componentDir / (componentNameLower + "_p.h"), genPrivateHdr(componentName));
        writeToFile(componentDir / (componentNameLower + "_p.cpp"), genPrivateSrc(componentName));
    } else {
        boost::filesystem::path componentGuiDir(componentDir / "gui");
        if (!boost::filesystem::exists(componentGuiDir)) {
            if (!boost::filesystem::create_directory(componentGuiDir)) {
                std::cerr << "Failed to create gui output directory" << std::endl;

                return EXIT_FAILURE;
            }
        }

        writeToFile(componentDir / "CMakeLists.txt", genGuiCMake(componentNameLower));
        writeToFile(componentDir / (componentNameLower + ".h"), genGuiComponentHdr(componentName));
        writeToFile(componentDir / (componentNameLower + ".cpp"), genGuiComponentSrc(componentName));
        writeToFile(componentDir / (componentNameLower + "_p.h"), genGuiPrivateHdr(componentName));
        writeToFile(componentDir / (componentNameLower + "_p.cpp"), genGuiPrivateSrc(componentName));

        writeToFile(componentGuiDir / (componentNameLower + "guiimpl.h"), genGuiImpl(componentName));
        writeToFile(componentGuiDir / (componentNameLower + "guiint.h"), genGuiInt(componentName));
        writeToFile(componentGuiDir / (componentNameLower + ".ui"), genGui(componentName));
    }

    writeToFile(componentDir / (componentNameLower + "model.h"), genDataModelHdr(componentName));
    writeToFile(componentDir / (componentNameLower + "model.cpp"), genDataModelSrc(componentName));
    writeToFile(componentDir / (componentNameLower + "plugin.h"), genPluginHdr(componentName));

    writeToFile(testsDir / (componentNameLower + "_test.cpp"), genTests(componentName, withGui));
    writeToFile(testsDir / (componentNameLower + "model_test.cpp"), genTestsModel(componentName));
    
    return EXIT_SUCCESS;
}
