#ifndef VK_API_H
#define VK_API_H

#include <QObject>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QByteArray>
#include <QPair>
#include <QList>
#include <QString>
#include <QDebug>
#include <QTextCodec>
#include <QJson/Parser>

class VkApi : public QObject
{
    Q_OBJECT

public:
    explicit VkApi(QString access_token, QObject *parent = 0);
    ~VkApi();

    QByteArray sendRequest(QString method, QList<QPair<QString, QString> > params);

    QVariantMap parseJson(QByteArray json);

    QVariantList getUsersInfo(QString uids, QString fields = NULL, QString name_case = "nom");
    QVariantList getUserFriends(QString uid, QString fields = NULL, QString name_case = "nom",
                                   QString order = "name");
    QVariantList getUsetHistory(QString uid, QString chat_id, QString offset = NULL, QString count = NULL);

signals:
    
public slots:
    void onFinished(QNetworkReply *reply);

private:
    QEventLoop *loop;
    QNetworkAccessManager *networkManager;

    QString accessToken;

    static const QString className;
};

#endif // VK_API_H
