#include "SockUtils.h"

void sock_utils::init_wsa() {
#ifdef _WIN32
  WSADATA wsa;
  WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
}

void sock_utils::setnonblocking(int socket) {
#ifdef _WIN32
  u_long mode = 1;
  ioctlsocket(socket, FIONBIO, &mode);
#else
  fcntl(socket, F_SETFL, O_NONBLOCK);
#endif
}

int sock_utils::reuse_addr(int socket, bool enable) {
  int optionValue = enable ? 1 : 0;
  return setsockopt(socket, SOL_SOCKET, SO_REUSEADDR,
                    (const void *)&optionValue, sizeof(optionValue)) < 0
             ? -1
             : 0;
}

void sock_utils::cleanup() {
#ifdef _WIN32
  WSACleanup();
#endif
}

void sock_utils::close_socket(int socket) {
#ifdef _WIN32
  closesocket(socket);
#else
  close(socket);
#endif
}

