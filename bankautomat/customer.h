#ifndef CUSTOMER_H
#define CUSTOMER_H
#include <QString>
#include <QList>

class Customer
{
public:
    Customer();
    QString cardID;
    QString pin;
    bool credit;
    double account;
    double balance;
    double credit_limit;
    QString withdrawal_amount;
    QString first_name;
    QString last_name;
    QList<QString> transactions;
    int page;
private:

};

#endif // CUSTOMER_H
