#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>
#include <QCoreApplication>

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

private:
    Ui::MainWindow *ui;
    uint8_t vaihe;
    void set_main_ui();
    void set_start_ui();
    void set_credit_or_debit_ui();
    void set_balance_ui();
    void set_nosta_ui();
    void set_muu_ui();
    void set_alert_ui();
    void delay(int millisecondsToWait);
};
#endif // MAINWINDOW_H
