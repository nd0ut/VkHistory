#ifndef VK_AUTH_H
#define VK_AUTH_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QWebView>
#include <QDebug>
#include <QTimer>

class VkAuth : public QWebView
{
    Q_OBJECT
public:
    explicit VkAuth(QString app_id , QWidget *parent = 0);
    ~VkAuth();

    void requestToken();

    QString getAppId();
    QString getAccessToken();
    QString getUserId();

protected:
    QString appId;

    QString accessToken;
    QString userId;

    QNetworkAccessManager *networkManager;

    static const QString className;

signals:
    void success();
    void unsuccess(QString error);

private slots:
    void onUrlChanged(QUrl url);
    
};

#endif // VK_AUTH_H
