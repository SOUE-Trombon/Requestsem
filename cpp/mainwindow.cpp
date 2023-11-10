#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageAuthenticationCode>
#include <QNetworkAccessManager>
#include <QNetworkReply>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->sendButton, &QAbstractButton::clicked, this, &MainWindow::send);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::send()
{
    QByteArray publicKey = ui->publicKeyEdit->text().toUtf8();
    QByteArray privateKey = ui->privateKeyEdit->text().toUtf8();
    QByteArray nonce = ui->nonceEdit->text().toUtf8();
    QByteArray address = ui->serverAddressEdit->text().toUtf8();
    QByteArray apiPath = ui->apiMethodEdit->text().toUtf8();
    QByteArray urlParams = ui->urlParams->text().toUtf8();
    QString fullPath = address + apiPath;
    if(!urlParams.isEmpty()) fullPath += "?" + urlParams;

    QJsonDocument doc = QJsonDocument::fromJson(ui->requestBodyEdit->toPlainText().toUtf8());
    QByteArray strBody(doc.toJson(QJsonDocument::Compact));

    QByteArray signature = apiPath + nonce + strBody;
    QByteArray signatureCalculated = QMessageAuthenticationCode::hash(signature, privateKey, QCryptographicHash::Sha1).toHex();

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, [this](QNetworkReply* reply)
    {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        this->ui->responseView->setText(doc.toJson());
    });

    QNetworkRequest req;

    req.setRawHeader("trombon-apikey", publicKey);
    req.setRawHeader("trombon-nonce", nonce);
    req.setRawHeader("trombon-signature", signatureCalculated);
    req.setRawHeader("Content-Type", "application/json");
    QUrl url = QUrl(fullPath);
    req.setUrl(url);

    int tempNonce = ui->nonceEdit->text().toInt();
    tempNonce++;

    ui->nonceEdit->setText(QString::number(tempNonce));
    if(ui->getButton->isChecked()) { manager->get(req); return; }
    if(ui->postButton->isChecked()) { manager->post(req, strBody); return; }
}

