# Role Mask Proxy Model

This proxy will act as a mask on top of the source model to intercept data.

An example usage of this proxy is to enable the use of special roles in read-only models.
To change the background brush of a `QSqlQueryModel` for example, you just need to create this proxy, call `addMaskedRole(Qt::BackgroundRole)` and now you can use `setData` as the model was writable for that role.

### Dependencies

+ Qt Core
+ Qt Gui (optional, only for tests)