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
                if(btn->text() != "C" && btn->text() != "OK"){
                    connect(btn, &QPushButton::clicked, this, &MainWindow::on_number_clicked);
                }
            }
        }
    }
}
void MainWindow::on_number_clicked()
{
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    QString text = ui->textEdit->toPlainText();
    if(phase == START_PHASE){//syötä ID
        customer->cardID.append(btn->text());
        text.append(btn->text());
        ui->textEdit->setFontPointSize(20);
        ui->textEdit->setText(text);
        set_timer(10);
    }
    else if(phase == LOGIN_PHASE){//syötä pin
        ui->textEdit->setFontPointSize(20);
        if(customer->pin.size() < 4){//PIN luvun pituus 4 merkkiä
            text.append('*');
            customer->pin.append(btn->text());
        }
        ui->textEdit->setFontPointSize(20);
        ui->textEdit->setText(text);
        set_timer(10);
    }
    else if(phase == SUM_PHASE){//syötä muu summa
        customer->withdrawal_amount.append(btn->text());
        set_sum_ui();
    }
}

void MainWindow::on_pushBack_clicked()
{
    QString text = ui->textEdit->toPlainText();
    switch(phase){
    case START_PHASE:
        if(customer->cardID.size() > 0){
            customer->cardID.resize(customer->cardID.size() - 1);
            text.resize(text.size() - 1);
        }
        ui->textEdit->setFontPointSize(20);
        ui->textEdit->setText(text);
        set_timer(10);
        break;
    case LOGIN_PHASE:
        if(customer->pin.size() > 0){
            customer->pin.resize(customer->pin.size() - 1);
            text.resize(text.size() - 1);
        }
        ui->textEdit->setFontPointSize(20);
        ui->textEdit->setText(text);
        set_timer(10);
        break;
    case SUM_PHASE:
        if(customer->withdrawal_amount.size() > 0){
            customer->withdrawal_amount.resize(customer->withdrawal_amount.size() - 1);
            text.resize(text.size() - 1);
        }
        set_sum_ui();
        break;
    default:
        break;
    }
}

void MainWindow::on_pushEnter_clicked()
{
    QString text = ui->textEdit->toPlainText();
    switch(phase){
    case START_PHASE:
        text = ui->textEdit->toPlainText();
        text.append("\nSyötä PIN koodi:\n");
        ui->textEdit->setFontPointSize(20);
        ui->textEdit->setText(text);
        set_timer(10);
        phase = LOGIN_PHASE;
        break;
    case LOGIN_PHASE:
        login(customer->cardID, customer->pin);
        break;
    case SUM_PHASE:
        if(customer->withdrawal_amount.toInt() % 20 != 0){
            QString alert_text = "Summan täytyy olla 20:lla jaollinen.";
            customer->withdrawal_amount.clear();
            set_alert_ui(alert_text, 5);
        }
        else if(customer->withdrawal_amount.toInt() > 1000){
            QString alert_text = "Maksimi nosto on 1000.";
            customer->withdrawal_amount.clear();
            set_alert_ui(alert_text, 5);
        }
        else {
            QString sum = customer->withdrawal_amount;
            customer->withdrawal_amount.clear();
            withdraw_money(sum);
        }
        set_sum_ui();
        break;
    default:
        break;
    }
}

