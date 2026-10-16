#ifndef json_handler_h
#define json_handler_h
#include<QString>
#include<QFile>
#include<QJsonArray>
#include<QJsonDocument>
class JsonHandler{
    public:
        //read json file and return QJsonArray
        static QJsonArray ReadJsonFile(const QString &filePath);
        //write QJsonArray to json file
        static void WriteJsonFile(const QString &filePath, const QJsonArray &jsonArray);
};
#endif