
#include "enumiterator.h"

#define CATCH_CONFIG_RUNNER
#include <catch.hpp>
#include <fakeit.hpp>

#include <QSignalSpy>
#include <QUuid>
#include <QtWidgets/QApplication>
#include <bcastmsgs.h>
#include <candbhandler.h>
#include <cstddef> // size_t, ptrdiff_t
#include <cstdint> // uint16_t
#include <iconlabel.h>
#include <iterator> // iterator_traits, begin, end
#include <limits> // numeric_limits
#include <log.h>
#include <propertyfields.h>
#include <type_traits> // is_same
#include <utility> // swap, next, advance
#include <vector> // vector

std::shared_ptr<spdlog::logger> kDefaultLogger;

enum class Trivial { A, B, C };

enum class TrivialStart { A = 0, B, C, D, E };

enum class NonTrivial : std::uint16_t { A = 100, B = 101, C = 102 };

enum class NoPastTheEndUnsigned : std::size_t {
    A = std::numeric_limits<std::size_t>::min(),
    B = std::numeric_limits<std::size_t>::max()
};

enum class NoPastTheEndSigned : std::ptrdiff_t {
    A = std::numeric_limits<std::ptrdiff_t>::min(),
    B = std::numeric_limits<std::ptrdiff_t>::max()
};

TEST_CASE("EnumIterator singularity", "[common]")
{
    using namespace fakeit;

    using I = EnumIterator<Trivial, Trivial::A, Trivial::C>;

    I it;
    REQUIRE(it.end() == it);
    REQUIRE(it.begin() != it);
}

TEST_CASE("EnumIterator non-singular", "[common]")
{
    using namespace fakeit;

    using I = EnumIterator<Trivial, Trivial::A, Trivial::C>;

    I it{ Trivial::B };
    REQUIRE(it.end() != it);
    REQUIRE(it.begin() != it);
    REQUIRE(I{ Trivial::C } == ++it);
}

TEST_CASE("EnumIterator operator*", "[common]")
{
    using namespace fakeit;

    using I = EnumIterator<Trivial, Trivial::A, Trivial::C>;

    I it{ Trivial::A };
    REQUIRE(it.end() != it);

    REQUIRE(I{ Trivial::B } == ++it);
    REQUIRE(Trivial::B == *it);
    REQUIRE(I{ Trivial::C } == ++it);
    REQUIRE(Trivial::C == *it);

    const I it1{ Trivial::A };
    REQUIRE(it1.end() != it1);
    REQUIRE(Trivial::A == *it1);
    REQUIRE(it.begin() == it1);
}

TEST_CASE("EnumIterator operator->", "[common]")
{
    using namespace fakeit;

    using I = EnumIterator<Trivial, Trivial::A, Trivial::C>;

    I it{ Trivial::A };
    REQUIRE(it.end() != it);

    REQUIRE(I{ Trivial::B } == ++it);
    REQUIRE(Trivial::B == it.operator->());
    REQUIRE(I{ Trivial::C } == ++it);
    REQUIRE(Trivial::C == it.operator->());

    const I it1{ Trivial::A };
    REQUIRE(it1.end() != it1);
    REQUIRE(Trivial::A == it1.operator->());
    REQUIRE(it.begin() == it1);
}

TEST_CASE("EnumIterator operator++", "[common]")
{
    using namespace fakeit;

    using I = EnumIterator<Trivial, Trivial::A, Trivial::C>;

    I it1{ Trivial::A };
    REQUIRE(it1.end() != it1);
    REQUIRE(Trivial::B == *++it1);
    REQUIRE(Trivial::C == *++it1);
    REQUIRE(it1.end() == ++it1);
    REQUIRE(it1.end() == ++it1);

    I it2{ Trivial::A };
    REQUIRE(it2.end() != it2);
    REQUIRE(Trivial::B == *it2++);
    REQUIRE(Trivial::A == *it2);

    const I it3{ Trivial::A };
    REQUIRE(it3.end() != it3);
    REQUIRE(it3.begin() == it3);

    I it4;
    REQUIRE(it4.end() == it4);
    REQUIRE(it4.end() == ++it4);
}

