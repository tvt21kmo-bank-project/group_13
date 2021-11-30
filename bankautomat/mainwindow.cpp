#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "customer.h"
#include <QThread>

Customer *customer;

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
    customer = new Customer();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_number_clicked()
{
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
            text.append(btn->text());
            customer->withdrawal_amount.append(btn->text());
        }

    }
    else if(btn->text() == "OK"){
        if(vaihe == 0){
            text = ui->textEdit->toPlainText();
            text.append("\n>Syötä PIN koodi:\n<");
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
            //withdraw money
            //show money
            customer->withdrawal_amount.clear();
            set_main_ui();
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
    if(vaihe < 2 || vaihe == 5){
        ui->textEdit->setText(text);
    }
}


void MainWindow::set_start_ui()
{
    ui->textEdit->clear();
    ui->textEdit->setFontPointSize(16);
    ui->textEdit->append(">Syötä korttisi ID numero:\n<");
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
    vaihe = 2;
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
}

void MainWindow::on_pushNosto_clicked()
{
    if(vaihe == 3){
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
        ui->textEdit->append("200");
        ui->textEdit->setFontPointSize(11);
        ui->textEdit->append("\n");
        ui->textEdit->setFontPointSize(20);
        ui->textEdit->append("MUU SUMMA");
        vaihe = 4;
    }

}


void MainWindow::on_pushMuu_clicked()
{
    if(vaihe == 4){
        ui->textEdit->clear();
        ui->textEdit->append("Syötä summa:\n");
        vaihe = 5;
    }
}


void MainWindow::on_pushSaldo_clicked()
{
    if(vaihe == 2){
        customer->credit = false;
        set_main_ui();
    }
}


void MainWindow::on_push20_clicked()
{
    if(vaihe == 2){
        customer->credit = true;
        set_main_ui();
    }
}

