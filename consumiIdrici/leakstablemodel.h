#ifndef TABLEMODEL_H
#define TABLEMODEL_H
#include <QAbstractTableModel>
#include <vector>
#include <consumption.h>
class leaksTableModel : public QAbstractTableModel
{
    Q_OBJECT
private:
    std::vector<consumption> m_cons;
public:
    leaksTableModel(QObject *parent, std::vector<consumption> cons);
    leaksTableModel(QObject *parent);
    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void load(const std::vector<consumption> &cons);
};

#endif // TABLEMODEL_H