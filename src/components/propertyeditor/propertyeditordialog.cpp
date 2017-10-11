#include "propertyeditordialog.h"

#include "ui_propertyeditordialog.h"

PropertyEditorDialog::PropertyEditorDialog(const QString &title, QObject* propertySource, QWidget* parent)
    : QDialog(parent)
    , ui(std::make_unique<Ui::PropertyEditorDialog>())
{
    ui->setupUi(this);
    setWindowTitle(title);
    myModel.setPropertySource(propertySource);
    ui->tableView->setModel(&myModel);
}

PropertyEditorDialog::~PropertyEditorDialog()
{
}
