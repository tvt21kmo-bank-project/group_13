#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>
#include <QCoreApplication>
#include <QTimer>
#include <QByteArray>
#include <QMessageBox>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "customer.h"

#define START_PHASE 0
#define LOGIN_PHASE 1
#define CREDIT_PHASE 2
#define MAIN_PHASE 3
#define WITHDRAW_PHASE 4
#define SUM_PHASE 5
#define BALANCE_PHASE 6
#define TRANSACTION_PHASE 7
#define ALERT_PHASE 8

#define DEBIT_WITHDRAWAL 0
#define CREDIT_WITHDRAWAL 1
#define DEBIT_PURCHASE 2
#define CREDIT_PURCHASE 3
#define DEBIT_DEPOSIT 4
#define CREDIT_DEPOSIT 5

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_number_clicked();
    void on_pushBack_clicked();
    void on_pushEnter_clicked();
    void on_pushNosto_clicked();
    void on_pushMuu_clicked();
    void on_pushSaldo_clicked();
    void on_push20_clicked();
    void on_pushLopeta_clicked();
    void on_push50_clicked();
    void on_push100_clicked();
    void on_push200_clicked();
    void on_timer_timeout();
    void on_pushTapahtumat_clicked();
    void on_login(QNetworkReply *reply);
    void on_withdrawal(QNetworkReply *reply);
    void on_balance_query(QNetworkReply *reply);
    void on_transactions_query(QNetworkReply *reply);

private:
    Ui::MainWindow *ui;
    uint8_t phase;
    Customer *customer;
    QTimer *timer = NULL;
    void numpad_event_create();
    void set_main_ui();
    void set_start_ui();
    void set_credit_or_debit_ui();
    void set_balance_ui();
    void set_withdraw_ui();
    void set_transactions_ui();
    void set_sum_ui();
    void set_alert_ui(QString alert_text, int alert_time);
    void delay(int millisecondsToWait);
    void set_timer(int timeout);
    void login(QString korttiID, QString pin);
    void get_balance();
    void get_transactions(int page);
    void withdraw_money(QString sum);
};
#endif // MAINWINDOW_H
