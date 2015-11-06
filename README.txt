本工程是C++工程, 主要功能是为linux/win原始socket, "附加"SSL双向认证功能.
依赖库openssl, 已经静态编译进thirdpartyxxx中. 当然也可以自己替换.

由于业务需要, 本工程用的是SSL的一个分支TLS(版本TLS1.0).

# 使用方法见src目录下xxx_demo.cpp代码. 一看就懂. 

# 理解SSL连接简要步骤是:
1. 建立TCP连接.
2. 建立SSL/TLS连接(此时根据需要双方验证对方证书)
3. SSL/TLS通信.
4. 关闭SSL/TLS连接和TCP连接.

# 理解整个SSL/TLS验证, 和单双向验证区别, 如下是示意图. 
1. 可以看到CA是同一个, 并且都是用它来生成的客户端/服务端证书ce.pem和私钥pr.key.
2. 私钥用于本地加密(不会发送给对端), ce.pem证书用于让对端通过CA验证是否是该CA颁发的.
3. 单向验证是客户端验证服务端的合法性.
4. 双向验证是客户端服务端互相验证合法性.验证细节见上2.

单向验证
server                  client
                +-----> CA.pem
ce.pem(s) +-----+    
pr.key(s) +

双向验证
server                  client
CA.pem    <----+   +--> CA.pem
ce.pem(s) +----|--+   + ce.pem(c)
pr.key(s) +    +------+ pr.key(c)

# linux平台编译方式: make

# win平台编译方式: 使用vs2010打开xxx_demo.vcxproj编译.


