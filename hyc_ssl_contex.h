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

class HycSSLContex;

struct HycSSLSocket {

    int Read(char* data, size_t maxLen) {
        return SSL_read(m_ssl, data, maxLen);
    }

    int Write(const char* data, size_t maxLen) {
        return SSL_write(m_ssl, data, maxLen);
    }

    explicit HycSSLSocket(SSL_CTX *ssl_ctx)
        :m_ssl(NULL),
         m_socket(-1)
    {
        //创建当前连接的SSL结构体
        m_ssl = SSL_new(ssl_ctx);
    }

    ~HycSSLSocket() {
        if(m_ssl)
        {
            //断开SSL链接
            SSL_shutdown(m_ssl);
            //释放当前SSL链接结构体
            SSL_free(m_ssl);
        }
        //断开TCP链接
        if(m_socket > 0)
            close(m_socket);
    }

private:
    int Accept(SOCKET socket) {
        // 保存socket
        if(socket < 0) return socket;
        m_socket = socket;

        //将SSL绑定到套接字上
        int ret = SSL_set_fd(m_ssl, m_socket);
        if(ret < 0) return ret;

        //建立SSL链接
        ret = SSL_accept(m_ssl);
        if(ret < 0) return ret;

        //打印所有加密算法的信息(可选)
        std::cout<< "SSL connection using" << SSL_get_cipher(m_ssl) << std::endl;

        return ret;
    }

    int Connect(SOCKET socket) {
        // 保存socket
        if(socket < 0) return socket;
        m_socket = socket;

        //将SSL绑定到套接字上
        int ret = SSL_set_fd(m_ssl, m_socket);
        if(ret < 0) return ret;

        //建立SSL链接
        ret = SSL_connect(m_ssl);
        if(ret < 0) return ret;

        //打印所有加密算法的信息(可选)
        std::cout<< "SSL connection using" << SSL_get_cipher(m_ssl) << std::endl;

        return ret;
    }

private:
    SSL     *m_ssl;
    SOCKET  m_socket;

    friend class HycSSLContex;
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
