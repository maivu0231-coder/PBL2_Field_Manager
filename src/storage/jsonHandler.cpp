#include"jsonHandler.h"
static QJsonArray JsonHandler::ReadJsonFile(const QString &filePath)
{
    // Open the file for reading
    QFile file(filePath);
    // Check if the file was opened successfully
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("Could not open file for reading: %s", qPrintable(file.errorString()));
        return QJsonArray();
    }
    // read the contents of the file into a QByteArray
    QByteArray jsonData = file.readAll();
    file.close();
    // Parse the JSON data
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    if (jsonDoc.isNull() || !jsonDoc.isArray()) {
        qWarning("Invalid JSON data in file: %s", qPrintable(filePath));
        return QJsonArray();
    }
    return jsonDoc.array();//return the QJsonArray include all the JSON objects
}
void JsonHandler::WriteJsonFile(const QString &filePath, const QJsonArray &jsonArray)
{
    // Create a QJsonDocument from the QJsonArray
    QJsonDocument jsonDoc(jsonArray);
    // Open the file for writing
    QFile file(filePath);
    // Check if the file was opened successfully
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning("Could not open file for writing: %s", qPrintable(file.errorString()));
        return;
    }
    // Write the JSON data to the file
    file.write(jsonDoc.toJson());
    // Close the file
    file.close();
}