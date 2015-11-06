#include "hyc_ssl_contex.h"


HycSSLSocket::HycSSLSocket(SSL_CTX *ssl_ctx)
    :m_ssl(NULL),
     m_socket(-1)
{
    //创建当前连接的SSL结构体
    m_ssl = SSL_new(ssl_ctx);
}

HycSSLSocket::~HycSSLSocket() {
    if(m_ssl)
    {
        //断开SSL链接
        SSL_shutdown(m_ssl);
        //释放当前SSL链接结构体
        SSL_free(m_ssl);
    }
    //断开TCP链接
    if(m_socket > 0)
        CloseSocket(m_socket);
}

int HycSSLSocket::Read(char* data, size_t maxLen) {
    return SSL_read(m_ssl, data, maxLen);
}

int HycSSLSocket::Write(const char* data, size_t maxLen) {
    return SSL_write(m_ssl, data, maxLen);
}

int HycSSLSocket::Accept(SOCKET socket) {
    // 保存socket
    if(socket < 0) return socket;
    m_socket = socket;

    //将SSL绑定到套接字上
    int ret = SSL_set_fd(m_ssl, m_socket);
    if(ret < 0) return ret;

    //建立SSL链接
    ret = SSL_accept(m_ssl);
    if(ret < 0) return ret;

    return ret;
}

int HycSSLSocket::Connect(SOCKET socket) {
    // 保存socket
    if(socket < 0) return socket;
    m_socket = socket;

    //将SSL绑定到套接字上
    int ret = SSL_set_fd(m_ssl, m_socket);
    if(ret < 0) return ret;

    //建立SSL链接
    ret = SSL_connect(m_ssl);
    if(ret < 0) return ret;

    return ret;
}

//////

HycSSLContex::HycSSLContex(bool isServer)
: m_isServer(isServer)
{
    //初始化SSL库
    SSL_library_init();
    //支持所有算法
    OpenSSL_add_all_algorithms();
    //提供将错误号解析为字符串的功能
    SSL_load_error_strings();

    //创建SSL上下文环境 每个进程只需维护一个SSL_CTX结构体
    if(m_isServer)
        m_ssl_ctx = SSL_CTX_new(TLSv1_server_method()); //设置服务端使用的SSL版本
    else
        m_ssl_ctx = SSL_CTX_new(TLSv1_client_method()); //设置客户端使用的SSL版本

}

HycSSLContex::~HycSSLContex() {
    //释放SSL上下文
    if(m_ssl_ctx)
        SSL_CTX_free(m_ssl_ctx);
}

int HycSSLContex::SetContex(const std::string &ca_verify_file_path,
                            const std::string &local_certificate_file_path,
                            const std::string &local_private_file_path) {
    //验证对方
    SSL_CTX_set_verify(m_ssl_ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);
    SSL_CTX_set_cipher_list(m_ssl_ctx, "RC4-MD5");

    SSL_CTX_set_default_verify_paths(m_ssl_ctx) ;
    SSL_CTX_set_verify_depth(m_ssl_ctx, 4);

    int ret = 0;
    //若验证,则放置CA证书
    if( (ret = SSL_CTX_load_verify_locations(m_ssl_ctx,
                                            ca_verify_file_path.c_str(),
                                            NULL)) < 0)
    {
        std::cerr << "load CA verify file error." << std::endl;
        return ret;
    }

    //设置pass phrase
    char password[1024] = "pass phrase";
    SSL_CTX_set_default_passwd_cb_userdata(m_ssl_ctx, password);

    //读取证书文件
    if( (ret = SSL_CTX_use_certificate_file(m_ssl_ctx,
                                            local_certificate_file_path.c_str(),
                                            SSL_FILETYPE_PEM)) < 0)
    {
        std::cerr << "use certificate file error." << std::endl;
        return ret;
    }

    //读取密钥文件
    if( (ret = SSL_CTX_use_PrivateKey_file(m_ssl_ctx,
                                           local_private_file_path.c_str(),
                                           SSL_FILETYPE_PEM)) < 0)
    {
        std::cerr << "use privatekey file error." << std::endl;
        return ret;
    }

    //验证密钥是否与证书一致
    if( (ret = SSL_CTX_check_private_key(m_ssl_ctx) ) < 0)
    {
        return ret;
    }
    return ret;
}

HycSSLSocket* HycSSLContex::CreateSSLSocket(SOCKET socket)
{
    HycSSLSocket *sslSocket = new HycSSLSocket(m_ssl_ctx);

    if(!sslSocket) return NULL;

    do {
        if(m_isServer)
        {
            if(sslSocket->Accept(socket) < 0) {
                std::cerr << "ssl accept fail." << std::endl;
                break;
            }
        }
        else
        {
            if(sslSocket->Connect(socket) < 0) {
                std::cerr << "ssl connect fail." << std::endl;
                break;
            }
        }

        //打印所有加密算法的信息(可选)
        std::cout<< "SSL connection using" << SSL_get_cipher(sslSocket->m_ssl) << std::endl;

        //获取和释放客户端证书
        X509 *peer_cert = SSL_get_peer_certificate(sslSocket->m_ssl);

        // 证书的subject
        if(!peer_cert)
        {
            std::cerr << "peer certificate nil." << std::endl;
        }

        char* strInfo = X509_NAME_oneline(X509_get_subject_name(peer_cert), 0, 0);
        if(!strInfo)
        {
           std::cerr << "X509_NAME_oneline subject name error." << std::endl;
           break;
        }
        else
        {
           sslSocket->m_subjectName = strInfo;
           OPENSSL_free(strInfo);
        }
        // 证书的issuer
        strInfo = X509_NAME_oneline(X509_get_issuer_name(peer_cert), 0, 0);
        if(!strInfo)
        {
           std::cerr << "X509_NAME_oneline issuer name error." << std::endl;
           break;
        }
        else
        {
           sslSocket->m_issuerName = strInfo;
           OPENSSL_free(strInfo);
        }

        X509_free(peer_cert);

        return sslSocket;

    }while(0);

    delete sslSocket;
    sslSocket = NULL;

    return NULL;
}
