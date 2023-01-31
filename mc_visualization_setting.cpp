#include "mc_visualization_setting.h"

mc_visualization_setting::mc_visualization_setting(QObject *parent)
    : QAbstractItemModel(parent)
{
}
void mc_visualization_setting::mousePressEvent(QMouseEvent* event)
{
    emit clicked();
    emit pressed();
}

void mc_visualization_setting::mouseReleaseEvent(QMouseEvent* event)
{
    emit released();
}

QVariant mc_visualization_setting::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
}

QModelIndex mc_visualization_setting::index(int row, int column, const QModelIndex &parent) const
{
    // FIXME: Implement me!
}

QModelIndex mc_visualization_setting::parent(const QModelIndex &index) const
{
    // FIXME: Implement me!
}

int mc_visualization_setting::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    // FIXME: Implement me!
}

int mc_visualization_setting::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    // FIXME: Implement me!
}

QVariant mc_visualization_setting::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // FIXME: Implement me!
    return QVariant();
}