TEST_CASE("EnumIterator swap", "[common]")
{
    using namespace fakeit;

    using I = EnumIterator<Trivial, Trivial::A, Trivial::C>;

    I it1{ Trivial::B };
    I it2{ Trivial::C };
    REQUIRE(Trivial::B == *it1);
    REQUIRE(Trivial::C == *it2);

    using std::swap;
    swap(it1, it2);

    REQUIRE(Trivial::C == *it1);
    REQUIRE(Trivial::B == *it2);
}

TEST_CASE("EnumIterator non-trivial", "[common]")
{
    using namespace fakeit;

    using I = EnumIterator<NonTrivial, NonTrivial::A, NonTrivial::C>;

    I it{ NonTrivial::A };
    REQUIRE(it.end() != it);
    REQUIRE(it.begin() == it);

    REQUIRE(NonTrivial::A == *it);
    REQUIRE(NonTrivial::B == *++it);
    REQUIRE(NonTrivial::C == *++it);
    REQUIRE(it.end() == ++it);
}

TEST_CASE("EnumIterator iterator_traits", "[common]")
{
    using T = std::iterator_traits<EnumIterator<NonTrivial, NonTrivial::A, NonTrivial::C>>;

    (void)std::is_same<void, T::difference_type>::value;
    (void)std::is_same<void, T::value_type>::value;
    (void)std::is_same<void, T::pointer>::value;
    (void)std::is_same<void, T::reference>::value;
    (void)std::is_same<void, T::iterator_category>::value;
}

TEST_CASE("EnumIterator next", "[common]")
{
    using namespace fakeit;

    using I = EnumIterator<NonTrivial, NonTrivial::A, NonTrivial::C>;

    I it{ NonTrivial::A };
    REQUIRE(I{ NonTrivial::B } == std::next(it));
    REQUIRE(NonTrivial::A == *it);

    std::advance(it, 2);
    REQUIRE(NonTrivial::C == *it);

    std::advance(it, 666);
    REQUIRE(it.end() == it);
}

TEST_CASE("EnumIterator range-based-for non-trivial", "[common]")
{
    using namespace fakeit;

    using I = EnumIterator<NonTrivial, NonTrivial::A, NonTrivial::C>;

    const std::vector<NonTrivial> a = { NonTrivial::A, NonTrivial::B, NonTrivial::C };
    std::vector<NonTrivial> b;

    for (auto&& i : I{ NonTrivial::A }) {
        b.emplace_back(i);
    }

    REQUIRE(a == b);
}

TEST_CASE("EnumIterator range-based-for trivial", "[common]")
{
    using namespace fakeit;

    using I = EnumIterator<Trivial, Trivial::A, Trivial::C>;

    const std::vector<Trivial> a = { Trivial::A, Trivial::B, Trivial::C };
    std::vector<Trivial> b;

    for (auto&& i : I{ Trivial::A }) {
        b.emplace_back(i);
    }

    REQUIRE(a == b);
}

TEST_CASE("EnumIterator range-based-for trivial-start", "[common]")
{
    using namespace fakeit;

    using I = EnumIterator<TrivialStart, TrivialStart::A, TrivialStart::E>;

    const std::vector<TrivialStart> a
        = { TrivialStart::A, TrivialStart::B, TrivialStart::C, TrivialStart::D, TrivialStart::E };
    std::vector<TrivialStart> b;

    for (auto&& i : I{ TrivialStart::A }) {
        b.emplace_back(i);
    }

    REQUIRE(a == b);
}

TEST_CASE("IconLable sizeHint", "[common]")
{
    IconLabel l("name", QColor(0, 0, 0), QColor(0, 0, 0));

    REQUIRE(l.minimumSizeHint() == QSize(140, 48));
}

