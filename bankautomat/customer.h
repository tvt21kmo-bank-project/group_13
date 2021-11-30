#ifndef CUSTOMER_H
#define CUSTOMER_H
#include <QString>

class Customer
{
public:
    Customer();
    QString cardID;
    QString pin;
    bool credit;
    QString account;
    QString withdrawal_amount;
};

#endif // CUSTOMER_H
