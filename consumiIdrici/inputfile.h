#ifndef INPUTFILE_H
#define INPUTFILE_H

#include <QDateTime>
#include <set>
#include <consumptionset.h>
#include <consumption.h>

class InputFile
{
private:
    QString fileName;
    QDateTime UTCtoDayLightSavTime(QDateTime date, int UTC_offset = 1) const;

public:
    InputFile(QString fileName);
    std::map<QString, consumptionSet> read(QWidget *parent) const;
};











#endif // INPUTFILE_H
