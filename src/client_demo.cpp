﻿#include "hyc_ssl_contex.h"

#ifdef WIN32
#include <string.h>
#pragma comment (lib,"WS2_32.lib")
#define SLEEP(t) Sleep(t*1000)
#else
// linux
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#define SLEEP(t) sleep(t)
#endif

int main(int argc, char* argv[])
{
    try
    {
        //! 创建SSL环境
        HycSSLContex contex(false);

        std::string ca_verify_file_path = "certificate/ca-cert.pem";
        std::string local_certificate_file_path = "certificate/client-cert.pem";
        std::string local_private_file_path = "certificate/client-cert.key";

        if(contex.SetContex(ca_verify_file_path,
                            local_certificate_file_path,
                            local_private_file_path,
                            "RC4-MD5") < 0 )
        {
            std::cerr << "set contex fail" << std::endl;
            return -1;
        }

        /***************************************************************************/
        // 以下是正常的socket连接流程
#ifdef WIN32
        // 初始化 Winsock.
        WSADATA wsaData;
        int iResult = WSAStartup( MAKEWORD(2,2), &wsaData );
        if ( iResult != NO_ERROR )
        {
            //失败
            return -2;
        }
#endif
        SOCKET clientFd = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
        if(clientFd < 0)
        {
            CloseSocket(clientFd);
            return -3;
        }
        sockaddr_in clientService;
        clientService.sin_family = AF_INET;
        clientService.sin_addr.s_addr = inet_addr("127.0.0.1");
        clientService.sin_port = htons( 8899 );
        if(connect(clientFd, (struct sockaddr *) &clientService, sizeof(clientService)) < 0)
        {
            CloseSocket(clientFd);
            return -4;
        }
        /***************************************************************************/

        //! 生成ssl socket
        HycSSLSocket *sslSocket = contex.CreateSSLSocket(clientFd);
        if(!sslSocket) {
            CloseSocket(clientFd);
            return -5;
        }

        std::cout << "peer certificate subject: " << sslSocket->m_subjectName.c_str() << std::endl;
        std::cout << "peer certificate issuer : " << sslSocket->m_issuerName.c_str() << std::endl;

        //! 开始ssl通信
        char buffer[1024] = {0};
        while(1)
        {
            // write
            if(sslSocket->Write("I AM CLIENT", sizeof("I AM CLIENT")) <= 0) break;
            // read
            if(sslSocket->Read(buffer, 1023) <= 0) break;

            std::cout << "receive:" << buffer << std::endl;
            SLEEP(1);
            memset(buffer, 0x0, 1024);
        }
        // 析构
        if(sslSocket) {
            delete sslSocket;
            sslSocket = NULL;
        }
        return 0;
    }
    catch (std::exception& e)
    {
        std::cerr << "[main]Exception: " << e.what() << "\n";
    }

}


