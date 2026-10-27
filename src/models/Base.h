#ifndef base_h
#define base_h
#include<QString>
#include<QDateTime>
#include<QJsonObject>
class Base{
    public:
        QString created_at;
        QString updated_at;
        Base(){
            QString now=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
            created_at=now;
            updated_at=now;
        }
        virtual ~Base(){}
        //convert to json
        virtual QJsonObject toJson() const{
            QJsonObject obj;
            obj["created_at"]=created_at;
            obj["updated_at"]=updated_at;
            return obj;
        }
        //read from json 
        virtual void fromJson(const QJsonObject &obj){
            QString now=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
            created_at=obj["created_at"].toString(now);
            updated_at=obj["updated_at"].toString(now);

        }
};
#endif