void MainWindow::login(QString idCard, QString pin)
{
    QNetworkRequest request;
    QNetworkAccessManager * mgr = new QNetworkAccessManager();
    connect(mgr, SIGNAL(finished(QNetworkReply*)),this,SLOT(on_login(QNetworkReply*)));
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
    connect(mgr, SIGNAL(finished(QNetworkReply*)),this,SLOT(on_balance_query(QNetworkReply*)));
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

void MainWindow::get_transactions(int page)
{
    QNetworkRequest request;
    QNetworkAccessManager * mgr = new QNetworkAccessManager();
    connect(mgr, SIGNAL(finished(QNetworkReply*)),this,SLOT(on_transactions_query(QNetworkReply*)));
    connect(mgr, SIGNAL(finished(QNetworkReply*)),mgr, SLOT(deleteLater()));
    QUrlQuery post_data;
    QString tili_str = QString::number(customer->account);
    post_data.addQueryItem("tiliID", tili_str);
    post_data.addQueryItem("alkaen_tapahtumasta", QString::number(page * 9));
    QString site_url = "http://localhost:3000/bankautomat/selaa_tilitapahtumia/";
    request.setUrl(site_url);
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    mgr->post(request, post_data.toString().toUtf8());
}

void MainWindow::withdraw_money(QString sum)
{
    QNetworkRequest request;
    QNetworkAccessManager * mgr = new QNetworkAccessManager();
    connect(mgr, SIGNAL(finished(QNetworkReply*)),this,SLOT(on_withdrawal(QNetworkReply*)));
    connect(mgr, SIGNAL(finished(QNetworkReply*)),mgr, SLOT(deleteLater()));
    QUrlQuery post_data;
    QString tili_str = QString::number(customer->account);
    QString luotto_str = QString::number(customer->credit);
    QString kortti_str = customer->cardID;
    post_data.addQueryItem("tiliID", tili_str);
    post_data.addQueryItem("korttiID", kortti_str);
    post_data.addQueryItem("luotto", luotto_str);
    post_data.addQueryItem("summa", sum);
    QString site_url = "http://localhost:3000/bankautomat/nosta_rahaa/";
    request.setUrl(site_url);
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    mgr->post(request, post_data.toString().toUtf8());
}

void MainWindow::on_login(QNetworkReply *reply)
{
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
        QString alert_text = "3 epäonnistunutta kirjautumista, korttisi on lukittu";
        set_alert_ui(alert_text, 5);
        set_start_ui();
    }
    else if(customer->account == 0){
        QString alert_text = "Kirjautuminen epäonnistui.";
        set_alert_ui(alert_text, 5);
        set_start_ui();
    }
    else if(customer->account > 0 && customer->credit){
        QString alert_text = "Kirjautuminen onnistui. Tervetuloa " + customer->first_name + " " + customer->last_name + ".";
        set_alert_ui(alert_text, 2);
        set_credit_or_debit_ui();
    }
    else if(customer->account > 0 && !customer->credit){
        QString alert_text = "Kirjautuminen onnistui. Tervetuloa " + customer->first_name + " " + customer->last_name + ".";
        set_alert_ui(alert_text, 2);
        set_main_ui();
    }
}

void MainWindow::on_balance_query(QNetworkReply *reply)
{
    QString response = (QString)reply->readAll();
    qDebug() << response;
    QJsonDocument temp = QJsonDocument::fromJson(response.toUtf8());
    QJsonArray arr = temp.array();
    QJsonObject jsonObj;
    int array_count = 0;
    foreach (const QJsonValue & value, arr){
        if(value.isArray()){
            QJsonArray array2 = value.toArray();
            qDebug() << "value is array";
            array_count++;
            foreach(const QJsonValue & value2, array2){
                if(value2.isObject()){
                    qDebug() << "value2 is object";
                    jsonObj = value2.toObject();
                    if(array_count == 1){
                        customer->balance = jsonObj.value("saldo").toDouble();
                        qDebug() << "saldo:" << customer->balance;
                        if(customer->credit){
                            customer->credit_limit = jsonObj.value("luottoraja").toDouble();
                        }
                    }
                    else{
                        QString transaction_type;
                        if(jsonObj.value("transaction_type").toDouble() == DEBIT_WITHDRAWAL){
                            transaction_type = "Nosto/D|";
                        }
                        else if(jsonObj.value("transaction_type").toDouble() == CREDIT_WITHDRAWAL){
                            transaction_type = "Nosto/C|";
                        }
                        else if(jsonObj.value("transaction_type").toDouble() == DEBIT_PURCHASE){
                            transaction_type = "Osto/D |";
                        }
                        else if(jsonObj.value("transaction_type").toDouble() == CREDIT_PURCHASE){
                            transaction_type = "Osto/C |";
                        }
                        else if(jsonObj.value("transaction_type").toDouble() == DEBIT_DEPOSIT){
                            transaction_type = "Pano/D |";
                        }
                        else if(jsonObj.value("transaction_type").toDouble() == CREDIT_DEPOSIT){
                            transaction_type = "Pano/C |";
                        }
                        QString transaction_datetime = jsonObj.value("transaction_time").toString().replace("Z","").replace("T", "|").replace(".000","|");
                        QString transaction_amount;
                        for(int i = QString::number(jsonObj.value("transaction_amount").toDouble()).size(); i < 7; i++){
                            transaction_amount += " ";
                        }
                        transaction_amount += QString::number(jsonObj.value("transaction_amount").toDouble());
                        transaction_amount += "€|";
                        QString transaction_card =  QString::number(jsonObj.value("idcard").toDouble());
                        for(int i = transaction_card.size(); i < 5; i++){
                            transaction_card += " ";
                        }
                        transaction_card += "|";
                        QString transactions_str = transaction_datetime + transaction_card + transaction_type + transaction_amount;
                        qDebug() << transactions_str;
                        customer->transactions.push_back(transactions_str);
                    }
                }
            }
        }
    }
    set_balance_ui();
}

