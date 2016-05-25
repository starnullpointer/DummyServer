#include <iostream>
#include <string>
#include <cstring>
#include <list>
#include <SFML/Network.hpp>
#include <sstream>
#include <vector>
#include "Server_Class/server.h"


using namespace std;


int getNum(vector<string>& v);


//3141
int main() {

    Server server;

    while (true) {
        server.Update();
    }
}


int getNum(vector<string>& v)
{
    int ret=atoi((v.front()).c_str());
    v.erase(v.begin());
    return ret;
}
