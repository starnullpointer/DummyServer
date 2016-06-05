#include "server.h"

Server::Server()
{
    cout<<"Server () Top"<<endl;
    camera_socket = new sf::TcpSocket;
    ip = sf::IpAddress::getLocalAddress();
    id = "server";

    cout<<"Ip: "<<ip<<endl; //mphone on m's laptop: 192.168.43.249

    // init camera
    camera_port = 2718;  // changed to test
    // bind a listener to port
    cout<<"past cam port"<<endl;
    if(listener_camera.listen(camera_port) != sf::Socket::Done){
        // error
        cout<<"Server() error: listen camera_port failed"<<endl;
    }
    if(listener_camera.accept(*camera_socket) != sf::Socket::Done){
        //error
        cout<<"Server() error: Accept camera_socket failed"<<endl;
    }
    selector_camera.add(*camera_socket);

    cout<<"cam connection made"<<endl;
    // init car
    car_port = 2719;
    listener_car.listen(car_port);
    //figure out selector_camera
    selector_car.add(listener_car);
    cout<< "listener for car added"<<endl;

}

void Server::Update() {
    // waits around until at least of its sockets has data available
    // and will return true when it does. can also use a timeout arg
    if (selector_car.wait()) {
        // see if the listener is ready...
        if (selector_car.isReady(listener_car)) {
            // if the listner is ready, theres an incoming connection!
            HandleIncomingConnection();
        }
        else {
            // so if the listen socket isn't ready, that means one of our
            // clients are.
            HandleCarData();
        }
    }
}


void Server::HandleIncomingConnection(){
    sf::TcpSocket* client = new sf::TcpSocket;
    // try to attach our new socket to this new connection...
    if (listener_car.accept(*client) == sf::Socket::Done) {
        cout << "accepting socket...." << endl;
        // add our new client to our client list
        cars.push_back(client);

        // also add our client to the socketselector so we can be
        // notified when something is sent
        selector_car.add(*client);
        cout << "Connection made!" << endl;
    }
    else {
        // so... error cuz we couldn't make a new connection, so del socket
        delete client;
    }
}

void Server::HandleCarData(){
    for (list<sf::TcpSocket*>::iterator it=cars.begin(); it!=cars.end(); ++it) {
        // we need a reference... not a copy... sockets can't be copied
        // anyway
        sf::TcpSocket& client = **it;
        if (selector_car.isReady(client)) {
            char buff[200];
            int size;
            size_t received_size;
            sf::Socket::Status status = client.receive(buff, sizeof(buff), received_size);
            if (status == sf::Socket::Done) {
                buff[received_size] = '\0';
                cout << "message: " << buff << endl;
                
                //deal with the receiving message
                Packet recieved(buff);
                Packet response;
                cout<<"----recieved-----"<<endl;
                PrintPacket(recieved);
                cout<<"-----------------"<<endl<<endl<<endl;
                cout<<recieved.Message<<endl;
                if (strcmp(recieved.Message, CAR_CON_REQ_POS) == 0){
                    cout<<"in Handle Position"<<endl;
                    response = HandlePositionReq(*it);
                }
                else if (strcmp(recieved.Message, CAR_CON_REQ_DEST) == 0){
                    cout<<"in handle dest"<<endl;
                    response = HandleDestReq(*it);
                }
                else{
                    cout<< "Error: Invalid Packet Sent"<<endl;
                }

                cout<<"Message we are sending to car: "<<response.Message<<endl;
                //response
                client.send(response.Message, strlen(response.Message) + 1);

            }
            // need to make other case for status (like disconnected, partcial...)
            else {
                cout << "error" << endl;
                // otherwise... an error occured, so del the socket
                // first deallocate it
                delete *it;
                // them remove from our socket list
                cars.erase(it--);
                // and remove the socket from the selector`
                selector_car.remove(client);

            }
        }
    }
}


