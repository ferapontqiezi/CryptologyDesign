# PKCS#7

### Notes

`BIO_new_mem_buf(const void *buf, int len) `使用**buf **中的**len**字节数据创建内存 BIO ，如果**len**为 -1，则假定**buf**为 nul 终止，其长度由**strlen**确定。BIO 设置为只读状态，因此无法写入。当需要以 BIO 的形式从静态内存区域提供某些数据时，这很有用。提供的数据直接从提供的缓冲区中读取：它**不会**首先复制，因此提供的内存区域必须保持不变，直到 BIO 被释放。

`PKCS7_get_signer_info`() 返回一个指向 a 的内部指针 `STACK_OF(PKCS7_SIGNER_INFO)` 对象或`NULL`失败。如果它失败 p7是 `NULL`，如果它没有内容，或者它的类型不是签名数据 或者 签名和包络数据.

`PKCS7_dataVerify()`是验证正确性的函数，如果返回值为1，则说明正确；反之亦然。

`ERR_load_crypto_strings()` 为所有**libcrypto**函数注册错误字符串。`SSL_load_error_strings()` 执行相同的操作，但还会注册**libssl**错误字符串。

`OpenSSL_add_all_algorithms() `将所有算法添加到表中（摘要和密码）。

> 典型的应用程序会在退出前调用 OpenSSL_add_all_algorithms() 和 EVP_cleanup()。
>
> 应用程序不需要添加算法来显式使用它们，例如通过 EVP_sha1()。如果它（或它调用的任何函数）需要查找算法，它只需要添加它们。
>
> 密码和摘要查找函数用于库的许多部分。如果该表未初始化，则多个函数将行为不端并抱怨它们找不到算法。这包括 PEM、PKCS#12、SSL 和 S/MIME 库。这是 OpenSSL 邮件列表中的常见查询。
>
> 在所有算法中调用 OpenSSL_add_all_algorithms() 链接：因此静态链接的可执行文件可能非常大。如果这很重要，则可以只添加所需的密码和摘要。

`X509_STORE_add_cert()` 和 `X509_STORE_add_crl()` 将各自的对象添加到**X509_STORE**的本地存储。不应该以这种方式添加不受信任的对象。添加的对象的引用计数增加 1，因此调用者保留对象的所有权，并在不再需要时需要释放它。



### References

[示例代码](https://cpp.hotexamples.com/examples/-/-/PKCS7_dataVerify/cpp-pkcs7_dataverify-function-examples.html)

[openssl官方文档](https://www.openssl.org/docs/man1.1.1/man3/X509_STORE_add_cert.html)
