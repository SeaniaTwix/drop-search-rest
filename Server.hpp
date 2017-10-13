//
// Created by Luria on 2017. 10. 11..
//

#pragma once
#include <served/served.hpp>
#include <boost/filesystem.hpp>
#include <pugixml.hpp>
#include <utility>

#define MAPLESTORY_XML_NODE "imgdir"

using MyCredentials = struct {
    std::string username;
    std::string password;
    std::string database;
    std::string host;
    std::string port;
};

using ItemData = std::map<std::string, int>;
using XMLDoc = struct {
    std::string name;
    std::string xml_path;
};

class Server {
public:
    Server(std::string ip, std::string port, MyCredentials c);
    void run(int&& thCount);
protected:
private:
    served::multiplexer mux;
    served::net::server server;

private:


private:
    std::vector<XMLDoc> loadXMLs();
    std::map<std::string, ItemData> getItemMap();
};


