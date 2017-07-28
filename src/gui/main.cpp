#include "mainwindow.h"
#include <QtCore/QtDebug>
#include <QtCore/QtGlobal>
#include <QtWidgets/QApplication>

#include "log.hpp"
#include "CanDeviceManager.hpp"

std::shared_ptr<spdlog::logger> kDefaultLogger;
std::shared_ptr<spdlog::logger> qtDefaultLogger;

const std::string localFile(const std::string& fullPath)
{
    return std::strchr(fullPath.c_str(), '/') ? std::strrchr(fullPath.c_str(), '/') + 1 : fullPath;
}

void setupLogger(bool verbose)
{
    kDefaultLogger = spdlog::stdout_color_mt("cds");
    qtDefaultLogger = spdlog::stdout_color_mt("cds-qt");
    if (verbose) {
        kDefaultLogger->set_level(spdlog::level::debug);
        qtDefaultLogger->set_level(spdlog::level::debug);
    }

    qInstallMessageHandler([](QtMsgType type, const QMessageLogContext& context, const QString& msg) {
        QByteArray localMsg = msg.toLocal8Bit();
        switch (type) {
        case QtDebugMsg:
            qtDefaultLogger->debug(
                "[{}@{}] {}", context.file ? localFile(context.file) : "", context.line, msg.toStdString());
            break;
        case QtInfoMsg:
            qtDefaultLogger->info(
                "[{}@{}] {}", context.file ? localFile(context.file) : "", context.line, msg.toStdString());
            break;
        case QtWarningMsg:
            qtDefaultLogger->warn(
                "[{}@{}] {}", context.file ? localFile(context.file) : "", context.line, msg.toStdString());
            break;
        case QtCriticalMsg:
            qtDefaultLogger->critical(
                "[{}@{}] {}", context.file ? localFile(context.file) : "", context.line, msg.toStdString());
            break;
        case QtFatalMsg:
            qtDefaultLogger->error(
                "[{}@{}] {}", context.file ? localFile(context.file) : "", context.line, msg.toStdString());
            std::abort();
        }
    });
}

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    
    // CDS_DEBUG is set automatically in CMakeLists.txt based on build type (i.e. false for Release, true for Debug)
    setupLogger(CDS_DEBUG);

    cds_debug("Staring CANdevStudio");

    qDebug() << "Qt message ";

    CanDeviceManager m;
    MainWindow w;

    QObject::connect(&w, SIGNAL(requestAvailableDevices(QString)), &m, SLOT(fetchAvailableDevices(QString)));
    QObject::connect(&m, SIGNAL(sendAvailableDevices(QString, QList<QCanBusDeviceInfo>)), &w, SLOT(availableDevices(QString, QList<QCanBusDeviceInfo>)));
    QObject::connect(&w, SIGNAL(selectCANDevice(QString, QString)), &m, SLOT(selectCANDevice(QString, QString)));
    QObject::connect(&m, SIGNAL(attachToViews(CanDevice*)), &w, SLOT(attachToViews(CanDevice*)));

    w.show();

    return a.exec();
}
