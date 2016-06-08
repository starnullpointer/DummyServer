#include <iostream>
#include <string>
#include <cstring>
#include <list>
#include <SFML/Network.hpp>
#include <sstream>
#include <vector>
#include "../Server_Class/server.h"
using namespace std;

int getNum(vector<string>& v);

//3141
int main() {
    srand(time(0));
    Server server;
    server.Initial_Window();

    while (true) {
        server.update_graphics();
//        server.Update();
    }
}


int getNum(vector<string>& v)
{
    int ret=atoi((v.front()).c_str());
    v.erase(v.begin());
    return ret;
}
