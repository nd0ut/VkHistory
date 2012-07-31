#include "vk_auth.h"

const QString VkAuth::className = "VkAuth";

void VkAuth::requestToken()
{
    QUrl url("http://api.vk.com/oauth/authorize");

    url.addQueryItem("client_id",       appId   );
    url.addQueryItem("redirect_uri",    "http://api.vk.com/blank.html"  );
    url.addQueryItem("display",         "page");
    url.addQueryItem("scope",           "messages,friends");
    url.addQueryItem("response_type",   "token");

    show();
    load(url);
}

VkAuth::VkAuth(QString app_id, QWidget *parent) : QWebView(parent)
{
    appId = app_id;

    networkManager = new QNetworkAccessManager;

    connect(this, SIGNAL(urlChanged(QUrl)), SLOT(onUrlChanged(QUrl)));
}

VkAuth::~VkAuth()
{
    delete networkManager;
}


QString VkAuth::getAppId()
{
    return appId;
}

QString VkAuth::getAccessToken()
{
    return accessToken;
}

QString VkAuth::getUserId()
{
    return userId;
}

void VkAuth::onUrlChanged(QUrl url)
{
    url = url.toString().replace('#', "?");

    if(url.encodedHost().contains("oauth.vk.com"))
        return;

    if(url.hasEncodedQueryItem("error")) {
        QString error = url.encodedQueryItemValue("access_token");

        qDebug() << className + ". " + "Auth unsuccess! Error description: " + error;

        emit unsuccess(error);
        return;
    }

    if(url.hasEncodedQueryItem("access_token")) {
        accessToken = url.encodedQueryItemValue("access_token");
        userId      = url.encodedQueryItemValue("user_id");

        qDebug() << className + ". " + "Auth succesful!";
        qDebug() << "accessToken: " + accessToken;
        qDebug() << "userId: " + userId;

        emit success();
    }

    return;
}

