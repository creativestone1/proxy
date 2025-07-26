#pragma once
#include <vector>
#include <functional>
#include <string>
#include <netinet/in.h>  // sockaddr_in
#include <sys/socket.h>  // socket functions

// Auxiliary types and constants used in the implementation

// Event handler type called upon connection events
using ConnectionUpEventHandler = std::function<void(const struct ConnectionInfo&)>;

// Maximum number of simultaneous connections (used in listen)
constexpr int MAX_CONNECTIONS_COUNT = 128;

// Types of connection retry strategies
enum TryType {
    TRY_ONCE,
    TRY_MULTIPLE_TIMES,
	TRY_INFINITE
};

// Configuration for a connection endpoint (client side)
struct ConnectConfig {
    std::string remoteIp;       // Remote IP address
    uint16_t remotePort;        // Remote port number
    uint16_t localPort;         // Local port number (0 for any)
    int connectionsCount;       // Number of allowed simultaneous connections
    int connectionTryTimes;     // Number of connection retry attempts
    TryType tryType;            // Retry strategy
};

// Configuration for a host endpoint (server side)
struct HostConfig {
    uint16_t localPort;         // Local port to listen on
    int connectionsCount;       // Maximum number of queued connections
};

// Information about a connection passed to event handlers
struct ConnectionInfo {
    int sockFd;                 // Socket file descriptor
    sockaddr_in remoteAddr;     // Remote socket address info
    sockaddr_in localAddr;      // Local socket address info
};

// Internal handler for connection endpoints
struct ConnectConfigHandler {
    sockaddr_in remoteAddr;     // Remote address initialized for connect
    sockaddr_in localAddr;      // Local address initialized for bind
    int lastConnections;        // Remaining allowed connections
    int lastConnectTries;       // Remaining connection retry attempts
    int sockFd;                 // Socket file descriptor
    TryType tryType;            // Retry strategy
};

// Internal handler for host endpoints
struct HostConfigHandler {
    sockaddr_in localAddr;      // Local address bound to socket
    int lastConnections;        // Remaining allowed connections
    int sockFd;                 // Listening socket descriptor
};

class ConnectionCreator {
public:
    // Subscribe to the event triggered upon successful connection
    void connectEventSubscribe(const ConnectionUpEventHandler& connectEvent);

    // Subscribe to the event triggered upon successful accept
    void acceptEventSubscribe(const ConnectionUpEventHandler& acceptEvent);

    // Add a connection endpoint (client configuration)
    int addConnectEndpoint(ConnectConfig conCfg);

    // Add a host endpoint (server configuration)
    int addHostEndpoint(HostConfig hostCfg);

    // Attempt to establish connections for all connect endpoints
    int tryConnect();

    // Attempt to accept incoming connections on the given sockets
    int tryAccept(std::vector<int>& sockets);

private:
    // List of active connection endpoints (client side)
    std::vector<ConnectConfigHandler*> socketsConnect;

    // List of active host endpoints (server side)
    std::vector<HostConfigHandler*> socketsHost;

    // Event handler called on successful connection
    ConnectionUpEventHandler connectEventHandler;

    // Event handler called on successful accept
    ConnectionUpEventHandler acceptEventHandler;
};