TEST_CASE("CanDbHandler - CanDbUpdated", "[common]")
{
    std::map<QString, QVariant> props;
    const QString dbProp = "CAN database";
    CanDbHandler db{ props, dbProp };
    QSignalSpy redrawSpy(&db, &CanDbHandler::requestRedraw);
    QSignalSpy dbSpy(&db, &CanDbHandler::dbChanged);

    CANmessages_t msg1{ { 1, {} } };
    CANmessages_t msg2{ { 1, {} }, { 2, {} } };

    const QString idStr1 = "{00000000-0000-0000-0000-000000000001}";
    const QString idStr2 = "{00000000-0000-0000-0000-000000000002}";

    QJsonObject msg;
    msg["id"] = idStr1;
    msg["msg"] = BcastMsg::CanDbUpdated;
    msg["caption"] = "name1";
    msg["color"] = "color1";

    REQUIRE(db.getDb().size() == 0);
    REQUIRE(db.getName() == "");

    // first call empty _currentDb
    db.processBcast(msg, {});

    REQUIRE(redrawSpy.count() == 1);
    REQUIRE(dbSpy.count() == 1);
    REQUIRE(db.getDb().size() == 0);
    REQUIRE(db.getName() == "name1");
    REQUIRE(props["color"].toString() == "color1");

    QVariant param;
    param.setValue(msg1);
    msg["caption"] = "name2";
    msg["color"] = "color2";

    // second call the same _currentDb
    db.processBcast(msg, param);

    REQUIRE(redrawSpy.count() == 2);
    REQUIRE(dbSpy.count() == 2);
    REQUIRE(db.getDb().size() == 1);
    REQUIRE(db.getName() == "name2");
    REQUIRE(props["color"].toString() == "color2");

    param.setValue(msg2);
    msg["id"] = idStr2;
    msg["caption"] = "name3";
    msg["color"] = "color3";

    // third call different than _currentDb
    db.processBcast(msg, param);

    REQUIRE(redrawSpy.count() == 2);
    REQUIRE(dbSpy.count() == 2);
    REQUIRE(db.getDb().size() == 1);
    REQUIRE(db.getName() == "name2");
    REQUIRE(props["color"].toString() == "color2");
}

TEST_CASE("CanDbHandler - updateCurrentDbFromProps", "[common]")
{
    std::map<QString, QVariant> props;
    const QString dbProp = "CAN database";
    CanDbHandler db{ props, dbProp };

    CANmessages_t msg1tmp{ { 1, {} } };
    QVariant msg1;
    msg1.setValue(msg1tmp);

    CANmessages_t msg2tmp{ { 1, {} }, { 2, {} } };
    QVariant msg2;
    msg2.setValue(msg2tmp);

    CANmessages_t msg3tmp{ { 1, {} }, { 2, {} }, { 3, {} } };
    QVariant msg3;
    msg3.setValue(msg3tmp);

    const QString idStr1 = "{00000000-0000-0000-0000-000000000001}";
    const QString idStr2 = "{00000000-0000-0000-0000-000000000002}";
    const QString idStr3 = "{00000000-0000-0000-0000-000000000003}";

    QJsonObject msg;
    msg["id"] = idStr1;
    msg["msg"] = BcastMsg::CanDbUpdated;
    msg["caption"] = "name1";
    msg["color"] = "color1";

    db.processBcast(msg, msg1);

    msg["id"] = idStr2;
    msg["caption"] = "name2";
    msg["color"] = "color2";
    db.processBcast(msg, msg2);

    msg["id"] = idStr3;
    msg["caption"] = "name3";
    msg["color"] = "color3";
    db.processBcast(msg, msg3);

    REQUIRE(db.getDb().size() == 1);
    REQUIRE(db.getName() == "name1");
    REQUIRE(props["color"].toString() == "color1");

    // change Db
    props[dbProp] = idStr2;
    db.updateCurrentDbFromProps();

    REQUIRE(db.getDb().size() == 2);
    REQUIRE(db.getName() == "name2");
    REQUIRE(props["color"].toString() == "color2");

    // change Db
    props[dbProp] = idStr3;
    db.updateCurrentDbFromProps();

    REQUIRE(db.getDb().size() == 3);
    REQUIRE(db.getName() == "name3");
    REQUIRE(props["color"].toString() == "color3");
}

TEST_CASE("CanDbHandler - InitDone", "[common]")
{
    std::map<QString, QVariant> props;
    const QString dbProp = "CAN database";
    CanDbHandler db{ props, dbProp };
    QSignalSpy dbSpy(&db, &CanDbHandler::sendCanDbRequest);

    const QString idStr1 = "{00000000-0000-0000-0000-000000000001}";

    QJsonObject msg;
    msg["id"] = idStr1;
    msg["msg"] = BcastMsg::InitDone;

    db.processBcast(msg, {});

    REQUIRE(dbSpy.count() == 1);
}

