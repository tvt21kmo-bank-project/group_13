#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    numpad_event_create();
    set_start_ui();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::numpad_event_create()
{
    for(int i=0; i < ui->numberGrid->rowCount(); i++){
        for(int j=0; j < ui->numberGrid->columnCount(); j++){
            QLayoutItem* item = ui->numberGrid->itemAtPosition(i, j);
            if(item != nullptr && item->widget()){
                QPushButton* btn = qobject_cast<QPushButton *>(item->widget());
                connect(btn, &QPushButton::clicked, this, &MainWindow::on_number_clicked);
            }
        }
    }
}
void MainWindow::on_number_clicked()
{
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    QString text = ui->textEdit->toPlainText();
    if(btn->text() != "OK" && btn->text() != "C"){
        if(vaihe == 0){//syötä ID
            customer->cardID.append(btn->text());
            text.append(btn->text());
        }
        else if(vaihe == 1){//syötä pin
            ui->textEdit->setFontPointSize(20);
            if(customer->pin.size() < 4){//PIN luvun pituus 4 merkkiä
                text.append('*');
                customer->pin.append(btn->text());
            }
        }
        else if(vaihe == 5){//syötä muu summa
            customer->withdrawal_amount.append(btn->text());
        }
    }
    else if(btn->text() == "OK"){
        if(vaihe == 0){//syötä ID
            text = ui->textEdit->toPlainText();
            text.append("\nSyötä PIN koodi:\n");
            vaihe = 1;
        }
        else if(vaihe == 1){//syötä PIN
            login(customer->cardID, customer->pin);
            return;
        }
        else if(vaihe == 5){//syötä muu summa
            if(customer->withdrawal_amount.toInt() % 20 != 0){
                alert = "Summan täytyy olla 20:lla jaollinen.";
                customer->withdrawal_amount.clear();
            }
            else if(customer->withdrawal_amount.toInt() > 1000){
                alert = "Maksimi nosto on 1000.";
                customer->withdrawal_amount.clear();
            }
            else if((customer->balance - customer->withdrawal_amount.toInt()) < 0){
                alert = "Tililläsi ei ole tarpeeksi rahaa tähän nostoon";
                customer->withdrawal_amount.clear();
            }
            else{
                //withdraw money
                //show money
                alert = "Tililtä nostettu onnistuneesti " + customer->withdrawal_amount;
                customer->withdrawal_amount.clear();
                set_alert_ui(2);
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
        ui->textEdit->setFontPointSize(20);
        ui->textEdit->setText(text);
        set_timer(10);
    }
    else if(vaihe == 5){
        if(alert == ""){
            set_sum_ui();
        }
        else{
            set_alert_ui(5);
            set_sum_ui();
        }
    }
}

void MainWindow::login(QString idCard, QString pin)
{
    QNetworkRequest request;
    QNetworkAccessManager * mgr = new QNetworkAccessManager();
    connect(mgr, SIGNAL(finished(QNetworkReply*)),this,SLOT(on_finish(QNetworkReply*)));
    connect(mgr, SIGNAL(finished(QNetworkReply*)),mgr, SLOT(deleteLater()));
    QUrlQuery post_data;
    post_data.addQueryItem("korttiID", idCard);
    post_data.addQueryItem("korttiPIN", pin);
    QString site_url = "http://localhost:3000/bankautomat/kirjaudu_sisaan/";
    request.setUrl(site_url);
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    mgr->post(request, post_data.toString().toUtf8());
}

void MainWindow::get_balance()
{
    QNetworkRequest request;
    QNetworkAccessManager * mgr = new QNetworkAccessManager();
    connect(mgr, SIGNAL(finished(QNetworkReply*)),this,SLOT(on_finish(QNetworkReply*)));
    connect(mgr, SIGNAL(finished(QNetworkReply*)),mgr, SLOT(deleteLater()));
    QUrlQuery post_data;
    QString tili_str = QString::number(customer->account);
    QString luotto_str = QString::number(customer->credit);
    post_data.addQueryItem("tiliID", tili_str);
    post_data.addQueryItem("luotto", luotto_str);
    QString site_url = "http://localhost:3000/bankautomat/tarkista_saldo/";
    request.setUrl(site_url);
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    mgr->post(request, post_data.toString().toUtf8());
}

void MainWindow::on_finish(QNetworkReply *reply)
{
    if(vaihe == 1){
        QString response = (QString)reply->readAll();
        QJsonDocument temp = QJsonDocument::fromJson(response.toUtf8());
        QJsonArray arr = temp.array();
        QJsonObject jsonObj;
        foreach (const QJsonValue & value, arr){
            if(value.isArray()){
                QJsonArray array2 = value.toArray();
                foreach(const QJsonValue & value2, array2){
                    if(value2.isObject()){
                        jsonObj = value2.toObject();
                    }
                }
            }
        }
        customer->account = jsonObj.value("tiliID").toDouble();
        qDebug() << "tiliID:" << customer->account;
        customer->credit = jsonObj.value("credit").toDouble();
        qDebug() << "credit:" << customer->credit;
        customer->first_name = jsonObj.value("etunimi").toString();
        qDebug() << "etunimi:" << customer->first_name;
        customer->last_name = jsonObj.value("sukunimi").toString();
        qDebug() << "sukunimi:" << customer->last_name;
        double failed = jsonObj.value("failCount").toDouble();
        qDebug() << "fail count:" << failed;
        if(failed > 2){
            alert = "3 epäonnistunutta kirjautumista, korttisi on lukittu";
            set_alert_ui(5);
            set_start_ui();
        }
        else if(customer->account == 0){
            alert = "Kirjautuminen epäonnistui.";
            set_alert_ui(5);
            set_start_ui();
        }
        else if(customer->account > 0 && customer->credit){
            alert = "Kirjautuminen onnistui. Tervetuloa " + customer->first_name + " " + customer->last_name + ".";
            set_alert_ui(2);
            set_credit_or_debit_ui();
        }
        else if(customer->account > 0 && !customer->credit){
            alert = "Kirjautuminen onnistui. Tervetuloa " + customer->first_name + " " + customer->last_name + ".";
            set_alert_ui(2);
            set_main_ui();
        }
    }
    else if(vaihe == 3){
        QString response = (QString)reply->readAll();
        qDebug() << response;
        QJsonDocument temp = QJsonDocument::fromJson(response.toUtf8());
        QJsonArray arr = temp.array();
        QJsonObject jsonObj;
        foreach (const QJsonValue & value, arr){
            if(value.isArray()){
                QJsonArray array2 = value.toArray();
                foreach(const QJsonValue & value2, array2){
                    if(value2.isObject()){
                        jsonObj = value2.toObject();
                    }
                    if(value2.isArray()){
                        QJsonArray array3 = value.toArray();
                        foreach (const QJsonValue & value3, array3) {
                            if(value3.isObject()){
                                jsonObj = value3.toObject();
                            }
                        }
                    }
                }
            }
        }
        customer->balance = jsonObj.value("saldo").toDouble();
        qDebug() << "saldo:" << customer->balance;
    }
}

void MainWindow::set_start_ui()
{
    customer = NULL;
    customer = new Customer();
    ui->textEdit->clear();
    ui->textEdit->setFontPointSize(20);
    ui->textEdit->append("Syötä ID numero:\n");
    vaihe = 0;
    set_timer(10);
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
    set_timer(10);
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
    set_timer(30);
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
    set_timer(10);
}

void MainWindow::set_withdraw_ui()
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
    set_timer(10);
}

void MainWindow::set_sum_ui()
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
    set_timer(10);
}
void MainWindow::set_alert_ui(int alert_time)
{
    timer->stop();
    timer = NULL;
    ui->textEdit->clear();
    QTextCursor cursor = ui->textEdit->textCursor();
    QTextBlockFormat textBlockFormat = cursor.blockFormat();
    textBlockFormat.setAlignment(Qt::AlignCenter);
    cursor.mergeBlockFormat(textBlockFormat);
    ui->textEdit->setTextCursor(cursor);
    ui->textEdit->setFontPointSize(16);
    ui->textEdit->append(alert);
    vaihe = 100;
    delay(alert_time * 1000);
    alert = "";
}
void MainWindow::on_pushNosto_clicked()
{
    if(vaihe == 3){
        set_withdraw_ui();
    }
}


