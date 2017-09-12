#ifndef SRC_GUI_PROPERTYEDITORDIALOG_H_
#define SRC_GUI_PROPERTYEDITORDIALOG_H_

#include "propertymodel.h"

#include <QBoxLayout>
#include <QDialog>
#include <QPushButton>
#include <QTableView>

#include <memory>

namespace Ui {
class PropertyEditorDialog;
}

class PropertyEditorDialog : public QDialog {
    Q_OBJECT

public:
    PropertyEditorDialog(QObject* propertySource, QWidget* parent = nullptr);
    virtual ~PropertyEditorDialog();

private:
    std::unique_ptr<Ui::PropertyEditorDialog> ui;
    PropertyModel myModel;
};

#endif /* SRC_GUI_PROPERTYEDITORDIALOG_H_ */
