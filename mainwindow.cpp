#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    authState = false;

    auth = new VkAuth("3028998");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_authButton_clicked()
{
    if(!authState) {
        auth->requestToken();

        connect(auth, SIGNAL(success()),this, SLOT(onAuthSuccess()));
        connect(auth, SIGNAL(unsuccess(QString)), this, SLOT(onAuthUnsuccess(QString)));

    }
}

void MainWindow::onAuthSuccess()
{
    accessToken = auth->getAccessToken();
    userId = auth->getUserId();
    api = new VkApi(accessToken);

    authState = true;

    QVariantList qlist = api->getUsersInfo(userId, "first_name,last_name");
    QString first_name = qlist.first().toMap()["first_name"].toString();
    QString last_name = qlist.first().toMap()["last_name"].toString();

    ui->authButton->setEnabled(false);
    ui->authButton->setText(first_name + " " + last_name);
    ui->exportHistoryButton->setEnabled(true);


    qlist = api->getUserFriends(userId, "first_name,last_name", "nom", "hints");
    foreach(QVariant cur, qlist) {
        QString uid = cur.toMap()["uid"].toString();
        QString first_name = cur.toMap()["first_name"].toString();
        QString last_name = cur.toMap()["last_name"].toString();

        QString label = first_name + " " + last_name;

        QListWidgetItem *item = new QListWidgetItem(label);
        item->setStatusTip(uid);

        ui->friendList->addItem(item);
    }
    auth->close();
}

void MainWindow::onAuthUnsuccess(QString error)
{
    authState = false;
    auth->close();

    ui->authButton->setEnabled(true);
    ui->exportHistoryButton->setEnabled(false);

    QMessageBox msgBox;
    msgBox.setText("Auth error! Description: " + error);
    msgBox.exec();
}



void MainWindow::on_exportHistoryButton_clicked()
{
    if(!authState) {
        QMessageBox msgBox;
        msgBox.setText("First login please");
        msgBox.exec();

        return;
    }

    if(ui->friendList->selectedItems().empty()){
        QMessageBox msgBox;
        msgBox.setText("No friends selected");
        msgBox.exec();

        return;
    }

    QString folder_path = QFileDialog::getExistingDirectory();

    if(folder_path.isEmpty()) {
        ui->exportHistoryButton->setEnabled(true);
        return;
    }

    ui->exportHistoryButton->setEnabled(false);

    QList<QListWidgetItem *> items = ui->friendList->selectedItems();

    foreach(QListWidgetItem *item, items) {
        qDebug() << "Parsing now: " << item->text();

        QFile file(folder_path + "\\" + item->text() + "(" + item->statusTip() + ")" + ".txt");
        file.open(QIODevice::WriteOnly | QIODevice::Text);

        QVariantList fullList;
        QString offset = QString::number(0);

        ui->progressBar->setValue(0);
        ui->exportHistoryButton->setText("Buffering...");

        while(true) {
            QVariantList qlist = api->getUserHistory(item->statusTip(), QString("36450115"), offset, QString("99"));
            qlist.removeFirst();

            if(qlist.empty())
                break;

            foreach(QVariant msg, qlist) {
                fullList.append(msg);
            }

            offset = QString::number(offset.toInt() + 99);

            if(ui->progressBar->value() == 99)
                ui->progressBar->setValue(0);

            ui->progressBar->setValue(ui->progressBar->value() + 1);
        }

        ui->exportHistoryButton->setText("Writing...");

        QTextStream out(&file);

        if(fullList.isEmpty())
            continue;

        QVariant this_user = api->getUsersInfo(userId, "first_name,last_name").first();
        QVariant remote_user = api->getUsersInfo(item->statusTip(), "first_name,last_name").first();

        for(QVariantList::iterator it = fullList.end() - 1; it >= fullList.begin(); it--) {
            QVariantMap msgMap = it->toMap();

            QString headStr;

            QString first_name;
            QString last_name;

            QString uid = msgMap["uid"].toString();

            if(uid == userId) {
                first_name = this_user.toMap()["first_name"].toString();
                last_name = this_user.toMap()["last_name"].toString();
            }
            else {
                first_name = remote_user.toMap()["first_name"].toString();
                last_name = remote_user.toMap()["last_name"].toString();
            }

            QDateTime date = QDateTime::fromTime_t(msgMap["date"].toInt());

            headStr += date.toString("[hh:mm:ss][dd.MM.yyyy] ");
            headStr += first_name + " " + last_name;


            QString msgStr;
            msgStr += msgMap["body"].toString();
            msgStr += msgMap["attachments"].toString();

            out << headStr;
            out << '\n';
            out << msgStr;
            out << '\n';
            out << '\n';
            out.flush();

            if(ui->progressBar->value() == 99)
                ui->progressBar->setValue(0);

            ui->progressBar->setValue(ui->progressBar->value() + 1);
        }

        ui->progressBar->setValue(100);
        file.close();
    }

    ui->exportHistoryButton->setEnabled(true);
    ui->authButton->setEnabled(true);

    ui->exportHistoryButton->setText("Export");

    ui->authButton->setEnabled(false);

    ui->progressBar->setValue(0);

    QMessageBox msgBox;
    msgBox.setText("Export complete!");
    msgBox.exec();
}
