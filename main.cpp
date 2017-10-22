#include "Server.hpp"
#include "INIReader.h"

int main(int argc, char const* argv[]) {

    std::string ini_filename("server.ini");
    INIReader reader(ini_filename);

    if (reader.ParseError() < 0) {
        std::cout << "Can't load '" + ini_filename + "'\n";
        return 1;
    }

    MyCredentials c;
    c.host = reader.Get("mysql", "host", "127.0.0.1");
    c.port = reader.Get("mysql", "port", "3306");

    c.username = reader.Get("mysql", "user", "root");
    c.password = reader.Get("mysql", "pass", "");
    c.database = reader.Get("mysql", "db_name", "maplestory");

    Server s("192.168.11.144", "8888", c);
    s.run(10);

    return 0;
}