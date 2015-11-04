#ifndef _HYC_SSL_CONTEX_H_
#define _HYC_SSL_CONTEX_H_

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

struct HycSSLSocket {
    HycSSLSocket(SOCKET socket, SSL_CTX *ssl_ctx, bool isServer) {
        //创建当前连接的SSL结构体
        m_ssl = SSL_new(ssl_ctx);
        //将SSL绑定到套接字上
        SSL_set_fd(m_ssl, socket);
        //建立SSL链接
        if(isServer)
            SSL_accept(m_ssl);
        else
            SSL_connect(m_ssl);
        // 保存socket
        m_socket = socket;
        //打印所有加密算法的信息(可选)
        std::cout<< "SSL connection using" << SSL_get_cipher(m_ssl) << std::endl;
    }
    ~HycSSLSocket() {
        //断开SSL链接
        SSL_shutdown(m_ssl);
        //释放当前SSL链接结构体
        SSL_free(m_ssl);
        //断开TCP链接
        close(m_socket);
    }

    int Read(char* data, size_t maxLen) {
        return SSL_read(m_ssl, data, maxLen);
    }

    int Write(const char* data, size_t maxLen) {
        return SSL_write(m_ssl, data, maxLen);
    }

//private:
//    friend class HycSSLContex;
    SSL     *m_ssl;
    SOCKET  m_socket;
};


class HycSSLContex {
public:
    explicit HycSSLContex(bool isServer);
    ~HycSSLContex();

    int SetContex(const std::string &ca_verify_file_path,
                  const std::string &local_certificate_file_path,
                  const std::string &local_private_file_path);

    HycSSLSocket* CreateSSLSocket(SOCKET socket);

private:
    SSL_CTX        *m_ssl_ctx;
    bool            m_isServer;
};


#endif // _HYC_SSL_CONTEX_H_
