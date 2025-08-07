#pragma once
#include <vector>
#include <functional>
#include <string>
#include "Logger.h"
#include "SockUtils.h"

// Maximum number of simultaneous connections (used in listen)
constexpr int MAX_CONNECTIONS_COUNT = 1024;

// Types of connection retry strategies
enum TryType {
    TRY_ONCE,
    TRY_MULTIPLE_TIMES,
	TRY_INFINITE
};

// Configuration for a connection endpoint (client side)
struct ConnectConfig {
    std::string remoteIp;			// Remote IP address
    uint16_t remotePort;			// Remote port number
    uint16_t localPort = 0;			// Local port number (0 for any)
    int connectionTryTimes = 0;		// Number of connection retry attempts (0 if tryType == TRY_INFINITE)
    TryType tryType = TRY_INFINITE;	// Retry strategy
};

// Configuration for a host endpoint (server side)
struct AcceptConfig {
    uint16_t localPort;         // Local port to listen on
    int connectionsCount;       // Maximum number of queued connections
};

// Information about a connection passed to event handlers
struct ConnectionInfo {
    int sockFd;                 // Socket file descriptor
    sockaddr_in remoteAddr;     // Remote socket address info
    sockaddr_in localAddr;      // Local socket address info
};

// Event handler type called upon connection events
using ConnectionUpEventHandler = std::function<void(ConnectionInfo*)>;

class ConnectionCreator {
protected:
	// Internal handler for connection endpoints
	struct InternalConnectConfig {
		sockaddr_in remoteAddr;     // Remote address initialized for connect
		sockaddr_in localAddr;      // Local address initialized for bind
		int lastConnections;        // Remaining allowed connections
		int lastConnectTries;       // Remaining connection retry attempts
		int sockFd;                 // Socket file descriptor
		TryType tryType;            // Retry strategy
	};

	// Internal handler for host endpoints
	struct InternalAcceptConfig {
		sockaddr_in localAddr;      // Local address bound to socket
		int lastConnections;        // Remaining allowed connections
		int sockFd;                 // Listening socket descriptor
	};

    std::vector<InternalConnectConfig*> _socketsConnect;
    std::vector<InternalAcceptConfig*> _socketsAccept;

    ConnectionUpEventHandler _connectEventHandler;
    ConnectionUpEventHandler _acceptEventHandler;
public:
    void connectEventSubscribe(const ConnectionUpEventHandler& connectEvent);
    void acceptEventSubscribe(const ConnectionUpEventHandler& acceptEvent);
    
	int addConnectEndpoint(ConnectConfig& conCfg);
    int addAcceptEndpoint(AcceptConfig& hostCfg);
   
	int tryConnect();
    int tryAccept(std::vector<int>& sockets);
	std::vector<int> getAcceptSockets();
};


