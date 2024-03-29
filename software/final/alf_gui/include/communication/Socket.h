//
// Created by christian on 18.11.18.
//

#ifndef ALF_GUI_SOCKET_H
#define ALF_GUI_SOCKET_H

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <vector>

const int MAXCONNECTIONS = 5;
const int MAXRECV = 500;

enum Commands {START_STREAM_MAP, STOP_STREAM_MAP, SAVE_MAP, RESET_MAP};

/** /CLASS Socket
 *
 * defines the basic functions for a server client connection
 *
 */
class Socket {

private:

    int m_sock;
    sockaddr_in m_addr;

public:

    Socket();
    virtual ~Socket();

    bool create();
    bool bind(const unsigned int port);
    bool listen() const;
    bool accept(Socket&) const;
    bool connect(const std::string host, const int port);
    void disconnect();

    bool send(const std::string) const;
    bool send(Commands&) const;

    int recv(std::string&) const;
    int recv(Commands&) const;
    int recv(std::vector<int>& v);

    bool isValid() const { return m_sock != -1;}
    void setNonBlocking(const bool);

};

#endif //ALF_GUI_SOCKET_H
