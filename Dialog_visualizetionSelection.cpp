#include "Dialog_visualizetionselection.h"

Dialog_visualizetionselection::Dialog_visualizetionselection(QObject *parent)
    : QAbstractItemModel(parent)
{
}

QVariant Dialog_visualizetionselection::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
}

QModelIndex Dialog_visualizetionselection::index(int row, int column, const QModelIndex &parent) const
{
    // FIXME: Implement me!
}

QModelIndex Dialog_visualizetionselection::parent(const QModelIndex &index) const
{
    // FIXME: Implement me!
}

int Dialog_visualizetionselection::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    // FIXME: Implement me!
}

int Dialog_visualizetionselection::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    // FIXME: Implement me!
}

QVariant Dialog_visualizetionselection::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // FIXME: Implement me!
    return QVariant();
}
