
#if defined(WIN32)

#include "Socket.h"

#include <windows.h>
#include <winsock2.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

size_t SocketBase::GetConnectedSocketsCount() const
{
  return s_read_.fd_count; 
}

size_t SocketBase::GetMaxAvailableSockets() const
{
  return FD_SETSIZE;
}

int SocketBase::SendData(SOCKET destSock, const char * data, size_t dataLen)
{
  return send(destSock, data, dataLen, 0);
}

void SocketBase::GetAcceptedSocketsThread()
{
  while (!this->_isClosed)
  {
    auto acceptedSock = accept(this->sock_, NULL, NULL);
    if (acceptedSock == INVALID_SOCKET)
    {
      continue;
    }

    FD_SET(acceptedSock, &s_read_);
    std::this_thread::yield();
  }
}

SocketBase::SocketBase()
{
  _isClosed = false;
  WSADATA wsaData;
  WSAStartup(MAKEWORD(2, 2), &wsaData);
}

SocketBase::~SocketBase()
{
  _isClosed = true;
  WSACleanup();
}

std::map<SOCKET, BytesData> SocketBase::Recv()
{
  std::map<SOCKET, BytesData> recvData;
  char recvBuff[SO_MAX_MSG_SIZE];

  // Sleep(1);
  fd_set fdset = s_read_;
  TIMEVAL tval; 
  tval.tv_usec = 1000;
  auto sockCount = select(0, &fdset, NULL, NULL, &tval);
  if (sockCount == 0 || sockCount == SOCKET_ERROR)
  {
    return recvData;
  }

  // for (auto sock : fdset.fd_array)
  //   if (FD_ISSET(sock, &fdset))
  //   {
  //     auto recvBytes = recv(sock, &recvBuff[0], SO_MAX_MSG_SIZE, MSG_PEEK);
  //     if (recvBytes <= 0)
  //       return recvData;

  //     memset((void*)&recvBuff[0], 0, SO_MAX_MSG_SIZE);
  //     recv(sock, &recvBuff[0], recvBytes, 0);
  //     recvData.emplace(sock, BytesData(recvBuff, recvBytes));
  //   }
  return recvData;
}

void SocketBase::Close()
{
  shutdown(sock_, SD_BOTH);
  closesocket(sock_);
}

TCPSocketSrv::TCPSocketSrv()
{
}

TCPSocketSrv::~TCPSocketSrv()
{
}

bool TCPSocketSrv::Open(unsigned short port, const char* ip)
{
  ADDRINFO *resultAddr;
  ADDRINFO localaddr_;
  ZeroMemory(&localaddr_, sizeof(localaddr_));
  localaddr_.ai_family = AF_INET;
  localaddr_.ai_socktype = SOCK_STREAM;
  localaddr_.ai_protocol = IPPROTO_TCP;
  localaddr_.ai_flags = AI_PASSIVE;

  char portValue[6];
  _itoa_s(port, portValue, 10);

  getaddrinfo(NULL, portValue, &localaddr_, &resultAddr);

  sock_ = socket(resultAddr->ai_family, resultAddr->ai_socktype, resultAddr->ai_protocol);
  if (sock_ == INVALID_SOCKET)
    return false;

  u_long nonBlockMode = 1;
  if (ioctlsocket(sock_, FIONBIO, &nonBlockMode) != NO_ERROR)
    return false;

  if (bind(sock_, resultAddr->ai_addr, (int)resultAddr->ai_addrlen) == SOCKET_ERROR)
    return false;

  if (listen(sock_, SOMAXCONN) == SOCKET_ERROR)
    return false;

  freeaddrinfo(resultAddr);

  FD_ZERO(&s_read_);
  FD_SET(sock_, &s_read_);

  _acceptThread = new std::thread(&TCPSocketSrv::GetAcceptedSocketsThread, this);
  return true;
}

int TCPSocketSrv::Send(SOCKET destSock, const char * data, size_t dataLen)
{
  return SendData(destSock, data, dataLen);
}

TCPSocketClt::TCPSocketClt()
{
}

TCPSocketClt::~TCPSocketClt()
{
}

bool TCPSocketClt::Open(unsigned short port, const char* ip)
{
  ADDRINFO *resultAddr;
  ADDRINFO localaddr_;
  ZeroMemory(&localaddr_, sizeof(localaddr_));
  localaddr_.ai_family = AF_INET;
  localaddr_.ai_socktype = SOCK_STREAM;
  localaddr_.ai_protocol = IPPROTO_TCP;

  char portValue[6];
  _itoa_s(port, portValue, 10);

  getaddrinfo(NULL, portValue, &localaddr_, &resultAddr);

  for (auto ptr = resultAddr; ptr != NULL; ptr = ptr->ai_next) {

    sock_ = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    if (sock_ == INVALID_SOCKET)
      return false;

    if (connect(sock_, ptr->ai_addr, (int)ptr->ai_addrlen) == SOCKET_ERROR)
    {
      sock_ = SOCKET_ERROR;
      continue;
    }

    break;
  }

  if (sock_ == SOCKET_ERROR)
    return false;

  FD_ZERO(&s_read_);
  FD_SET(sock_, &s_read_);
  freeaddrinfo(resultAddr);
  _acceptThread = new std::thread(&TCPSocketClt::GetAcceptedSocketsThread, this);
  return true;
}

int TCPSocketClt::Send(const char * data, size_t dataLen)
{
  return SendData(sock_, data, dataLen);
}
#endif
