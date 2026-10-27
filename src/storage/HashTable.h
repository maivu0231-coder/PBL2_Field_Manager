#ifndef hashtable_h
#define hashtable_h
#include<QString>
#include<vector>
#include<iostream>
#include<optional>
#include<algorithm>
using namespace std;
template<typename V>
struct HashNode{
    QString key;
    V value;
    HashNode(const QString &key,const V &   value):key(key),value(value){}
};
template<typename V>
class HashTable{
    private:
        int capacity;
        int count;
        double load_factor;
        //vector of vector of HashNode
        vector<vector<HashNode<V>>> table;
        int hash(const QString &key) const {
        unsigned int hash_value = 0;

        for (int i = 0; i < key.length(); i++) {
            hash_value = hash_value * 31 + key[i].unicode();
        }   

             return hash_value % capacity;
        }
        void rehash()
        {
            int old_capacity=capacity;
            capacity*=2;
            vector<vector<HashNode<V>>> new_table(capacity);
            for(int i=0;i<old_capacity;i++)
            {
                for(auto &node:table[i])
                {
                    int index=hash(node.key);
                    new_table[index].push_back(node);
                }
            }
            table=new_table;
        }
    public:
        HashTable(int capacity=16,double load_factor=0.75):capacity(max(capacity,4)),count(0),load_factor(load_factor){
            table.resize(capacity);
        }
        void clear()
        {
            count=0;
            table.assign(capacity,vector<HashNode<V>>());
        }
        void put(const QString &key,const V &value)
        {
            if(static_cast<double>(count)/capacity>=load_factor)
            {
                rehash();
            }
            int index=hash(key);
            for(auto &node:table[index])
            {
                if(node.key==key)
                {
                    node.value=value;
                    return;
                }
            }
            table[index].push_back(HashNode<V>(key,value));
            count++;
            
        }
        optional<V>get(const QString &key) const
        {
            int index=hash(key);
            for(const auto&node:table[index])
            {
                if(node.key==key)
                {
                    return node.value;
                }
            }
            return nullopt;

        }
        bool remove(const QString &key)
        {
            int index=hash(key);
            for(auto it=table[index].begin();it!=table[index].end();it++)
            {
                if(it->key==key){
                    table[index].erase(it);
                    count--;
                    return true;
                }
            }
            return false;
        }
        bool contains(const QString &key) const{
            int index=hash(key);
            for(const auto&node:table[index])
            {
                if(node.key==key)
                {
                    return true;
                }
            }
            return false;
        }
        vector<V> getAllValues() const{
            vector<V> values;
            for(int i=0;i<capacity;i++)
            {
                for(const auto&node:table[i])
                {
                    values.push_back(node.value);
                }
            }
            return values;
        }

};
#endif
