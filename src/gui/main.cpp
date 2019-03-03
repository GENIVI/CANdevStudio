#include "mainwindow.h"
#include <QFile>
#include <QtCore/QtDebug>
#include <QtCore/QtGlobal>
#include <QtWidgets/QApplication>
#include <QCanBusFrame>
#include <nodes/Connection>

#include "log.h"

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
    Q_INIT_RESOURCE(CANdevResources);
    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar);

    qRegisterMetaType<QtNodes::PortIndex>("PortIndex");
    qRegisterMetaType<QCanBusFrame>("QCanBusFrame");

    // CDS_DEBUG is set automatically in CMakeLists.txt based on build type (i.e. false for Release, true for Debug)
    setupLogger(CDS_DEBUG);

    cds_debug("Starting CANdevStudio");

    qDebug() << "Qt message ";

    MainWindow w;
    w.show();

    return a.exec();
}
