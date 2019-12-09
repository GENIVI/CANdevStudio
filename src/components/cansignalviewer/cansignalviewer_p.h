#ifndef CANSIGNALVIEWER_P_H
#define CANSIGNALVIEWER_P_H

#include "cansignalviewer.h"
#include "gui/cansignalviewerguiimpl.h"
#include <QElapsedTimer>
#include <memory>
#include <sortmodel.h>

class CanSignalViewer;

class CanSignalViewerPrivate : public QObject {
    Q_OBJECT
    Q_DECLARE_PUBLIC(CanSignalViewer)

public:
    CanSignalViewerPrivate(
        CanSignalViewer* q, CanSignalViewerCtx&& ctx = CanSignalViewerCtx(new CanSignalViewerGuiImpl));
    ComponentInterface::ComponentProperties getSupportedProperties() const;
    QJsonObject getSettings();
    void setSettings(const QJsonObject& json);
    void addSignal(const QString& name, const QVariant& val, const Direction& dir);

private:
    void initProps();

public:
    bool _simStarted{ false };
    CanSignalViewerCtx _ctx;
    CanSignalViewerGuiInt& _ui;
    bool _docked{ true };
    std::map<QString, QVariant> _props;
    QStringList _columnsOrder;
    QStandardItemModel _tvModel;
    QStandardItemModel _tvModelUnique;
    SortModel _tvModelSort;
    SortModel _tvModelUniqueSort;

private slots:
    void clear();
    void sort(const int clickedIndex);
    void setFilter(bool enabled);

private:
    int _rowID{ 0 };
    int _prevIndex{ 0 };
    int _sortIndex{ 0 };
    Qt::SortOrder _currentSortOrder{ Qt::AscendingOrder };
    std::map<QString,
        std::tuple<QStandardItem*, QStandardItem*, QStandardItem*, QStandardItem*, QStandardItem*, QStandardItem*>>
        _uniqueTxMap;
    std::map<QString,
        std::tuple<QStandardItem*, QStandardItem*, QStandardItem*, QStandardItem*, QStandardItem*, QStandardItem*>>
        _uniqueRxMap;
    QElapsedTimer _timer;

    CanSignalViewer* q_ptr;
    const QString _nameProperty = "name";

    // workaround for clang 3.5
    using cf = ComponentInterface::CustomEditFieldCbk;

    // clang-format off
    ComponentInterface::ComponentProperties _supportedProps = {
            std::make_tuple(_nameProperty, QVariant::String, true, cf(nullptr))
    };
    // clang-format on
};

#endif // CANSIGNALVIEWER_P_H
