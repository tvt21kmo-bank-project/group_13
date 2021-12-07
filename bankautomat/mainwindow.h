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

    void on_finish(QNetworkReply *reply);

private:
    Ui::MainWindow *ui;
    uint8_t vaihe;
    Customer *customer;
    QString alert;
    QTimer *timer = NULL;
    void numpad_event_create();
    void set_main_ui();
    void set_start_ui();
    void set_credit_or_debit_ui();
    void set_balance_ui(QString text);
    void set_withdraw_ui();
    void set_transactions_ui(QString text);
    void set_sum_ui();
    void set_alert_ui(int alert_time);
    void delay(int millisecondsToWait);
    void set_timer(int timeout);
    void login(QString korttiID, QString pin);
    void get_balance();
    void get_transactions(int page);
    void withdraw_money(QString sum);
};
#endif // MAINWINDOW_H
