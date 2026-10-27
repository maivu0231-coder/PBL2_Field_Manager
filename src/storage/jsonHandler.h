#ifndef JSONHANDLER_H
#define JSONHANDLER_H
#include<QString>
#include<QFile>
#include<QFileInfo>
#include<QDir>
#include<QJsonDocument>
#include<QJsonArray>
#include<QJsonObject>
#include<QSaveFile>
#include<QIODevice>
#include<QDebug>
class JsonHandler{
    private:
        QString filepath;
        void ensureFileExists() const{
            QFileInfo fileInfo(filepath);
            QDir dir=fileInfo.absoluteDir();
            if(!dir.exists())
            {
                dir.mkpath(".");
            }
            QFile file(filepath);
            if(!file.exists())
            {
                file.open(QIODevice::WriteOnly|QIODevice::Text);
                file.write("[]");
                file.close();
            }
        }
    public:
        explicit JsonHandler(const QString &filepath):filepath(filepath){
            ensureFileExists();
        }
        //read json array from file and returt vector of QJsonObject
        QJsonArray readAll() const {
            QFile file(filepath);
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                return QJsonArray();
            }
            QByteArray raw = file.readAll().trimmed();
            file.close();
            if (raw.isEmpty()) {
                return QJsonArray();
            }
            QJsonParseError err;
            QJsonDocument doc = QJsonDocument::fromJson(raw, &err);
            if (err.error != QJsonParseError::NoError || !doc.isArray()) {
                return QJsonArray();
            }
            return doc.array();
        }
        //write vector of QJsonObject to json file
        bool writeAll(const QJsonArray& arr) const {
            QSaveFile saveFile(filepath); // Atomic write trong Qt
            if (!saveFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                return false;
            }
            QJsonDocument doc(arr);
            saveFile.write(doc.toJson(QJsonDocument::Indented));
            return saveFile.commit();
    }
};
#endif