TEST_CASE("CanDbHandler - NodeDeleted", "[common]")
{
    std::map<QString, QVariant> props;
    const QString dbProp = "CAN database";
    CanDbHandler db{ props, dbProp };

    CANmessages_t msg1tmp{ { 1, {} } };
    QVariant msg1;
    msg1.setValue(msg1tmp);

    CANmessages_t msg2tmp{ { 1, {} }, { 2, {} } };
    QVariant msg2;
    msg2.setValue(msg2tmp);

    CANmessages_t msg3tmp{ { 1, {} }, { 2, {} }, { 3, {} } };
    QVariant msg3;
    msg3.setValue(msg3tmp);

    const QString idStr1 = "{00000000-0000-0000-0000-000000000001}";
    const QString idStr2 = "{00000000-0000-0000-0000-000000000002}";
    const QString idStr3 = "{00000000-0000-0000-0000-000000000003}";

    QJsonObject msg;
    msg["id"] = idStr1;
    msg["msg"] = BcastMsg::CanDbUpdated;
    msg["caption"] = "name1";
    msg["color"] = "color1";

    db.processBcast(msg, msg1);

    msg["id"] = idStr2;
    msg["caption"] = "name2";
    msg["color"] = "color2";
    db.processBcast(msg, msg2);

    msg["id"] = idStr3;
    msg["caption"] = "name3";
    msg["color"] = "color3";
    db.processBcast(msg, msg3);

    QSignalSpy redrawSpy(&db, &CanDbHandler::requestRedraw);
    QSignalSpy dbSpy(&db, &CanDbHandler::dbChanged);

    msg["id"] = idStr3;
    msg["msg"] = BcastMsg::NodeDeleted;
    db.processBcast(msg, {});

    REQUIRE(redrawSpy.count() == 0);
    REQUIRE(dbSpy.count() == 0);
    REQUIRE(db.getDb().size() == 1);
    REQUIRE(db.getName() == "name1");
    REQUIRE(props["color"].toString() == "color1");

    msg["id"] = idStr1;
    msg["msg"] = BcastMsg::NodeDeleted;
    db.processBcast(msg, {});

    REQUIRE(redrawSpy.count() == 1);
    REQUIRE(dbSpy.count() == 1);
    REQUIRE(db.getDb().size() == 2);
    REQUIRE(db.getName() == "name2");
    REQUIRE(props["color"].toString() == "color2");

    msg["id"] = idStr2;
    msg["msg"] = BcastMsg::NodeDeleted;
    db.processBcast(msg, {});

    REQUIRE(redrawSpy.count() == 2);
    REQUIRE(dbSpy.count() == 2);
    REQUIRE(db.getDb().size() == 0);
    REQUIRE(db.getName() == "");
    REQUIRE(props["color"].toString() == "");
}

