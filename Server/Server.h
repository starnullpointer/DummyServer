#ifndef SERVER_H
#define SERVER_H
#include <iostream>
#include <string>
#include <cstring>
#include <list>
#include <SFML/Network.hpp>
#include <sstream>
#include <vector>
#include "../CarProjCommon/CarProjCommon.h"

using namespace std;

void dummy();

class Server
{
    public:
    Server();

    void Init();
    void ProcessEvents();
    void Update();
    void Render();
    void StartServer();
    void HandleIncomingConnection();
    void HandleCarData();
    Packet HandlePositionReq(sf::TcpSocket * car);
    Packet HandleDestReq(sf::TcpSocket * car);
    void PrintPacket(const Packet & printMe);
    void ErrorHandling(sf::Socket::Status checking);

    sf::IpAddress ip;
    string id;

    // camera
    int camera_port;
    sf::TcpListener listener_camera;
    sf::TcpSocket *camera_socket;

    // car
    int car_port;
    list<sf::TcpSocket*> cars;
    sf::TcpListener listener_car;
    sf::SocketSelector selector_car;
};


#endif // SERVER_H
