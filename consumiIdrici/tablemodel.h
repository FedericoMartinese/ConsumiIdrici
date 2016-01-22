#ifndef TABLEMODEL_H
#define TABLEMODEL_H
#include <QAbstractTableModel>
#include <vector>
#include <consumption.h>
class tableModel : public QAbstractTableModel
{
    Q_OBJECT
private:
    std::vector<consumption> m_cons;
public:
    tableModel(QObject *parent, std::vector<consumption> cons);
    tableModel(QObject *parent);
    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void operator =(std::vector<consumption> cons);
};

#endif // TABLEMODEL_H
