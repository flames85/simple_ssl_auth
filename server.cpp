/************server*************************/
#include <string.h>
#include <iostream>
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

char buffer[10001] = {0};

int main(int argc, char* argv[])
{
    SSL_library_init(); //初始化SSL库
    OpenSSL_add_all_algorithms(); //支持所有算法
    SSL_load_error_strings();  //提供将错误号解析为字符串的功能
    SSL *ssl = NULL;
    SSL_CTX *ssl_ctx = NULL;
    X509 *client_cert = NULL;
    //设置客户端使用的SSL版本
    //ssl_method = SSLv3_server_method();
    const SSL_METHOD *ssl_method = SSLv23_server_method();
    //创建SSL上下文环境 每个进程只需维护一个SSL_CTX结构体
    ssl_ctx = SSL_CTX_new(ssl_method);
    //验证对方
    SSL_CTX_set_verify(ssl_ctx,SSL_VERIFY_PEER,NULL);

    //若验证,则放置CA证书
    SSL_CTX_load_verify_locations(ssl_ctx, "certificate/ca-cert.pem", NULL);

    //设置pass phrase
    char password[1024] = "pass phrase";
    SSL_CTX_set_default_passwd_cb_userdata(ssl_ctx, password);
    //读取证书文件
    SSL_CTX_use_certificate_file(ssl_ctx,"certificate/server-cert.pem",SSL_FILETYPE_PEM);

    //读取密钥文件
    SSL_CTX_use_PrivateKey_file(ssl_ctx,"certificate/server-cert.key",SSL_FILETYPE_PEM);

    //验证密钥是否与证书一致
    SSL_CTX_check_private_key(ssl_ctx);

//    /*构建随机数生成机制,WIN32平台必需*/
//    srand( (unsigned)time( NULL ) );
//    for( int i = 0;    i < 100;i++ )
//        seed_int[i] = rand();
//    RAND_seed(seed_int, sizeof(seed_int));
    //设置加密方式
    //SSL_CTX_set_cipher_list(ssl_ctx,"RC4-MD5");
    //建立TCP服务器端、开始监听并接受客户端连接请求

    // 建立socket
    SOCKET server = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if ( server < 0 )
    {
        return -2;
    }

    // 绑定socket
    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = INADDR_ANY;
    service.sin_port = htons( 8899 );

    if ( bind( server, (struct sockaddr *) &service, sizeof(service) ) < 0 )
    {
        //失败
        close(server);
        return -3;
    }

    int opt = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt));

    // 监听 socket
    if ( listen( server, 10 ) < 0 )
    {
        //失败
        return -4;
    }

    SOCKET sClient;
    do
    {
        sClient = accept(server,NULL,NULL);
        sleep(2);
    }while(sClient < 0);

    //创建当前连接的SSL结构体
    ssl = SSL_new(ssl_ctx);
    //将SSL绑定到套接字上
    SSL_set_fd(ssl, sClient);

    //接受SSL链接
    SSL_accept(ssl);

    //获取和释放客户端证书
    client_cert = SSL_get_peer_certificate(ssl);

    //打印所有加密算法的信息(可选)
    std::cout<<"SSL connection using"<<SSL_get_cipher(ssl)<<std::endl;

    X509_free(client_cert);

    while(true)
    {
        SSL_read(ssl, buffer, 10000);
        printf("received: %s\n",buffer);
        memset(buffer,0,10000);
        SSL_write(ssl, "hello!I am server!", sizeof("hello!I am server!"));
        sleep(1);
    }

    //断开SSL链接
    SSL_shutdown(ssl);
    //释放当前SSL链接结构体
    SSL_free(ssl);
    //断开TCP链接
    close(sClient);
    //释放SSL上下文
    SSL_CTX_free(ssl_ctx);

    return 0;
}

