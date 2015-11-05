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
    // ssl读, 失败返回非正数
    int Read(char* data, size_t maxLen);
    // ssl写, 失败返回非正数
    int Write(const char* data, size_t maxLen);
    // 允许外部析构
    ~HycSSLSocket();
private:
    // 不允许外部调用
    explicit HycSSLSocket(SSL_CTX *ssl_ctx);
    int Accept(SOCKET socket) ;
    int Connect(SOCKET socket) ;

private:
    SSL     *m_ssl;
    SOCKET  m_socket;
    friend class HycSSLContex;
};

class HycSSLContex {
public:
    // 构建环境对象必须告知是否为服务
    explicit HycSSLContex(bool isServer);
    virtual ~HycSSLContex();
    // 创建环境, 失败返回负数
    int SetContex(const std::string &ca_verify_file_path,
                  const std::string &local_certificate_file_path,
                  const std::string &local_private_file_path);
    // 通过socket句柄返回ssl-socket对象
    HycSSLSocket* CreateSSLSocket(SOCKET socket);

private:
    SSL_CTX        *m_ssl_ctx;
    bool            m_isServer;
};


#endif // _HYC_SSL_CONTEX_H_
