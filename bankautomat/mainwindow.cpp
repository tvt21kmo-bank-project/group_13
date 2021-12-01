#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "customer.h"
#include <QThread>
#include <QTimer>

Customer *customer;
QString alert;
QTimer *timer = new QTimer();

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    for(int i=0; i < ui->numberGrid->rowCount(); i++)
        for(int j=0; j < ui->numberGrid->columnCount(); j++){
            QLayoutItem* item = ui->numberGrid->itemAtPosition(i, j);
            if(item != nullptr && item->widget()){
                QPushButton* btn = qobject_cast<QPushButton *>(item->widget());
                connect(btn, &QPushButton::clicked, this, &MainWindow::on_number_clicked);
            }
        }
    set_start_ui();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_number_clicked()
{
    timer->stop();
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    QString text = ui->textEdit->toPlainText();
    if(btn->text() != "OK" && btn->text() != "C"){
        if(vaihe == 0){
            customer->cardID.append(btn->text());
            text.append(btn->text());
        }
        else if(vaihe == 1){
            text.append('*');
            customer->pin.append(btn->text());
        }
        else if(vaihe == 5){
            customer->withdrawal_amount.append(btn->text());
        }
    }
    else if(btn->text() == "OK"){
        if(vaihe == 0){
            text = ui->textEdit->toPlainText();
            text.append("\n>Syötä PIN koodi:\n");
            vaihe = 1;
        }
        else if(vaihe == 1){
            //TODO login
            //login successful
            //check credit
            customer->credit = true;
            if(customer->credit){
                set_credit_or_debit_ui();
            }
            else{
                set_main_ui();
            }
            return;
        }
        else if(vaihe == 5){
            if(customer->withdrawal_amount.toInt() % 20 != 0){
                alert = "Summan täytyy olla 20:lla jaollinen.";
                customer->withdrawal_amount.clear();
            }
            else if(customer->withdrawal_amount.toInt() > 1000 != 0){
                alert = "Maksimi nosto on 1000.";
                customer->withdrawal_amount.clear();
            }
            else if((customer->balance.toInt() - customer->withdrawal_amount.toInt()) < 0){
                alert = "Tililläsi ei ole tarpeeksi rahaa tähän nostoon";
                customer->withdrawal_amount.clear();
            }
            else{
                //withdraw money
                //show money
                customer->withdrawal_amount.clear();
                set_main_ui();
            }

        }
    }
    else if(btn->text() == "C"){
        if(vaihe == 0){
            if(customer->cardID.size() > 0){
                customer->cardID.resize(customer->cardID.size() - 1);
                text.resize(text.size() - 1);
            }
        }
        else if(vaihe == 1){
            if(customer->pin.size() > 0){
                customer->pin.resize(customer->pin.size() - 1);
                text.resize(text.size() - 1);
            }
        }
        else if(vaihe == 5){
            if(customer->withdrawal_amount.size() > 0){
                customer->withdrawal_amount.resize(customer->withdrawal_amount.size() - 1);
                text.resize(text.size() - 1);
            }
        }
    }
    if(vaihe < 2){
        ui->textEdit->setText(text);
    }
    else if(vaihe == 5){
        if(alert == ""){
            set_muu_ui();
        }
        else{
            set_alert_ui();
            set_muu_ui();
        }
    }
}


void MainWindow::set_start_ui()
{
    customer = NULL;
    customer = new Customer();
    ui->textEdit->clear();
    ui->textEdit->setFontPointSize(16);
    ui->textEdit->append(">Syötä korttisi ID numero:\n");
    vaihe = 0;
}

void MainWindow::set_credit_or_debit_ui()
{
    ui->textEdit->setFontPointSize(6);
    ui->textEdit->setText(" ");
    QTextCursor cursor = ui->textEdit->textCursor();
    QTextBlockFormat textBlockFormat = cursor.blockFormat();
    textBlockFormat.setAlignment(Qt::AlignLeft);
    cursor.mergeBlockFormat(textBlockFormat);
    ui->textEdit->setTextCursor(cursor);
    ui->textEdit->setFontPointSize(20);
    ui->textEdit->append("DEBIT                               CREDIT");
    ui->textEdit->setFontPointSize(11);
    ui->textEdit->append("\n");
    ui->textEdit->setFontPointSize(20);
    ui->textEdit->append(" ");
    ui->textEdit->setFontPointSize(11);
    ui->textEdit->append("\n");
    ui->textEdit->setFontPointSize(20);
    ui->textEdit->append(" ");
    ui->textEdit->setFontPointSize(11);
    ui->textEdit->append("\n");
    ui->textEdit->setFontPointSize(20);
    ui->textEdit->append("SULJE");
    vaihe = 2;
    connect(timer, SIGNAL(timeout()), this, SLOT(on_timer_timeout()));
    timer->start(10000);
}

