#include "avgtablemodel.h"

AvgTableModel::AvgTableModel(QObject *parent, std::vector<std::vector<QString> > cons) :
    QAbstractTableModel(parent), m_cons(cons)
{
}


AvgTableModel::AvgTableModel(QObject *parent) : QAbstractTableModel(parent)
{
}

int AvgTableModel::rowCount(const QModelIndex & /*parent*/) const
{
   return m_cons.size();
}

int AvgTableModel::columnCount(const QModelIndex & /*parent*/) const
{
    return 4;
}

QVariant AvgTableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        //controllare dimensioni vector e nel caso resituire stringa vuota
        return m_cons[index.row()][index.column()];
    }
    return QVariant();
}

QVariant AvgTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section) {
            case 0: return QString("Cliente");
            case 1: return QString("Giorn.");
            case 2: return QString("Sett.");
            case 3: return QString("Mensile");
            }
        }
    }
    return QVariant();
}

void AvgTableModel::load(const std::vector<std::vector<QString>> &cons) {
    m_cons = cons;
}
