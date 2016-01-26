#include "avgtablemodel.h"

avgTableModel::avgTableModel(QObject *parent, std::vector<std::vector<QString> > cons) :
    QAbstractTableModel(parent), m_cons(cons)
{
}


avgTableModel::avgTableModel(QObject *parent) : QAbstractTableModel(parent)
{
}

int avgTableModel::rowCount(const QModelIndex & /*parent*/) const
{
   return m_cons.size();
}

int avgTableModel::columnCount(const QModelIndex & /*parent*/) const
{
    return 4;
}

QVariant avgTableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        //controllare dimensioni vector e nel caso resituire stringa vuota
        return m_cons[index.row()][index.column()];
    }
    return QVariant();
}

QVariant avgTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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

void avgTableModel::load(const std::vector<std::vector<QString>> &cons) {
    m_cons = cons;
}