void MainWindow::set_main_ui()
{
    ui->textEdit->setFontPointSize(6);
    ui->textEdit->setText(" ");
    QTextCursor cursor = ui->textEdit->textCursor();
    QTextBlockFormat textBlockFormat = cursor.blockFormat();
    textBlockFormat.setAlignment(Qt::AlignLeft);
    cursor.mergeBlockFormat(textBlockFormat);
    ui->textEdit->setTextCursor(cursor);
    ui->textEdit->setFontPointSize(20);
    ui->textEdit->append("SALDO");
    ui->textEdit->setFontPointSize(11);
    ui->textEdit->append("\n");
    ui->textEdit->setFontPointSize(20);
    ui->textEdit->append("TAPAHTUMAT");
    ui->textEdit->setFontPointSize(11);
    ui->textEdit->append("\n");
    ui->textEdit->setFontPointSize(20);
    ui->textEdit->append("NOSTO");
    ui->textEdit->setFontPointSize(11);
    ui->textEdit->append("\n");
    ui->textEdit->setFontPointSize(20);
    ui->textEdit->append("LOPETA");
    vaihe = 3;
    connect(timer, SIGNAL(timeout()), this, SLOT(on_timer_timeout()));
    timer->start(30000);
}

void MainWindow::set_balance_ui()
{
    ui->textEdit->setFontPointSize(6);
    ui->textEdit->setText(" ");
    QTextCursor cursor = ui->textEdit->textCursor();
    QTextBlockFormat textBlockFormat = cursor.blockFormat();
    textBlockFormat.setAlignment(Qt::AlignLeft);
    cursor.mergeBlockFormat(textBlockFormat);
    ui->textEdit->setTextCursor(cursor);
    ui->textEdit->setFontPointSize(20);
    ui->textEdit->append(" ");
    ui->textEdit->setFontPointSize(11);
    ui->textEdit->append("\n");
    ui->textEdit->setFontPointSize(20);
    ui->textEdit->append(" ");
    ui->textEdit->setFontPointSize(11);
    ui->textEdit->append("\n");
    ui->textEdit->setFontPointSize(20);
    ui->textEdit->append(" ");
    ui->textEdit->setFontPointSize(11);
    ui->textEdit->append("\n");
    ui->textEdit->setFontPointSize(20);
    ui->textEdit->append("SULJE");
    vaihe = 6;
    connect(timer, SIGNAL(timeout()), this, SLOT(on_timer_timeout()));
    timer->start(10000);
}

void MainWindow::set_nosta_ui()
{
    ui->textEdit->setFontPointSize(6);
    ui->textEdit->setText(" ");
    QTextCursor cursor = ui->textEdit->textCursor();
    QTextBlockFormat textBlockFormat = cursor.blockFormat();
    textBlockFormat.setAlignment(Qt::AlignRight);
    cursor.mergeBlockFormat(textBlockFormat);
    ui->textEdit->setTextCursor(cursor);
    ui->textEdit->setFontPointSize(20);
    ui->textEdit->append("20");
    ui->textEdit->setFontPointSize(11);
    ui->textEdit->append("\n");
    ui->textEdit->setFontPointSize(20);
    ui->textEdit->append("50");
    ui->textEdit->setFontPointSize(11);
    ui->textEdit->append("\n");
    ui->textEdit->setFontPointSize(20);
    ui->textEdit->append("100");
    ui->textEdit->setFontPointSize(11);
    ui->textEdit->append("\n");
    ui->textEdit->setFontPointSize(20);
    ui->textEdit->append("SULJE                                      200");
    ui->textEdit->setFontPointSize(11);
    ui->textEdit->append("\n");
    ui->textEdit->setFontPointSize(20);
    ui->textEdit->append("MUU SUMMA");
    vaihe = 4;
}

