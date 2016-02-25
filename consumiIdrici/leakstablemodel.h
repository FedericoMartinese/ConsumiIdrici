#ifndef TABLEMODEL_H
#define TABLEMODEL_H
#include <QAbstractTableModel>
#include <vector>
#include <consumption.h>
class LeaksTableModel : public QAbstractTableModel
{
    Q_OBJECT
private:
    std::vector<Consumption> m_cons;
public:
    LeaksTableModel(QObject *parent, std::vector<Consumption> cons);
    LeaksTableModel(QObject *parent);
    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void load(const std::vector<Consumption> &cons);
};

#endif // TABLEMODEL_H
