#include "message.hpp"

#include "rapidjson/reader.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace std;
using namespace rapidjson;

Message::Message() {
    this->argument = Argument::NONE;
    this->data = Value("none");
    this->doc.SetObject();
}

Message::~Message() {

}

void Message::Clear() {

}

bool Message::parseJson(char *json) {
    ParseResult ok = doc.Parse((const char*)json);
    if(!ok)
        return false;
    
    if( !doc.HasMember("type") || !doc["type"].IsInt() ||
        !doc.HasMember("command") || !doc["command"].IsInt() ||
        !doc.HasMember("argument") || !doc["argument"].IsInt())
        return false;
    
    this->type = (Type)doc["type"].GetInt();
    this->command = (Command)doc["command"].GetInt();
    this->argument = (Argument)doc["argument"].GetInt();

    if(!doc.HasMember("data"))
        return false;

    this->data = doc["data"]; 

    return true;
}

void Message::setType(Type t) {
    type = t;
}

Message::Type Message::getType() {
    return type;
}

void Message::setCommand(Command c) {
    command = c;
}

Message::Command Message::getCommand() {
    return command;
}

void Message::setArgument(Argument a) {
    argument = a;
}

Message::Argument Message::getArgument() {
    return argument;
}

bool Message::getData(int& i) {
    if(!this->data.IsInt())
        return false;
    i = data.GetInt();
    return true;
}

bool Message::getData(float& f) {
    if(!this->data.IsFloat())
        return false;
    f = data.GetFloat();
    return true;
}

bool Message::getData(vector<node>& nodes) {
    if(!this->data.IsArray())
        return false;
    for (auto& v : this->data.GetArray()) {
        node node;
        if(!node.setJson(v))
            return false;
        nodes.push_back(node);
    }
    return true;
}

bool Message::getData(node& nodeA, vector<node>& nodesB) {
    if( !this->data.HasMember("A") || !this->data["A"].IsString() ||
        !this->data.HasMember("B") || !this->data["B"].IsArray())
        return false;

    if(!nodeA.setJson(this->data["A"]))
        return false;

    for (auto& v : this->data["B"].GetArray()) {
        node node;
        if(!node.setJson(v))
            return false;
        nodesB.push_back(node);
    }
    return true;
}

bool Message::getData(vector<node>& nodesA, vector<node>& nodesB) {
    if( !this->data.HasMember("A") || !this->data["A"].IsArray() ||
        !this->data.HasMember("B") || !this->data["B"].IsArray())
        return false;

    for (auto& v : this->data["A"].GetArray()) {
        node node;
        if(!node.setJson(v))
            return false;
        nodesA.push_back(node);
    }

    for (auto& v : this->data["B"].GetArray()) {
        node node;
        if(!node.setJson(v))
            return false;
        nodesB.push_back(node);
    }
    return true;
}

 bool Message::getData(Report& report) {
     return report.parseJson(this->data);
 }

void Message::setData(int i) {

    Value val(i);                                                                                                                                                                                                                                                                                                                                                                                                                           
    
    this->data = val;
}

void Message::setData(float f) {

    Value val(f);                                                                                                                                                                                                                                                                                                                                                                                                                             
    
    this->data = val;
}

void Message::setData(std::vector<node> nodes) {

    Value arr(kArrayType);
    Document::AllocatorType& allocator = doc.GetAllocator();
    
    for(auto node : nodes) {
        arr.PushBack(node.getJson(allocator), allocator);
    }                                                                                                                                                                                                                                                                                                                                                                                                                                                        
    this->data = arr;
    
}

void Message::setData(node nodeA, vector<node> nodesB) {

    Value obj(kObjectType);
    Value arrB(kArrayType);
    Document::AllocatorType& allocator = doc.GetAllocator();

    for(auto node : nodesB) {
        arrB.PushBack(node.getJson(allocator), allocator);
    }

    obj.AddMember("A", nodeA.getJson(allocator), allocator);
    obj.AddMember("B", arrB, allocator);
    this->data = obj;
}

void Message::setData(std::vector<node> nodesA, std::vector<node> nodesB) {

    Value obj(kObjectType);
    Value arrA(kArrayType);
    Value arrB(kArrayType);
    Document::AllocatorType& allocator = doc.GetAllocator();
    
    for(auto node : nodesA) {
        arrA.PushBack(node.getJson(allocator), allocator);
    }
    for(auto node : nodesB) {
        arrB.PushBack(node.getJson(allocator), allocator);
    }

    obj.AddMember("A", arrA, allocator);
    obj.AddMember("B", arrB, allocator);
    this->data = obj;
    if(data.IsNull()) {
        int a =3;
        a++;
    }
}

void Message::setData(Report& report) {
    this->data = *(report.getJson());
}

void Message::buildString() {
    doc.RemoveAllMembers();
    
    doc.AddMember("type", this->type, doc.GetAllocator());
    doc.AddMember("command", this->command, doc.GetAllocator());
    doc.AddMember("argument", this->argument, doc.GetAllocator());

    Value v;
    v.CopyFrom(this->data, doc.GetAllocator());
    doc.AddMember("data", v, doc.GetAllocator());
}

string Message::getString() {
    StringBuffer s;
    rapidjson::Writer<StringBuffer> writer (s);
    doc.Accept (writer);
    std::string str (s.GetString());
    return str;
}