void MainWindow::on_withdrawal(QNetworkReply *reply)
{
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
                    if(jsonObj.value("FAIL").toString() == "FAIL"){
                        QString alert_text = "Tililläsi ei ole tarpeeksi rahaa.";
                        set_alert_ui(alert_text, 2);
                        if(phase == WITHDRAW_PHASE){
                            set_withdraw_ui();
                        }
                        else if(phase == SUM_PHASE){
                            set_sum_ui();
                        }
                    }
                    else if(jsonObj.value("OK").toString() == "OK"){
                        QString alert_text = "Nosto suoritettu onnistuneesti.";
                        set_alert_ui(alert_text, 2);
                        set_main_ui();
                    }
                }
            }
        }
    }
}

void MainWindow::on_transactions_query(QNetworkReply *reply)
{
    QString response = (QString)reply->readAll();
    qDebug() << response;
    QJsonDocument temp = QJsonDocument::fromJson(response.toUtf8());
    QJsonArray arr = temp.array();
    QJsonObject jsonObj;
    int array_count = 0;
    foreach (const QJsonValue & value, arr){
        if(value.isArray()){
            QJsonArray array2 = value.toArray();
            qDebug() << "value is array";
            array_count++;
            foreach(const QJsonValue & value2, array2){
                if(value2.isObject()){
                    qDebug() << "value2 is object";
                    jsonObj = value2.toObject();
                    if(array_count == 1){
                        QString transaction_type;
                        if(jsonObj.value("transaction_type").toDouble() == DEBIT_WITHDRAWAL){
                            transaction_type = "Nosto/D|";
                        }
                        else if(jsonObj.value("transaction_type").toDouble() == CREDIT_WITHDRAWAL){
                            transaction_type = "Nosto/C|";
                        }
                        else if(jsonObj.value("transaction_type").toDouble() == DEBIT_PURCHASE){
                            transaction_type = "Osto/D |";
                        }
                        else if(jsonObj.value("transaction_type").toDouble() == CREDIT_PURCHASE){
                            transaction_type = "Osto/C |";
                        }
                        else if(jsonObj.value("transaction_type").toDouble() == DEBIT_DEPOSIT){
                            transaction_type = "Pano/D |";
                        }
                        else if(jsonObj.value("transaction_type").toDouble() == CREDIT_DEPOSIT){
                            transaction_type = "Pano/C |";
                        }
                        QString transaction_datetime = jsonObj.value("transaction_time").toString().replace("Z","").replace("T", "|").replace(".000","|");
                        QString transaction_amount;
                        for(int i = QString::number(jsonObj.value("transaction_amount").toDouble()).size(); i < 7; i++){
                            transaction_amount += " ";
                        }
                        transaction_amount += QString::number(jsonObj.value("transaction_amount").toDouble());
                        transaction_amount += "€|";
                        QString transaction_card =  QString::number(jsonObj.value("idcard").toDouble());
                        for(int i = transaction_card.size(); i < 7; i++){
                            transaction_card += " ";
                        }
                        transaction_card += "|";
                        QString transaction_balance = QString::number(jsonObj.value("balance").toDouble());
                        QString transactions_str = transaction_datetime + transaction_card + transaction_type + transaction_amount;
                        qDebug() << transactions_str;
                        customer->transactions.push_back(transactions_str);
                    }
                }
            }
        }
    }
    if(customer->transactions.size() == 0){
        if(customer->page == 0){
            QString alert_text = "Ei tilitapahtumia.";
            set_alert_ui(alert_text, 2);
            set_main_ui();
        }
        else if(customer->page > 0){
            customer->page--;
            QString alert_text = "Ei enempää tilitapahtumia.";
            set_alert_ui(alert_text, 2);
            get_transactions(customer->page);
        }
    }
    else{
        set_transactions_ui();
    }
}

void MainWindow::set_start_ui()
{
    customer = NULL;
    customer = new Customer();
    ui->textEdit->clear();
    ui->textEdit->setFontPointSize(20);
    ui->textEdit->append("Syötä ID numero:\n");
    phase = START_PHASE;
    set_timer(10);
}

