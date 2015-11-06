#ifndef _HYC_SSL_CONTEX_H_
#define _HYC_SSL_CONTEX_H_


// openssl include files
#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <openssl/rand.h>
#include <openssl/err.h>
// std
#include <iostream>


#ifdef WIN32
#pragma comment( lib, "libeay32.lib" )
#pragma comment( lib, "ssleay32.lib" )
#define CloseSocket(s)    closesocket(s)
#else
typedef int SOCKET;
#define CloseSocket(s)    close(s)
#endif

class HycSSLContex;

struct HycSSLSocket {
    // ssl读, 失败返回非正数
    int Read(char* data, size_t maxLen);
    // ssl写, 失败返回非正数
    int Write(const char* data, size_t maxLen);
    // 允许外部析构
    ~HycSSLSocket();

    std::string     m_subjectName;
    std::string     m_issuerName;

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
                  const std::string &local_private_file_path,
                  const char *cipher = NULL);
    // 通过socket句柄返回ssl-socket对象
    HycSSLSocket* CreateSSLSocket(SOCKET socket);

private:
    SSL_CTX        *m_ssl_ctx;
    bool            m_isServer;
};


#endif // _HYC_SSL_CONTEX_H_
