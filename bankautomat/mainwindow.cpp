#include "mainwindow.h"
#include "ui_mainwindow.h"

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
   ui->textEdit->append(">Syötä korttisi ID numero:\n>>");
   vaihe = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_number_clicked()
{
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    QString text = ui->textEdit->toPlainText();
    if(btn->text() != "<" && btn->text() != ">"){
        text.append(btn->text());

    }
    else if(btn->text() == "OK"){
        if(vaihe == 0){

        }
    }
    else if(btn->text() == ">"){

    }
    ui->textEdit->setText(text);
}

