#include <PropertyModel.h>
#include <iostream>
PropertyModel::PropertyModel(QObject* parent) : QAbstractTableModel(parent)
{

}

PropertyModel::~PropertyModel()
{

}

int PropertyModel::rowCount(const QModelIndex& ) const
{
    return properties.size();
}

int PropertyModel::columnCount(const QModelIndex& ) const
{
    return 2;
}

QVariant PropertyModel::data(const QModelIndex& index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (index.column() == 0)
            return properties[index.row()];
        else if (index.column() == 1)
            return propertySource->property(properties[index.row()].toStdString().c_str());
    }

    return {};
}

Qt::ItemFlags PropertyModel::flags(const QModelIndex& index) const
{
    if (index.column() == 1)
        return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;

    return QAbstractTableModel::flags(index);
}

void PropertyModel::setPropertySource(QObject* propertySource_)
{
    propertySource = propertySource_;

    if (!propertySource)
        return;

    auto prop = propertySource->property("exposedProperties");
    if (prop.isNull() || !prop.isValid())
        return;

    for (const auto& p: prop.toStringList())
        if (!p.isEmpty())
            properties.push_back(p);
}

bool PropertyModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    int size = properties.size();
    if (role == Qt::EditRole && index.row() < size)
    {
        propertySource->setProperty(properties[index.row()].toStdString().c_str(), value);
    }

    return true;
}
