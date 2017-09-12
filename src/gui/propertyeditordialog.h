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

/**
 * Class defining Property Editor Dialog
 */
class PropertyEditorDialog : public QDialog {
    Q_OBJECT

public:
    /**
     *
     * @param propertySource Pointer to QObject whose properties will be edited.
     *        NOTE: properties are actually modified, even if dialog is closed
     *        by cancelling (emitting "rejected" signal). Please make sure to
     *        pass here pointer to a copy of object if you wish to preserve
     *        original values in case of cancelling.
     * @param parent optional pointer to parent widget
     */
    PropertyEditorDialog(QObject* propertySource, QWidget* parent = nullptr);

    virtual ~PropertyEditorDialog();

private:
    std::unique_ptr<Ui::PropertyEditorDialog> ui;
    PropertyModel myModel;
};

#endif /* SRC_GUI_PROPERTYEDITORDIALOG_H_ */
