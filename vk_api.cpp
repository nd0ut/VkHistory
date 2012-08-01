#include "vk_api.h"

const QString VkApi::className = "VkApi";

VkApi::VkApi(QString access_token, QObject *parent) : QObject(parent)
{
    accessToken = access_token;

    loop = new QEventLoop();
    networkManager = new QNetworkAccessManager();

    connect(networkManager, SIGNAL(finished(QNetworkReply*)), loop, SLOT(quit()));
    connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onFinished(QNetworkReply*)));
}

VkApi::~VkApi()
{
    delete loop;
    delete networkManager;
}

void VkApi::onFinished(QNetworkReply *reply)
{
   reply->deleteLater();
}

QByteArray VkApi::sendRequest(QString method, QList<QPair<QString, QString> > params) {
    QUrl url("https://api.vk.com/method/" + method);

    for(QList<QPair<QString, QString> >::iterator cur = params.begin(); cur != params.end(); cur++) {
        url.addQueryItem(cur->first, cur->second);
    }

    url.addQueryItem("access_token",accessToken);

    QNetworkRequest request(url);

    QNetworkReply *reply = networkManager->get(request);
    loop->exec();

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    QString str = QString::fromUtf8(reply->readAll());
    if(!(method == "messages.getHistory"))
        qDebug() << className + ".sendRequest(" + method + ") " + "Reply received: " + str;

    return str.toLocal8Bit();
}

QVariantMap VkApi::parseJson(QByteArray json)
{
    QJson::Parser parser;
    bool ok;

    QVariant result = parser.parse(json, &ok);

    return result.toMap();
}

QVariantList VkApi::getUsersInfo(QString uids, QString fields, QString name_case)
{
    QList<QPair<QString, QString> > params;

    params.append(qMakePair(QString("uids"), uids));

    if(fields != NULL)
        params.append(qMakePair(QString("fields"), fields));

    params.append(qMakePair(QString("name_case"), name_case));

    QByteArray json = sendRequest("users.get", params);

    QVariantMap parsed = parseJson(json);
    QVariantList qlist = parsed.value("response").toList();

    return qlist;
}

QVariantList VkApi::getUserFriends(QString uid, QString fields, QString name_case, QString order)
{
    QList<QPair<QString, QString> > params;

    params.append(qMakePair(QString("uid"), uid));

    if(fields != NULL)
        params.append(qMakePair(QString("fields"), fields));

    params.append(qMakePair(QString("name_case"), name_case));
    params.append(qMakePair(QString("order"), order));

    QByteArray json = sendRequest("friends.get", params);
    QVariantList qlist = parseJson(json).value("response").toList();

    return qlist;
}

QVariantList VkApi::getUserHistory(QString uid, QString chat_id, QString offset, QString count)
{
    QList<QPair<QString, QString> > params;

    params.append(qMakePair(QString("uid"), uid));
    params.append(qMakePair(QString("chat_id"), chat_id));

    if(offset != NULL)
        params.append(qMakePair(QString("offset"), offset));

    if(count != NULL)
        params.append(qMakePair(QString("count"), count));

    QByteArray json = sendRequest("messages.getHistory", params);
    QVariantList qlist = parseJson(json).value("response").toList();

    return qlist;
}