void MainWindow::set_credit_or_debit_ui()
{
    ui->textEdit->setFontPointSize(7);
    ui->textEdit->setText(" ");
    QTextCursor cursor = ui->textEdit->textCursor();
    QTextBlockFormat textBlockFormat = cursor.blockFormat();
    textBlockFormat.setAlignment(Qt::AlignLeft);
    cursor.mergeBlockFormat(textBlockFormat);
    ui->textEdit->setTextCursor(cursor);
    ui->textEdit->setFontPointSize(20);
    ui->textEdit->append("DEBIT                   CREDIT");
    ui->textEdit->setFontPointSize(103);
    ui->textEdit->append(" ");
    ui->textEdit->setFontPointSize(20);
    ui->textEdit->append("SULJE");
    phase = CREDIT_PHASE;
    set_timer(10);
}

void MainWindow::set_main_ui()
{
    ui->textEdit->setFontPointSize(7);
    ui->textEdit->setText(" ");
    QTextCursor cursor = ui->textEdit->textCursor();
    QTextBlockFormat textBlockFormat = cursor.blockFormat();
    textBlockFormat.setAlignment(Qt::AlignLeft);
    cursor.mergeBlockFormat(textBlockFormat);
    ui->textEdit->setTextCursor(cursor);
    ui->textEdit->setFontPointSize(20);
    ui->textEdit->append("SALDO");
    ui->textEdit->setFontPointSize(21);
    ui->textEdit->append(" ");
    ui->textEdit->setFontPointSize(20);
    ui->textEdit->append("TAPAHTUMAT");
    ui->textEdit->setFontPointSize(21);
    ui->textEdit->append(" ");
    ui->textEdit->setFontPointSize(20);
    ui->textEdit->append("NOSTO");
    ui->textEdit->setFontPointSize(21);
    ui->textEdit->append(" ");
    ui->textEdit->setFontPointSize(20);
    ui->textEdit->append("LOPETA");
    phase = MAIN_PHASE;
    set_timer(30);
}

void MainWindow::set_balance_ui()
{
    ui->textEdit->clear();
    QTextCursor cursor = ui->textEdit->textCursor();
    QTextBlockFormat textBlockFormat = cursor.blockFormat();
    textBlockFormat.setAlignment(Qt::AlignLeft);
    cursor.mergeBlockFormat(textBlockFormat);
    ui->textEdit->setTextCursor(cursor);
    ui->textEdit->setFontPointSize(16);
    if(!customer->credit){
        ui->textEdit->append("Saldo: " + QString::number(customer->balance) + "€");
    }
    else{
        ui->textEdit->append("Saldo: " + QString::number(customer->balance) + "€ Luottoraja: " + QString::number(customer->credit_limit) + "€");
    }
    ui->textEdit->setFontPointSize(12);
    for(int i = 0; i < customer->transactions.size(); i++){
        ui->textEdit->append(customer->transactions.at(i));
    }
    ui->textEdit->setFontPointSize(12 * (10 - customer->transactions.size()) + 10 - 16);
    ui->textEdit->append(" ");
    customer->transactions.clear();
    ui->textEdit->setFontPointSize(20);
    ui->textEdit->append("SULJE");
    phase = BALANCE_PHASE;
    set_timer(10);
}

void MainWindow::set_transactions_ui()
{
    ui->textEdit->clear();
    QTextCursor cursor = ui->textEdit->textCursor();
    QTextBlockFormat textBlockFormat = cursor.blockFormat();
    textBlockFormat.setAlignment(Qt::AlignLeft);
    cursor.mergeBlockFormat(textBlockFormat);
    ui->textEdit->setTextCursor(cursor);
    ui->textEdit->setFontPointSize(12);
    ui->textEdit->append("Pvm       |Aika    |Kortti |Tyyppi |Summa   |");
    for(int i = 0; i < customer->transactions.size(); i++){
        ui->textEdit->append(customer->transactions.at(i));
    }
    for(int i = customer->transactions.size(); i < 9; i++){
        ui->textEdit->setFontPointSize(12);
        ui->textEdit->append(" ");
    }
    ui->textEdit->setFontPointSize(6);
    ui->textEdit->append(" ");
    customer->transactions.clear();
    ui->textEdit->setFontPointSize(20);
    ui->textEdit->append("SULJE                SEURAAVAT");
    ui->textEdit->setFontPointSize(21);
    ui->textEdit->append(" ");
    ui->textEdit->setFontPointSize(20);
    ui->textEdit->append("                     EDELLISET");
    phase = TRANSACTION_PHASE;
    set_timer(10);
}

