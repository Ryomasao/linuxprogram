# HTTP ヘッダーの仕様メモ

## リクエスト

HEADER 部分と本文(BODY)で構成される。

```
HEADER
\r\n
BODY
```

### HEADER

以下から構成される。
`request-line`
`request-header-field`

#### request-line

これが一番大事。

```
Method SP Request-URI SP HTTP-Version\r\n
```

#### request-header-field

MDN をみると、さらに`request-headers`,`general headers`,`entity headers`とわけている。
https://developer.mozilla.org/ja/docs/Web/HTTP/Messages
