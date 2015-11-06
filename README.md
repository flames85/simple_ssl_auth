本工程主要功能是为原始socket"附加"SSL双向认证功能.

由于业务需要, 本工程用的是SSL的一个分支TLS(版本TLS1.0).

SSL连接创建步骤是:

1. 建立TCP连接.
2. 建立SSL/TLS连接.
3. SSL/TLS通信.
4. 关闭SSL/TLS连接和TCP连接.

代码见src目录下demo代码.
