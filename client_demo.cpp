#include "hyc_ssl_contex.h"

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
                            local_private_file_path) < 0 )
        {
            std::cerr << "set contex fail" << std::endl;
        }


        // 以下是正常的socket连接流程
        SOCKET clientFd = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
        if ( clientFd < 0 )
        {
          return -1;
        }
        sockaddr_in clientService;
        clientService.sin_family = AF_INET;
        clientService.sin_addr.s_addr = inet_addr("127.0.0.1");
        clientService.sin_port = htons( 8899 );
        if(connect(clientFd, (struct sockaddr *) &clientService, sizeof(clientService)) < 0)
        {
            return -1;
        }


        //! 生成ssl socket
        HycSSLSocket *sslSocket = contex.CreateSSLSocket(clientFd);
        char buffer[1024] = {0};

        //! 开始ssl通信
        while(1)
        {
            // write
            if(contex.Write(sslSocket, "I AM CLIENT", sizeof("I AM CLIENT")) <= 0)
            {
                break;
            }
            // read
            if(contex.Read(sslSocket, buffer, 1023) <= 0)
            {
                break;
            }
            std::cout << "receive:" << buffer << std::endl;

            sleep(1);
            memset(buffer, 0x0, 1024);
        }

        return 0;
    }
    catch (std::exception& e)
    {
        std::cerr << "[main]Exception: " << e.what() << "\n";
    }

}


