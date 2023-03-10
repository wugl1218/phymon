#ifndef mc_visualization_SETTING_H
#define mc_visualization_SETTING_H

#include <QAbstractItemModel>
#include <QWidget>

class mc_visualization_setting : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit mc_visualization_setting(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
protected:
    void paintEvent(QPaintEvent *event) ;
    void mousePressEvent(QMouseEvent *event) ;
    void mouseReleaseEvent(QMouseEvent* event);

signals:
    void clicked();
    void pressed();
    void released();
};

#endif // mc_visualization_SETTING_H
