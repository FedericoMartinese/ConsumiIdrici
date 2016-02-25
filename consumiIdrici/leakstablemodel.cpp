#include "leakstablemodel.h"

LeaksTableModel::LeaksTableModel(QObject *parent, std::vector<Consumption> cons)
    :QAbstractTableModel(parent), m_cons(cons)
{
}

LeaksTableModel::LeaksTableModel(QObject *parent)
    :QAbstractTableModel(parent)
{
}

int LeaksTableModel::rowCount(const QModelIndex & /*parent*/) const
{
   return m_cons.size();
}

int LeaksTableModel::columnCount(const QModelIndex & /*parent*/) const
{
    return 2;
}

QVariant LeaksTableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (index.column() == 0) {
            return m_cons[index.row()].date().toString("yyyy-MM-dd");
        } else {
            return QString::number(m_cons[index.row()].value());
        }
    }
    return QVariant();
}

QVariant LeaksTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            if (section == 0)
                return QString("Data");
            else
                return QString("Consumo notturno");
        }
    }
    return QVariant();
}

void LeaksTableModel::load(const std::vector<Consumption> &cons) { //riferimento costante perché potenzialmente sono molti dati
    m_cons = cons;
}
