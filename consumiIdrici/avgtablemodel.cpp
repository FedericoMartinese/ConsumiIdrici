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
        //restituisce il testo da mostrare in una determinata cella
        if (index.row() < m_cons.size() && index.column() < m_cons[index.row()].size())
            return m_cons[index.row()][index.column()];

        return "";
    }
    return QVariant();
}

QVariant AvgTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        //header colonne
        if (orientation == Qt::Horizontal) {
            switch (section) {
            case 0: return QString("Utente");
            case 1: return QString("Giorn.");
            case 2: return QString("Sett.");
            case 3: return QString("Mensile");
            }
        }
    }
    return QVariant();
}

void AvgTableModel::load(const std::vector<std::vector<QString>> &cons) { //riferimento costante perché potenzialmente sono molti dati
    m_cons = cons;
}