void MainWindow::on_pushMuu_clicked()
{
    if(vaihe == 4){
        set_sum_ui();
    }
}


void MainWindow::on_pushSaldo_clicked()
{
    if(vaihe == 2){
        customer->credit = false;
        set_main_ui();
    }
    else if(vaihe == 3){
        get_balance();
    }
}

void MainWindow::on_pushTapahtumat_clicked()
{
    if(vaihe == 3){
        //set_transactions_ui();
    }
}

void MainWindow::on_push20_clicked()
{
    if(vaihe == 2){
        customer->credit = true;
        set_main_ui();
    }
    else if(vaihe == 4){
        if(customer->balance < 20){
            alert = "Tililläsi ei ole tarpeeksi rahaa tähän nostoon";
            set_alert_ui(5);
            set_withdraw_ui();
        }
        else{
            alert = "Tililtä nostettu onnistuneesti 20";
            set_alert_ui(2);
            set_main_ui();
        }
    }
}

void MainWindow::on_push50_clicked()
{
    if(vaihe == 4){
        if(customer->balance < 50){
            alert = "Tililläsi ei ole tarpeeksi rahaa tähän nostoon";
            set_alert_ui(5);
            set_withdraw_ui();
        }
        else{
            alert = "Tililtä nostettu onnistuneesti 50";
            set_alert_ui(2);
            set_main_ui();
        }
        set_timer(10);
    }
}

