#ifndef AVGTABLEMODEL_H
#define AVGTABLEMODEL_H

#include <QAbstractTableModel>

class AvgTableModel : public QAbstractTableModel
{
    Q_OBJECT
private:
    std::vector<std::vector<QString>> m_cons;
public:
    AvgTableModel(QObject *parent, std::vector<std::vector<QString>> cons);
    AvgTableModel(QObject *parent);
    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void load(const std::vector<std::vector<QString>> &cons);
};

#endif // AVGTABLEMODEL_H
