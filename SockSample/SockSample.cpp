// SockSample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Socket.h"
#include <iostream>
int main()
{
  SOCKET srcSock;
  size_t dataLen;
  const char* data;

  TCPSocketSrv srv;
  TCPSocketClt clt;
  TCPSocketClt clt2;

  if (!srv.Open(9999, NULL))
  {
    std::cout << "srv.Open() error" << std::endl;
    system("pause");
    return 1;
  }
  if (!clt.Open(9999, NULL))
  {
    std::cout << "clt.Open() error" << std::endl;
    system("pause");
    return 1;
  }
  if (!clt2.Open(9999, NULL))
  {
    std::cout << "clt2.Open() error" << std::endl;
    system("pause");
    return 1;
  }

  auto sended = clt.Send("Test", 4);
  std::cout << "Client sended: Test" << std::endl;
  std::cout << "Sended bytes: " << sended << std::endl;

  data = srv.Recv(srcSock, dataLen);
  std::cout << "Server recieved: " << data << std::endl; 
  data = srv.Recv(srcSock, dataLen);
  std::cout << "Server recieved: " << data << std::endl;

  sended = clt2.Send("Test2", 5);
  std::cout << "Client sended: Test2" << std::endl;
  std::cout << "Sended bytes: " << sended << std::endl;

  data = srv.Recv(srcSock, dataLen);
  std::cout << "Server recieved: " << data << std::endl;
  data = srv.Recv(srcSock, dataLen);
  std::cout << "Server recieved: " << data << std::endl;


  sended = srv.Send(srcSock, "Hello", 5);
  std::cout << "Server sended: Hello" << std::endl;
  std::cout << "Sended bytes: " << sended << std::endl;

  data = clt.Recv(srcSock, dataLen);
  std::cout << "Client recieved: " << data << std::endl;
  data = clt2.Recv(srcSock, dataLen);
  std::cout << "Client2 recieved: " << data << std::endl;

  sended = srv.Send(srcSock, "Hello2", 6);
  std::cout << "Server sended: Hello2" << std::endl;
  std::cout << "Sended bytes: " << sended << std::endl;

  data = clt.Recv(srcSock, dataLen);
  std::cout << "Client recieved: " << data << std::endl;
  data = clt2.Recv(srcSock, dataLen);
  std::cout << "Client2 recieved: " << data << std::endl;

  clt.Close();
  clt2.Close();
  srv.Close();

  sended = clt.Send("asd", 3);
  std::cout << "Sended bytes: " << sended << std::endl;

  system("pause");
  return 0;
}

