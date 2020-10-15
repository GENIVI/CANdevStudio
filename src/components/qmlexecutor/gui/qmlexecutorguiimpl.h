#ifndef QMLEXECUTORGUIIMPL_H
#define QMLEXECUTORGUIIMPL_H

#include <chrono>
#include <cassert>

#include <QList>
#include <QWidget>
#include <QDesktopServices>
#include <QPushButton>
#include <QUrl>
#include <QFileDialog>
#include <QDesktopServices>
#include <QMessageBox>
#include <QDateTime>
#include <QTimer>
#include <QtQml/QQmlError>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>

#include "ui_qmlexecutor.h"
#include "qmlexecutorguiint.h"
#include "canbusmodel.hpp"

/**
 * @brief QMLExecutor plugin gui implementation
 */
class QMLExecutorGuiImpl : public QMLExecutorGuiInt
{
    Q_OBJECT

public:
    /**
     * @brief explicit constructor
     * @param parent qt QObject requirement, possibly to remove
     */
    explicit QMLExecutorGuiImpl(QObject* parent)
        : QMLExecutorGuiInt(parent)
        , _ui(new Ui::QMLExecutorPrivate)
        , _widget(new QWidget)
        , _qmlUpdateCheckTimer(this)
        , _CANBusModel(nullptr)
    {
        _ui->setupUi(_widget);
        _ui->logWindow->setVisible(false);

        _ui->splitter->setCollapsible(1, true);

        constexpr int size = 1234;
        _ui->splitter->setSizes(QList<int>({size,size}));

        _ui->fileName->setReadOnly(true);

        QObject::connect(_ui->editQMLButton, SIGNAL(clicked()), this, SLOT(editQML()));
        QObject::connect(_ui->loadQMLButton, SIGNAL(clicked()), this, SLOT(browseAndOpenQML()));
        QObject::connect(&_qmlUpdateCheckTimer, SIGNAL(timeout()), this, SLOT(checkQMLModification()));
        QObject::connect(_ui->pbDockUndock, &QPushButton::clicked, this, &QMLExecutorGuiImpl::dockUndock);
        QObject::connect(_ui->showLog, &QPushButton::clicked, [this] {
                    _ui->logWindow->setVisible(!_ui->logWindow->isVisible());
                });
    }

    virtual QWidget* mainWidget() override
    {
        // Using this in constructor is too early (no CSS colors set yet)
        // Background color of QQuickWidget cannot be set via CSS
        updateUIColor();

        return _widget;
    }

    void updateUIColor() override
    {
        auto color = _widget->palette().color(_widget->backgroundRole());
        _ui->quickWidget->setClearColor(color);
    }

public slots:
    /**
     * @brief Slot to do browsing for QML file, calls qml load
     */
    void browseAndOpenQML()
    {
         QUrl url = QFileDialog::getOpenFileUrl(mainWidget(), "Open QML");

         if(!url.isEmpty() && url.isValid())
         {
             loadQML(url);
         }
    }

    /**
     * @brief loads qml into a quick widget
     * @param url url of a local file to load
     */
    virtual void loadQML(const QUrl& url) override
    {
        if(url.isValid() && !url.isEmpty())
        {
            // The easiest way to reload QML seems to be creating new QQuickWidget
            auto splitter = static_cast<QSplitter*>(_ui->quickWidget->parentWidget());
            auto splitState = splitter->saveState();

            delete _ui->quickWidget;
            _ui->quickWidget = new QQuickWidget();
            updateUIColor();
            splitter->insertWidget(0, _ui->quickWidget);
            splitter->restoreState(splitState);

            QObject::connect(_ui->quickWidget, &QQuickWidget::statusChanged, this, &QMLExecutorGuiImpl::handleStatusChange);

            _qmlUrl = url;

            log("Loading qml file: " + url.toLocalFile());

            assert(_CANBusModel != nullptr);
            
            _ui->quickWidget->rootContext()->setContextProperty("CANBusModel", _CANBusModel);
            _ui->quickWidget->rootContext()->setContextProperty("LogWindow", _ui->logWindow);

            _ui->quickWidget->setSource(url);

            _ui->fileName->setText(url.toLocalFile());

            emit QMLLoaded(_qmlUrl);

            startQMLFileModificationChecks();

            _ui->editQMLButton->setEnabled(true);
        }

    }

    /**
     * @brief remembers can bus model for qml
     * @param model can bus model
     */
    virtual void setModel(CANBusModel* model) override
    {
        _CANBusModel = model;
    }

    /**
     * @brief starts checking for qml file modification
     */
    void startQMLFileModificationChecks()
    {
        _qmlLoadTime = QDateTime::currentDateTime();
        _qmlUpdateCheckTimer.start(std::chrono::milliseconds(1000).count());
    }

    /**
     * @brief stops cheking for qml file modification
     */
    void stopFileModificationChecks()
    {
        _qmlUpdateCheckTimer.stop();
    }

    /**
     * @brief checks if qml file was modified, ask the user and reloads it if needed
     */
    void checkQMLModification()
    {
        QDateTime modifycationTime = QFileInfo(_qmlUrl.toLocalFile()).lastModified();

        if(modifycationTime > _qmlLoadTime)
        {
            stopFileModificationChecks();

            if(askForQMLReload())
            {
                loadQML(_qmlUrl);
            }

            startQMLFileModificationChecks();
        }
    }

    /**
     * @brief ask if qml reload should be done
     * @return true if yes, false if no
     */
    bool askForQMLReload()
    {
        QMessageBox::StandardButton reply;

        reply = QMessageBox::question(mainWidget(), "QML was updated", _qmlUrl.toLocalFile() + " was changed.\nDo you want to reload it?", QMessageBox::Yes|QMessageBox::No);

        if (reply == QMessageBox::Yes)
        {
            return true;
        }

        return false;
    }

    /**
     * @brief log message to log area
     * @param text text to log
     */
    void log(const QString& text)
    {
        _ui->logWindow->appendPlainText(text);
    }

    /**
     * @brief tries to launch system editor for .qml file
     */
    void editQML()
    {
        if(_qmlUrl.isValid() && !_qmlUrl.isEmpty())
        {
            bool status = QDesktopServices::openUrl(_qmlUrl);

            if(!status)
            {
                log(QString("Failed to launch external editor for file: ") + _qmlUrl.toString());
            }
            else
            {
                log(QString("System editor for QML launched!"));
            }
        }
    }


public slots:
    /**
     * @brief handle status change of qml, just log it
     * @param status qml status loading
     */
    void handleStatusChange(QQuickWidget::Status status)
    {
        switch (status)
        {
        case QQuickWidget::Status::Error:
            {
            auto errors = _ui->quickWidget->errors();

            for (auto error : errors)
            {
                QString errorString = error.toString();

                log(QString("QML status: error - ") + errorString);
            }
            }
            break;

        case QQuickWidget::Status::Null:
            log(QString("QML status: Null"));
            break;

        case QQuickWidget::Status::Loading:
            log(QString("QML status: Loading"));
            break;

        case QQuickWidget::Status::Ready:
            log(QString("QML status: Ready"));
            break;
        }
    }

private:
    //! generated ui
    Ui::QMLExecutorPrivate* _ui;

    //! main widget
    QWidget* _widget;

    //! url of qml file
    QUrl _qmlUrl;

    //! qml load time used to determine if file was modified
    QDateTime _qmlLoadTime;

    //! timer used to check for qml file modification
    QTimer _qmlUpdateCheckTimer;

    CANBusModel* _CANBusModel;
};

#endif // QMLEXECUTORGUIIMPL_H
