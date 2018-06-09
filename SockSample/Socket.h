
#if !defined(SOCKETCLASS_H)
#define SOCKETCLASS_H

#include "BytesData.h"
#include <map>

#if defined(WIN32)
#include <WS2tcpip.h>
#elif defined(LINUX)
#include <set>
#include <sys/socket.h>
#define SOCKET int
#endif

class SocketBase
{
protected:
#if defined(LINUX)
  std::set<SOCKET> acceptedSockets_;
#endif
  SOCKET sock_;
  fd_set s_read_;
  int SendData(SOCKET destSock, const char *data, size_t dataLen);
  fd_set GetAcceptedSockets(SOCKET sock);

public:
  SocketBase();
  virtual ~SocketBase();
  virtual bool Open(unsigned short port, const char *ip = NULL) = 0;
  virtual std::map<SOCKET, BytesData> Recv();
  virtual void Close();
};

class TCPSocketSrv : public SocketBase
{
public:
  TCPSocketSrv();
  ~TCPSocketSrv();
  bool Open(unsigned short port, const char *ip = NULL);
  int Send(SOCKET destSock, const char *data, size_t dataLen);
};

class TCPSocketClt : public SocketBase
{
public:
  TCPSocketClt();
  ~TCPSocketClt();
  bool Open(unsigned short port, const char *ip = NULL);
  int Send(const char *data, size_t dataLen);
};

#endif