已知$RSA$解密密钥$(d,p,q)$，解密密文$C$

**解**：一般解法：$m \equiv C^d(mod pq)$，

中国剩余定理解法：第一部分
$$m_1 \equiv C^{d(modp-1)}(modp) \tag{1}$$

第二部分
$$m_2 \equiv C^{d(modq-1)}(modq) \tag{2}$$

由1式和2式可以得到`CRT`最终结果：
$$m \equiv m_1qq^{-1}(modp)+m_2pp^{-1}(modq)(modpq)$$