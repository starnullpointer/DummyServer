#ifndef SERVER_H
#define SERVER_H
#include <iostream>
#include <string>
#include <cstring>
#include <list>
#include <SFML/Network.hpp>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <SFML/Graphics.hpp>
#include "vector"
#include "../CarProjCommon/CarProjCommon.h"

using namespace std;

class Server
{
public:
    Server();

//    void Init();
//    void ProcessEvents();
    void Update();
    void Render();
    void StartServer();
    void HandleIncomingConnection();
    void HandleCarData();
    Packet HandlePositionReq(sf::TcpSocket * car);
    Packet HandleDestReq(sf::TcpSocket * car);
    void PrintPacket(const Packet & printMe);
    void ErrorHandling(sf::Socket::Status checking);
    void checkEvent();

    //sfml drawing
    void Initial_Window();
    void Draw();
    void update_graphics();
    void ProcessEventsForDrawing();
    void update_drawing();
    void render();
    sf::RenderWindow window;
    sf::CircleShape newdot;
    vector<sf::CircleShape> dots;
    sf::CircleShape destdot;
    sf::Event event;
    bool isEvent;

private:
    sf::IpAddress ip;
    string id;
    // camera
    int camera_port;
    sf::TcpListener listener_camera;
    sf::TcpSocket *camera_socket;
    sf::SocketSelector selector_camera; //remove

    // car
    int car_port;
    list<sf::TcpSocket*> cars;
    sf::TcpListener listener_car;
    sf::SocketSelector selector_car;

    double x;
    double y;
    double x_dest;
    double y_dest;
};




#endif // SERVER_H
