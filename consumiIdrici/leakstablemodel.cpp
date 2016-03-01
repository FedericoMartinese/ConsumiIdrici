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
            //nella prima colonna vengono inserite le date in cui ci sono stati consumi notturni superiori alla soglia
            return m_cons[index.row()].date().toString("yyyy-MM-dd");
        } else {
            //nella seconda colonna vengono inseriti i consumi relativi a ciascuna data
            return QString::number(m_cons[index.row()].value(), 'f', 3);
        }
    }
    return QVariant();
}

QVariant LeaksTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        //header colonne
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
