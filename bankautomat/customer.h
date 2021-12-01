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
    QString balance;
    QString withdrawal_amount;
    QString first_name;
    QString last_name;
};

#endif // CUSTOMER_H
