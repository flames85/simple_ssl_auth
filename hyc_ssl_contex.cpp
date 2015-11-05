#include "hyc_ssl_contex.h"

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
    SSL_CTX_set_verify(m_ssl_ctx, SSL_VERIFY_PEER, NULL);
//        SSL_CTX_set_verify_depth(m_ssl_ctx, 1);

    int ret = 0;
    //若验证,则放置CA证书
    if( (ret = SSL_CTX_load_verify_locations(m_ssl_ctx,
                                            ca_verify_file_path.c_str(),
                                            NULL)) < 0)
    {
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
        return ret;
    }

    //读取密钥文件
    if( (ret = SSL_CTX_use_PrivateKey_file(m_ssl_ctx,
                                           local_private_file_path.c_str(),
                                           SSL_FILETYPE_PEM)) < 0)
    {
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
            if(sslSocket->Accept(socket) < 0) break;
        }
        else
        {
            if(sslSocket->Connect(socket) < 0) break;
        }

        //获取和释放客户端证书
        X509 *peer_cert = SSL_get_peer_certificate(sslSocket->m_ssl);
        // 获取失败
        if(!peer_cert) break;

        X509_free(peer_cert);

        return sslSocket;

    }while(0);

    delete sslSocket;
    sslSocket = NULL;

    return NULL;
}