void MainWindow::set_muu_ui()
{
    ui->textEdit->setFontPointSize(6);
    ui->textEdit->setText(" ");
    QTextCursor cursor = ui->textEdit->textCursor();
    QTextBlockFormat textBlockFormat = cursor.blockFormat();
    textBlockFormat.setAlignment(Qt::AlignLeft);
    cursor.mergeBlockFormat(textBlockFormat);
    ui->textEdit->setTextCursor(cursor);
    ui->textEdit->setFontPointSize(20);
    ui->textEdit->append("SYÖTÄ SUMMA:");
    ui->textEdit->setFontPointSize(20);
    ui->textEdit->append(customer->withdrawal_amount);
    ui->textEdit->setFontPointSize(11);
    ui->textEdit->append("\n");
    ui->textEdit->setFontPointSize(11);
    ui->textEdit->append("\n");
    ui->textEdit->setFontPointSize(20);
    ui->textEdit->append(" ");
    ui->textEdit->setFontPointSize(11);
    ui->textEdit->append("\n");
    ui->textEdit->setFontPointSize(20);
    ui->textEdit->append("SULJE");
    vaihe = 5;
}
void MainWindow::set_alert_ui()
{
    ui->textEdit->clear();
    QTextCursor cursor = ui->textEdit->textCursor();
    QTextBlockFormat textBlockFormat = cursor.blockFormat();
    textBlockFormat.setAlignment(Qt::AlignCenter);
    cursor.mergeBlockFormat(textBlockFormat);
    ui->textEdit->setTextCursor(cursor);
    ui->textEdit->setFontPointSize(11);
    ui->textEdit->append(alert);
    vaihe = 100;
    delay(5000);
    alert = "";
}
void MainWindow::on_pushNosto_clicked()
{
    timer->stop();
    if(vaihe == 3){
        set_nosta_ui();
    }
}


void MainWindow::on_pushMuu_clicked()
{
    timer->stop();
    if(vaihe == 4){
        set_muu_ui();
    }
}


void MainWindow::on_pushSaldo_clicked()
{
    timer->stop();
    if(vaihe == 2){
        customer->credit = false;
        timer->stop();
        set_main_ui();
    }
    else if(vaihe == 3){
        set_balance_ui();
    }
}


void MainWindow::on_push20_clicked()
{
    timer->stop();
    if(vaihe == 2){
        customer->credit = true;
        timer->stop();
        set_main_ui();
    }
    else if(vaihe == 4){
        if(customer->balance.toInt() < 20){
            alert = "Tililläsi ei ole tarpeeksi rahaa tähän nostoon";
            set_alert_ui();
            set_nosta_ui();
        }
        else{
            //Withdraw money
        }
    }
}

void MainWindow::on_push50_clicked()
{
    timer->stop();
    if(vaihe == 4){
        if(customer->balance.toInt() < 50){
            alert = "Tililläsi ei ole tarpeeksi rahaa tähän nostoon";
            set_alert_ui();
            set_nosta_ui();
        }
        else{
            //Withdraw money
        }
    }
}

void MainWindow::on_push100_clicked()
{
    timer->stop();
    if(vaihe == 4){
        if(customer->balance.toInt() < 100){
            alert = "Tililläsi ei ole tarpeeksi rahaa tähän nostoon";
            set_alert_ui();
            set_nosta_ui();
        }
        else{
            //Withdraw money
        }
    }
}

void MainWindow::on_push200_clicked()
{
    timer->stop();
    if(vaihe == 4){
        if(customer->balance.toInt() < 200){
            alert = "Tililläsi ei ole tarpeeksi rahaa tähän nostoon";
            set_alert_ui();
            set_nosta_ui();
        }
        else{
            //Withdraw money
        }
    }
}

void MainWindow::on_pushLopeta_clicked()
{
    timer->stop();
    if(vaihe == 2 || vaihe == 3){
        set_start_ui();
    }
    else if(vaihe == 4){
        set_main_ui();
    }
    else if(vaihe == 5){
        set_nosta_ui();
    }
    else if(vaihe == 6){
        set_main_ui();
    }
}

void MainWindow::delay( int millisecondsToWait )
{
    QTime dieTime = QTime::currentTime().addMSecs( millisecondsToWait );
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }
}

void MainWindow::on_timer_timeout()
{
    if(vaihe == 2){
        set_start_ui();
    }
    else if(vaihe == 3){
        set_start_ui();
    }
    else if(vaihe == 6){
        set_main_ui();
    }
}
