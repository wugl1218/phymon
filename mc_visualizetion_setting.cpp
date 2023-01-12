#include "mc_visualizetion_setting.h"

mc_visualizetion_setting::mc_visualizetion_setting(QObject *parent)
    : QAbstractItemModel(parent)
{
}

QVariant mc_visualizetion_setting::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
}

QModelIndex mc_visualizetion_setting::index(int row, int column, const QModelIndex &parent) const
{
    // FIXME: Implement me!
}

QModelIndex mc_visualizetion_setting::parent(const QModelIndex &index) const
{
    // FIXME: Implement me!
}

int mc_visualizetion_setting::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    // FIXME: Implement me!
}

int mc_visualizetion_setting::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    // FIXME: Implement me!
}

QVariant mc_visualizetion_setting::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // FIXME: Implement me!
    return QVariant();
}