void MainWindow::set_withdraw_ui()
{
    ui->textEdit->setFontPointSize(7);
    ui->textEdit->setText(" ");
    QTextCursor cursor = ui->textEdit->textCursor();
    QTextBlockFormat textBlockFormat = cursor.blockFormat();
    textBlockFormat.setAlignment(Qt::AlignRight);
    cursor.mergeBlockFormat(textBlockFormat);
    ui->textEdit->setTextCursor(cursor);
    ui->textEdit->setFontPointSize(20);
    ui->textEdit->append("20€");
    ui->textEdit->setFontPointSize(21);
    ui->textEdit->append(" ");
    ui->textEdit->setFontPointSize(20);
    ui->textEdit->append("50€");
    ui->textEdit->setFontPointSize(21);
    ui->textEdit->append(" ");
    ui->textEdit->setFontPointSize(20);
    ui->textEdit->append("100€");
    ui->textEdit->setFontPointSize(21);
    ui->textEdit->append(" ");
    ui->textEdit->setFontPointSize(20);
    ui->textEdit->append("SULJE                     200€");
    ui->textEdit->setFontPointSize(21);
    ui->textEdit->append(" ");
    ui->textEdit->setFontPointSize(20);
    ui->textEdit->append("MUU SUMMA");
    phase = WITHDRAW_PHASE;
    set_timer(10);
}

void MainWindow::set_sum_ui()
{
    ui->textEdit->setFontPointSize(7);
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
    ui->textEdit->setFontPointSize(83);
    ui->textEdit->append(" ");
    ui->textEdit->setFontPointSize(20);
    ui->textEdit->append("SULJE");
    phase = SUM_PHASE;
    set_timer(10);
}

void MainWindow::set_alert_ui(QString alert_text, int alert_time)
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
    ui->textEdit->append(alert_text);
    phase = ALERT_PHASE;
    delay(alert_time * 1000);
}

void MainWindow::on_pushNosto_clicked()
{
    if(phase == MAIN_PHASE){
        set_withdraw_ui();
    }
}


void MainWindow::on_pushMuu_clicked()
{
    if(phase == WITHDRAW_PHASE){
        set_sum_ui();
    }
    if(phase == TRANSACTION_PHASE){
        customer->page++;
        get_transactions(customer->page);
    }
}


void MainWindow::on_pushSaldo_clicked()
{
    if(phase == CREDIT_PHASE){
        customer->credit = false;
        set_main_ui();
    }
    else if(phase == MAIN_PHASE){
        get_balance();
    }
}

void MainWindow::on_pushTapahtumat_clicked()
{
    if(phase == MAIN_PHASE){
        customer->page = 0;
        get_transactions(customer->page);
    }
}

void MainWindow::on_push20_clicked()
{
    if(phase == CREDIT_PHASE){
        customer->credit = true;
        set_main_ui();
    }
    else if(phase == WITHDRAW_PHASE){
        withdraw_money("20");
    }
}

void MainWindow::on_push50_clicked()
{
    if(phase == WITHDRAW_PHASE){
        withdraw_money("50");
    }
}

void MainWindow::on_push100_clicked()
{
    if(phase == WITHDRAW_PHASE){
        withdraw_money("100");
    }
}

void MainWindow::on_push200_clicked()
{
    if(phase == WITHDRAW_PHASE){
        withdraw_money("200");
    }
    else if(phase == TRANSACTION_PHASE){
        if(customer->page > 0)
        {
            customer->page--;
            get_transactions(customer->page);
        }
    }
}

void MainWindow::on_pushLopeta_clicked()
{
    if(phase == CREDIT_PHASE || phase == MAIN_PHASE){
        set_start_ui();
    }
    else if(phase == WITHDRAW_PHASE || phase == BALANCE_PHASE || phase == TRANSACTION_PHASE){
        set_main_ui();
    }
    else if(phase == SUM_PHASE){
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
    if(phase == LOGIN_PHASE || phase == CREDIT_PHASE || phase == MAIN_PHASE){
        set_start_ui();
    }
    else if(phase == WITHDRAW_PHASE || phase == BALANCE_PHASE || phase == TRANSACTION_PHASE){
        set_main_ui();
    }
    else if(phase == SUM_PHASE){
        set_withdraw_ui();
    }
}

