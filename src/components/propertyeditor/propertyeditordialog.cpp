#include "propertyeditordialog.h"

#include "ui_propertyeditordialog.h"

PropertyEditorDialog::PropertyEditorDialog(QObject* propertySource, QWidget* parent)
    : QDialog(parent)
    , ui(std::make_unique<Ui::PropertyEditorDialog>())
{
    ui->setupUi(this);
    myModel.setPropertySource(propertySource);
    ui->tableView->setModel(&myModel);
}

PropertyEditorDialog::~PropertyEditorDialog()
{
}
