#include "propertyeditordialog.h"
#include "ui_propertyeditordialog.h"
#include <log.h>
#include <propertyfields.h>

PropertyEditorDialog::PropertyEditorDialog(const QString& title, const QWidget& propertySource, QWidget* parent)
    : QDialog(parent)
    , _ui(std::make_unique<Ui::PropertyEditorDialog>())
{
    _ui->setupUi(this);
    setWindowTitle(title);
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint);
    _model.setHorizontalHeaderLabels({ "Property", "Value" });

    _ui->tableView->verticalHeader()->hide();
    _ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    _ui->tableView->horizontalHeader()->setStretchLastSection(true);
    _ui->tableView->setModel(&_model);

    fillModel(propertySource);
}

PropertyEditorDialog::~PropertyEditorDialog() {}

std::shared_ptr<QWidget> PropertyEditorDialog::properties()
{
    std::shared_ptr<QWidget> ret = std::make_shared<QWidget>();

    for (int i = 0; i < _model.rowCount(); ++i) {
        auto&& propName = _model.item(i, 0)->data(Qt::DisplayRole).toString().toStdString();

        auto w = static_cast<PropertyField*>(_ui->tableView->indexWidget(_model.index(i, 1)));
        auto&& propVal = w->propText();

        cds_debug("Setting property '{}' = '{}'", propName, propVal.toStdString());
        ret->setProperty(propName.c_str(), propVal);
    }

    return ret;
}

void PropertyEditorDialog::fillModel(const QWidget& propsObj)
{
    auto prop = propsObj.property("exposedProperties");

    if (prop.isNull() || !prop.isValid()) {
        cds_info("No exposed properties");
        return;
    }

    cds_info("Exposed properties size: {}", prop.toStringList().size());

    for (const auto& p : prop.toStringList()) {
        if (!p.isEmpty()) {
            QList<QStandardItem*> list;

            auto propName = new QStandardItem(p);
            propName->setFlags(Qt::NoItemFlags);
            list.append(propName);
            list.append(new QStandardItem(propsObj.property(p.toStdString().c_str()).toString()));

            _model.appendRow(list);

            cds_debug("Adding '{}' to model", p.toStdString());

            auto w = propsObj.findChild<PropertyField*>(p + "Widget");
            if (w) {
                // set value only if it was not set in lambda already
                if (w->propText().length() == 0) {
                    w->setPropText(propsObj.property(p.toStdString().c_str()).toString());
                }
                auto ndx = _model.index(_model.rowCount() - 1, 1);
                _ui->tableView->setIndexWidget(ndx, w);
            }
        }
    }
}
