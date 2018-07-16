#ifndef SRC_GUI_PROPERTYEDITORDIALOG_H_
#define SRC_GUI_PROPERTYEDITORDIALOG_H_

#include <QBoxLayout>
#include <QDialog>
#include <QPushButton>
#include <QStandardItemModel>
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
    PropertyEditorDialog(const QString& title, const QObject& propertySource, QWidget* parent = nullptr);
    virtual ~PropertyEditorDialog();

    std::shared_ptr<QObject> properties();

private:
    std::unique_ptr<Ui::PropertyEditorDialog> _ui;
    QStandardItemModel _model;

    void fillModel(const QObject& propsObj);
};

#endif /* SRC_GUI_PROPERTYEDITORDIALOG_H_ */
