# Insert Proxy Model {#InsertProxyModelReadme}

This proxy will add an extra row, column or both to allow users to insert new sections with a familiar interface.

You can use `setInsertDirection` to determine whether to show an extra row or column. By default, this model will behave as `QIdentityProxyModel`

You can either call `commitRow`/`commitColumn` or reimplement `validRow`/`validColumn` to decide when a row/column should be added to the main model. 
By default the row/column is committed as soon as there is any data in any cell

Only flat models are supported. Branches of a tree will be hidden by the proxy

### Class Documentation
+ InsertProxyModel

### Dependencies

+ Qt Core