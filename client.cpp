/**************client*********************/
#include <iostream>
#include <string.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <openssl/rand.h>
#include <openssl/err.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int SOCKET;

int main(int argc, char* argv[])
{
    SOCKET client;
    char buffer[256] = {0};

    SSL *ssl = NULL;
    SSL_CTX *ssl_ctx = NULL;
    X509 *server_cert = NULL;

    SSL_library_init();        //init libraries
    OpenSSL_add_all_algorithms(); //支持所有算法
    SSL_load_error_strings();  //提供将错误号解析为字符串的功能

    //设置客户端使用的SSL版本
    const SSL_METHOD * ssl_method = SSLv3_client_method();
    //创建SSL上下文环境 每个进程只需维护一个SSL_CTX结构体
    ssl_ctx = SSL_CTX_new(ssl_method);

    /* Load the RSA CA certificate into the SSL_CTX structure */
    /* This will allow this client to verify the server's   */
    /* certificate.                             */
    SSL_CTX_load_verify_locations(ssl_ctx, "certificate/ca-cert.pem", NULL);

    /* Set flag in context to require peer (server) certificate verification */
    SSL_CTX_set_verify(ssl_ctx,SSL_VERIFY_PEER,NULL);
    SSL_CTX_set_verify_depth(ssl_ctx,1);

    char password[1024] = "pass phrase";
    SSL_CTX_set_default_passwd_cb_userdata(ssl_ctx, password);
    //读取证书文件
    SSL_CTX_use_certificate_file(ssl_ctx,"certificate/client-cert.pem",SSL_FILETYPE_PEM);
    //读取密钥文件
    SSL_CTX_use_PrivateKey_file(ssl_ctx,"certificate/client-cert.key",SSL_FILETYPE_PEM);
    //验证密钥是否与证书一致
    SSL_CTX_check_private_key(ssl_ctx);

    // 建立socket socket.
    client = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if ( client < 0 )
    {
      //AfxMessageBox("Error at socket!");
      return -1;
    }

    // 连接到服务器.
    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr( "127.0.0.1" );
    clientService.sin_port = htons( 8899 );
    if(connect( client, (struct sockaddr *) &clientService, sizeof(clientService) ) < 0)
    {
       return -1;
    }

    //创建维护当前连接信息的SSL结构体
    ssl = SSL_new(ssl_ctx);
    //将SSL绑定到套接字上
    SSL_set_fd(ssl, client);
    //建立SSL链接
    SSL_connect(ssl);

    //获取服务器端证书
    server_cert = SSL_get_peer_certificate(ssl);

    //释放服务器端证书
    X509_free(server_cert);

    while(true)
    {
        SSL_write(ssl, "hello! I am client!", sizeof("hello! I am client!"));
        SSL_read(ssl, buffer, 255);
        printf("received: %s\n",buffer);
        memset(buffer,0,255);
        sleep(1);
     }
    //断开SSL链接
    SSL_shutdown(ssl);
    //释放当前SSL链接结构体
    SSL_free(ssl);
    close(client);
    //释放上下文
    SSL_CTX_free(ssl_ctx);

    return 0;
}


