#include "propertyeditordialog.h"
#include "ui_propertyeditordialog.h"
#include <log.h>

PropertyEditorDialog::PropertyEditorDialog(const QString& title, const QObject& propertySource, QWidget* parent)
    : QDialog(parent)
    , _ui(std::make_unique<Ui::PropertyEditorDialog>())
{
    _ui->setupUi(this);
    setWindowTitle(title);
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint);
    _model.setHorizontalHeaderLabels({ "Property", "Value" });

    fillModel(propertySource);
    _ui->tableView->verticalHeader()->hide();
    _ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    _ui->tableView->horizontalHeader()->setStretchLastSection(true);
    _ui->tableView->setModel(&_model);
}

PropertyEditorDialog::~PropertyEditorDialog() {}

std::shared_ptr<QObject> PropertyEditorDialog::properties()
{
    std::shared_ptr<QObject> ret = std::make_shared<QObject>();

    for (int i = 0; i < _model.rowCount(); ++i) {
        auto&& propName = _model.item(i, 0)->data(Qt::DisplayRole).toString().toStdString();
        auto&& propVal = _model.item(i, 1)->data(Qt::DisplayRole).toString();

        cds_debug("Setting property '{}' = '{}'", propName, propVal.toStdString());
        ret->setProperty(propName.c_str(), propVal);
    }

    return ret;
}

void PropertyEditorDialog::fillModel(const QObject& propsObj)
{
    auto prop = propsObj.property("exposedProperties");

    if (prop.isNull() || !prop.isValid())
        return;

    for (const auto& p : prop.toStringList()) {
        if (!p.isEmpty()) {
            QList<QStandardItem*> list;

            auto propName = new QStandardItem(p);
            propName->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            list.append(propName);
            list.append(new QStandardItem(propsObj.property(p.toStdString().c_str()).toString()));

            _model.appendRow(list);
        }
    }
}
