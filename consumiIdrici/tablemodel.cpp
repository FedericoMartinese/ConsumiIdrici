#include "tablemodel.h"

tableModel::tableModel(QObject *parent, std::vector<consumption> cons)
    :QAbstractTableModel(parent), m_cons(cons)
{
}

tableModel::tableModel(QObject *parent)
    :QAbstractTableModel(parent)
{
}

int tableModel::rowCount(const QModelIndex & /*parent*/) const
{
   return m_cons.size();
}

int tableModel::columnCount(const QModelIndex & /*parent*/) const
{
    return 2;
}

QVariant tableModel::data(const QModelIndex &index, int role) const
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

QVariant tableModel::headerData(int section, Qt::Orientation orientation, int role) const
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

void tableModel::load(const std::vector<consumption>& cons) { //riferimento costante perché potenzialmente sono molti dati
    m_cons = cons;
}
