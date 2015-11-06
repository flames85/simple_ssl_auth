本工程主要功能是为原始socket"附加"SSL双向认证功能.

由于业务需要, 本工程用的是SSL的一个分支TLS(版本TLS1.0).

SSL连接简要步骤是:

1. 建立TCP连接.
2. 建立SSL/TLS连接(此时根据需要双方验证对方证书)
3. SSL/TLS通信.
4. 关闭SSL/TLS连接和TCP连接.

代码见src目录下demo代码.


单向验证
server                  client
                +-----> ca.pem
ce.pem(s) +-----+    
pr.key(s) +

双向验证
server                  client
ca.pem    <----+   +--> ca.pem
ce.pem(s) +----|--+   + ce.pem(c)
pr.key(s) +    +------+ pr.key(c)