Packet Server::HandlePositionReq(sf::TcpSocket * car) {
    //car turn on light
    Packet p;
    char received[101];
    char cam_received [101] = "{ no message }";
    size_t size;
//    int error;
    sf::Socket::Status checking;


    // ****DEAL WITH ALL OF THE ERRORS WHEN PUT INTO SEPARATE FUNCTIONS****
    // if error return error packet


    checking = car->send(CON_CAR_TURN_LIGHT_ON, strlen(CON_CAR_TURN_LIGHT_ON)+1);
    ErrorHandling(checking);
    //wait for car to say light on

    checking = car->receive(received, 100, size);  //deal with error
    received[size] = '\0';
    ErrorHandling(checking);
    cout<<"before pacect assignment"<<endl;

    p = Packet(received);
    cout<<"1 HandleLocationRequest: after sending: "<<CON_CAR_TURN_LIGHT_ON<<" recieved: "<<p.Command<<endl;

    if(strcmp(p.Command, COMM_LIGHT_IS_ON) == 0){
        cout<<"2 HandleLocationRequest: recieved: "<<COMM_LIGHT_IS_ON<<endl;
        //ask camera for location of car

        checking = camera_socket->send(CON_CAM_REQ_POS, strlen(CON_CAM_REQ_POS)+1);
        cout<<"2 HandleLocationRequest: sent: "<<CON_CAM_REQ_POS<<endl;
        ErrorHandling(checking);


        selector_camera.wait();
        if (selector_camera.isReady(*camera_socket)){
            cout<<"selector cam is ready"<<endl;
        }



        //testing camera_socket
        if (camera_socket->isBlocking()){
            cout<<"****blocking"<<endl;
        }
        else{
            cout<<"******not blocking"<<endl;
        }
        cout<<"camera_sockect local port: "<<camera_socket->getLocalPort()<<endl;



        checking = camera_socket->receive(cam_received, 100, size);
        cam_received[size] = '\0';
        ErrorHandling(checking);

        cout<<"cam_received: "<<cam_received<<endl;
        cout<<"cam_received size: "<<strlen(cam_received)<<endl;
        cout<<"size: "<<size<<endl;


//        cout<<"stop here"<<endl;
//        cin.get();

        p = Packet(cam_received);
        cout<<"after Packer constructor: cam_received"<<endl;
        cout<<"p.message: "<<p.Message<<endl;

       if(strcmp(p.Command, COMM_HERE_IS_POS)==0){
            p.MakeMessage("Con", "Car", COMM_YOUR_POS);
            cout<<"4 HandleLocationRequest: after sending: "<<COMM_HERE_IS_POS<<endl;
            cout<<"Print P Message in HandlePosition funciton before return: "<<p.Message<<endl;
            return p;
       }
       else {
            cout<<"BAD RESPONSE FROM CAR  expected: HereIsPos  recieved: "<<p.Command<<endl;

        }

    }
    else {
        cout<<"BAD RESPONSE FROM CAR  expected: LightIsOn  recieved: "<<p.Command<<endl;
    }
    //wait for camera to give coords
    //tell car coords

}

Packet Server::HandleDestReq(sf::TcpSocket * car) {
    //car turn on light
    Packet p;
    char received[101];
    size_t size;
    sf::Socket::Status checking;


    // ****DEAL WITH ALL OF THE ERRORS WHEN PUT INTO SEPARATE FUNCTIONS****
    // if error return error packet

    char str[101];
    strcpy(str, CON_CAR_HERE_IS_DEST);
    strcat(str, "4056 4056 }");

    checking = car->send(str, strlen(str)+1);
    ErrorHandling(checking);
    //wait for car to say light on

//    checking = car->receive(received, 100, size);  //deal with error
//    received[size] = '\0';
//    ErrorHandling(checking);
//    cout<<"before pacect assignment"<<endl;

//    p = Packet(received);
//    cout<<"1 HandleLocationRequest: after sending: "<<CON_CAR_TURN_LIGHT_ON<<" recieved: "<<p.Command<<endl;


}

void Server::PrintPacket(const Packet & printMe){
    cout<<"Message: |"<<printMe.Message<<"|  Command: |"<<printMe.Command;
    cout<<"|  Sender: |"<<printMe.Sender<<"|  Recipient: |"<<printMe.Recipient<<"|  Other: |";
    for(int i = 0; i < printMe.otherUsed; i++){
        cout<<printMe.Other[i]<<"| |";
    }
    cout<<endl;
}

void Server::ErrorHandling(sf::Socket::Status checking){
    if(checking == sf::Socket::Done){
        cout<<"done"<<endl;
    }
    else if (checking == sf::Socket::NotReady){
        cout<<"not ready"<<endl;
    }
//    else if ( checking == sf::Socket::Partial){
//        cout<<"partial"<<endl;
//    }
    else if ( checking == sf::Socket::Disconnected){
        cout<<"disconnected"<<endl;
    }
    else{
        cout<<"error"<<endl;
    }
}

