# Root Index Proxy Model {#RootIndexProxyModelReadme}

This proxy will restrict the model to only descendants of a particular index in the source model.

The functionality is similar to `QAbstractItemView::setRootIndex` but on the model side.
An example usage of this proxy is to use in conjunction with `QSortFilterProxyModel` to prevent the view to lose the set root index when the filter is updated.

### Class Documentation
+ RootIndexProxyModel

### Dependencies

+ Qt Core
+ Qt Gui (optional, only for tests)