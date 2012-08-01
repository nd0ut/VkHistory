#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <Qfile>
#include <QDateTime>

#include "vk_auth.h"
#include "vk_api.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_authButton_clicked();
    void onAuthSuccess();
    void onAuthUnsuccess(QString error);

    void on_exportHistoryButton_clicked();

private:
    Ui::MainWindow *ui;
    VkAuth *auth;
    VkApi *api;

    QString accessToken;
    QString userId;

    bool authState;
};

#endif // MAINWINDOW_H
