#ifndef DATA_UTILITY_H
#define DATA_UTILITY_H

#include <qdatetime.h>
#include <vector>
#include <string>

struct record {
    std::string clientID;    // identificativo univoco cliente
    QDateTime date;     // data e ora della lettura
    double value;       // valore della lettura
};


std::vector<record> readFile(QString fileName);
#endif
