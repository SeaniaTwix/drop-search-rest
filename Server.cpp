//
// Created by Luria on 2017. 10. 11..
//

#include "Server.hpp"

#include <iostream>
#include <sstream>

#include <served/uri.hpp>
#include <mysql++11/mysql++11.h>
#include <boost/format.hpp>

#define TAB "    "

static std::string getFirstWord(std::string origin) {
    std::istringstream stream(origin);
    std::string res;

    while (getline(stream, res, '.')) {
        // Only returns first element
        return res;
    }

    return res;
}

Server::Server(std::string ip, std::string port, MyCredentials crd)
        : server(ip, port, mux) {
    auto itemData = getMap();

    std::map<std::string, std::string> mob;
    for (auto it : itemData["Mob"]) {
        mob.emplace(std::pair<std::string, std::string>(std::to_string(it.second), it.first));
    }

    mux.handle("/get/code/{item_type}").get([=](served::response & res, const served::request & req) {
        auto type = req.params["item_type"];
        auto itemName = req.query["name"];

        if (itemName.length() <= 0) {
            res << "{ result: \"require query\" }";
            return;
        }

        int itemCode = 0;

        try {
            itemCode = itemData.at(type).at(itemName);
        } catch (std::exception &e) {
            std::cout << e.what() << std::endl;
            res << " { result: \"error\" }";

            return;
        }

        res << "{ result: " + std::to_string(itemCode) + "}" << "\n";
    });

    mux.handle("/get/dropper").get([=](served::response &res, const served::request &req) {
        if (req.query["get"] == "name") {
            std::string mobCode = req.query["code"];

            if (mobCode.length() <= 0) {
                res << "{ result: \"not_enough_query\" }";
                return;
            }

            std::cout << mobCode << std::endl;
            //auto mobName = mob[mobCode];
            res << "{ result: \"" << mob.at(mobCode) << "\" }";
        } else if (req.query["get"] == "monsters") {
            using namespace daotk::mysql;

            connection mysql(crd.host, crd.username, crd.password, crd.database);

            if (mysql) {
                //std::cout << req.params["code"] << std::endl;
                auto sql = boost::format("select dropperid from drop_data where itemid=%1%") %
                           req.query["code"];
                //std::cout << sql << std::endl;
                auto result = mysql.query(sql.str());

                if (result.is_empty()) {
                    res << "{ result: \"not_found\" }";
                    return;
                }

                res << "{\n"
                    << std::string(TAB) + "result: [";

                for (auto d : result.as_container<int>()) {
                    int droppper_id;
                    std::tie(droppper_id) = d;

                    auto _result = boost::format("{ dropper: %1% }") % droppper_id;

                    res << std::string(TAB) << std::string(TAB) << boost::str(_result);
                    res << ",";
                }

                res << std::string(TAB) << "]";
                res << "}";
            } else {
                res << "Database connection failed";
            }

            mysql.close();
        } else {
            // require parameter at least more than one for this handle
            res << " { result: \"not_defined_query\" }";
        }
    });
}

std::map<std::string, ItemData> Server::getMap() {
    std::map<std::string, ItemData> itm;

    auto all_data = loadXMLs();

    for (auto &data : all_data) {
        pugi::xml_document doc;
        // xml_path is validated path. no require to check
        doc.load_file(data.xml_path.c_str());

        auto mainNode = doc.child(MAPLESTORY_XML_NODE);

        for (pugi::xml_node itemNode : mainNode.children(MAPLESTORY_XML_NODE)) {
            std::string itemName;
            int itemCode;

            // std::cout << data.name << ":::";

            for (pugi::xml_node itemInfo : itemNode.children()) {
                for (auto c : itemInfo.attributes()) {
                    const std::string val = c.value();
                    if (val == "name") {
                        itemName = c.next_attribute().value();
                        // std::cout << itemName << " : ";
                    }
                }
            }

            for (pugi::xml_attribute attr : itemNode.attributes()) {
                std::string code = attr.value();
                try {
                    itemCode = std::stoi(code);
                    // std::cout << itemCode << std::endl;
                } catch (std::exception& e) {
                    continue;
                }

            }

            itm[data.name][itemName] = itemCode;
        }
    }

    return itm;
}

std::vector<XMLDoc> Server::loadXMLs() {
    boost::filesystem::path p("./wz/String.wz");
    boost::filesystem::directory_iterator end;
    std::vector<XMLDoc> docs;

    for (boost::filesystem::directory_iterator itr(p); itr != end; ++itr) {
        if (boost::filesystem::is_regular_file(itr->path()) && (itr->path().filename().string() != ".DS_Store")) {
            std::string current_file = itr->path().string();
            std::string current_filename = itr->path().filename().string();
            std::string id = getFirstWord(current_filename);

            pugi::xml_document doc;
            pugi::xml_parse_result result = doc.load_file(current_file.c_str());

            if (result) {
                //std::cout << id << ":" << current_file << std::endl;
                XMLDoc d { id, current_file };
                docs.push_back(d);
            }
        }
    }

    std::cout << "All of XML Files loaded on memory..." << std::endl;

    return docs;
}

void Server::run(int&& thCount) {
    server.run(thCount);
}
