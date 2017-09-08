#ifndef SRC_GUI_PROPERTYMODEL_H_
#define SRC_GUI_PROPERTYMODEL_H_

#include <QAbstractTableModel>

/**
 * Model class used internally by PropertyEditor Dialog
 */
class PropertyModel: public QAbstractTableModel
{
    Q_OBJECT

public:
    PropertyModel(QObject* parent = nullptr);
    virtual ~PropertyModel();

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex & index) const override;

    void setPropertySource(QObject* propertySource_);
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;

private:
    QObject* propertySource = nullptr;
    std::vector<QString> properties;
};

#endif /* SRC_GUI_PROPERTYMODEL_H_ */
