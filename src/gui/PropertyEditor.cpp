#include <PropertyEditor.h>

PropertyEditor::PropertyEditor(QObject* propertySource, QWidget* parent) :
    QDialog(parent), mainLayout(QBoxLayout::Direction::TopToBottom),
    buttonLayout(QBoxLayout::Direction::LeftToRight),
    okButton(tr("OK")), cancelButton(tr("Cancel"))
{
    myModel.setPropertySource(propertySource);
    tableView.setModel( &myModel );

    buttonLayout.addWidget(&okButton);
    buttonLayout.addWidget(&cancelButton);

    mainLayout.addWidget(&tableView);
    mainLayout.addLayout(&buttonLayout);

    setLayout(&mainLayout);

    connect(&okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(&cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

PropertyEditor::~PropertyEditor()
{

}