void MainWindow::on_push100_clicked()
{
    if(vaihe == 4){
        if(customer->balance < 100){
            alert = "Tililläsi ei ole tarpeeksi rahaa tähän nostoon";
            set_alert_ui(5);
            set_withdraw_ui();
        }
        else{
            alert = "Tililtä nostettu onnistuneesti 100";
            set_alert_ui(2);
            set_main_ui();
        }
    }
}

void MainWindow::on_push200_clicked()
{
    if(vaihe == 4){
        timer->stop();
        if(customer->balance < 200){
            alert = "Tililläsi ei ole tarpeeksi rahaa tähän nostoon";
            set_alert_ui(5);
            set_withdraw_ui();
        }
        else{
            alert = "Tililtä nostettu onnistuneesti 200";
            set_alert_ui(2);
            set_main_ui();
        }

    }
}

void MainWindow::on_pushLopeta_clicked()
{
    if(vaihe == 2 || vaihe == 3){
        set_start_ui();
    }
    else if(vaihe == 4 || vaihe == 6 || vaihe == 7){
        set_main_ui();
    }
    else if(vaihe == 5){
        set_withdraw_ui();
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

void MainWindow::set_timer(int timeout)
{
    if(timer != nullptr){
        timer->stop();
        timer = NULL;
    }
    timer = new QTimer();
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(on_timer_timeout()));
    timer->start(timeout * 1000);
}
void MainWindow::on_timer_timeout()
{
    qDebug() << "Ajastin triggeröity " << QDateTime::currentDateTime().toString();
    if(vaihe < 4 ){
        set_start_ui();
    }

    else if(vaihe == 5){
        set_withdraw_ui();
    }
    else if(vaihe == 6 || vaihe == 4 || vaihe == 7){
        set_main_ui();
    }
}

