#include <qdatetime.h>
#include <vector>
#include <string>

struct record {
    std::string clientID;    // identificativo univoco cliente
    QDateTime date;     // data e ora della lettura
    double value;       // valore della lettura
};


std::vector<record> readFile();
