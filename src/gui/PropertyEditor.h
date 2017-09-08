#ifndef SRC_GUI_PROPERTYEDITOR_H_
#define SRC_GUI_PROPERTYEDITOR_H_

#include <PropertyModel.h>
#include <QBoxLayout>
#include <QDialog>
#include <QPushButton>
#include <QTableView>

/**
 * Dialog allowing modification of object properties.
 * To enable property modification through this dialog,
 * add property name to "exposedProperties" property of
 * your QObject. "exposedProperties" property should be
 * of type QStringList.
 * Example usage:
 * \code{.cpp}
 *   MainWindow w;
 *
 *   QStringList props;
 *   props.push_back("animated");
 *   props.push_back("iconSize");
 *   w.setProperty("exposedProperties", props);
 *   PropertyEditor e(&w);
 *   e.exec();
 * \endcode
 */
class PropertyEditor : public QDialog
{
    Q_OBJECT

public:
    /**
     * Construct PropertyEditor dialog object
     * @param propertySource QObject that will have it's properties modified through this dialog.
     * @param parent Optional parent of this Widget
     */
    PropertyEditor(QObject* propertySource, QWidget* parent = nullptr);
    virtual ~PropertyEditor();

private:
    QTableView tableView;
    PropertyModel myModel;
    QBoxLayout mainLayout;
    QBoxLayout buttonLayout;
    QPushButton okButton;
    QPushButton cancelButton;
};

#endif /* SRC_GUI_PROPERTYEDITOR_H_ */