TEST_CASE("CanDbHandler - ConfigChanged", "[common]")
{
    std::map<QString, QVariant> props;
    const QString dbProp = "CAN database";
    CanDbHandler db{ props, dbProp };

    CANmessages_t msg1tmp{ { 1, {} } };
    QVariant msg1;
    msg1.setValue(msg1tmp);

    CANmessages_t msg2tmp{ { 1, {} }, { 2, {} } };
    QVariant msg2;
    msg2.setValue(msg2tmp);

    CANmessages_t msg3tmp{ { 1, {} }, { 2, {} }, { 3, {} } };
    QVariant msg3;
    msg3.setValue(msg3tmp);

    const QString idStr1 = "{00000000-0000-0000-0000-000000000001}";
    const QString idStr2 = "{00000000-0000-0000-0000-000000000002}";
    const QString idStr3 = "{00000000-0000-0000-0000-000000000003}";

    QJsonObject msg;
    msg["id"] = idStr1;
    msg["msg"] = BcastMsg::CanDbUpdated;
    msg["caption"] = "name1";
    msg["color"] = "color1";

    db.processBcast(msg, msg1);

    msg["id"] = idStr2;
    msg["caption"] = "name2";
    msg["color"] = "color2";
    db.processBcast(msg, msg2);

    msg["id"] = idStr3;
    msg["caption"] = "name3";
    msg["color"] = "color3";
    db.processBcast(msg, msg3);

    REQUIRE(db.getName() == "name1");
    REQUIRE(props["color"].toString() == "color1");

    QJsonObject config;

    msg["id"] = idStr1;
    msg["msg"] = BcastMsg::ConfigChanged;
    msg["caption"] = "name1_new";
    config["color"] = "color1_new";
    msg["config"] = config;
    msg["name"] = "WrongType";
    db.processBcast(msg, {});

    REQUIRE(db.getName() == "name1");
    REQUIRE(props["color"].toString() == "color1");

    msg["id"] = idStr1;
    msg["msg"] = BcastMsg::ConfigChanged;
    msg["caption"] = "name1_new";
    config["color"] = "color1_new";
    msg["config"] = config;
    msg["name"] = "CanSignalData";
    db.processBcast(msg, {});

    REQUIRE(db.getName() == "name1_new");
    REQUIRE(props["color"].toString() == "color1_new");

    msg["id"] = idStr2;
    msg["msg"] = BcastMsg::ConfigChanged;
    msg["caption"] = "name2_new";
    config["color"] = "color2_new";
    msg["config"] = config;
    msg["name"] = "CanSignalData";
    db.processBcast(msg, {});

    REQUIRE(db.getName() == "name1_new");
    REQUIRE(props["color"].toString() == "color1_new");

    // change Db
    props[dbProp] = idStr2;
    db.updateCurrentDbFromProps();

    REQUIRE(db.getName() == "name2_new");
    REQUIRE(props["color"].toString() == "color2_new");
}

TEST_CASE("CanDbHandler - createPropertyWidget", "[common]")
{
    std::map<QString, QVariant> props;
    const QString dbProp = "CAN database";
    CanDbHandler db{ props, dbProp };

    CANmessages_t msg1tmp{ { 1, {} } };
    QVariant msg1;
    msg1.setValue(msg1tmp);

    CANmessages_t msg2tmp{ { 1, {} }, { 2, {} } };
    QVariant msg2;
    msg2.setValue(msg2tmp);

    CANmessages_t msg3tmp{ { 1, {} }, { 2, {} }, { 3, {} } };
    QVariant msg3;
    msg3.setValue(msg3tmp);

    const QString idStr1 = "{00000000-0000-0000-0000-000000000001}";
    const QString idStr2 = "{00000000-0000-0000-0000-000000000002}";
    const QString idStr3 = "{00000000-0000-0000-0000-000000000003}";

    QJsonObject msg;
    msg["id"] = idStr1;
    msg["msg"] = BcastMsg::CanDbUpdated;
    msg["caption"] = "name1";
    msg["color"] = "color1";

    db.processBcast(msg, msg1);

    msg["id"] = idStr2;
    msg["caption"] = "name2";
    msg["color"] = "color2";
    db.processBcast(msg, msg2);

    msg["id"] = idStr3;
    msg["caption"] = "name3";
    msg["color"] = "color3";
    db.processBcast(msg, msg3);

    auto w = static_cast<PropertyFieldCombo*>(db.createPropertyWidget());
    REQUIRE(w->propText() == idStr1);
    delete w;

    // change Db
    props[dbProp] = idStr2;
    db.updateCurrentDbFromProps();

    auto w2 = static_cast<PropertyFieldCombo*>(db.createPropertyWidget());
    REQUIRE(w2->propText() == idStr2);

    // change Db
    props[dbProp] = idStr3;
    db.updateCurrentDbFromProps();

    w = static_cast<PropertyFieldCombo*>(db.createPropertyWidget());
    REQUIRE(w->propText() == idStr3);
}

int main(int argc, char* argv[])
{
    bool haveDebug = std::getenv("CDS_DEBUG") != nullptr;
    kDefaultLogger = spdlog::stdout_color_mt("cds");
    if (haveDebug) {
        kDefaultLogger->set_level(spdlog::level::debug);
    }
    cds_debug("Starting unit tests");
    QApplication a(argc, argv); // QApplication must exist when constructing QWidgets TODO check QTest
    return Catch::Session().run(argc, argv);
}
