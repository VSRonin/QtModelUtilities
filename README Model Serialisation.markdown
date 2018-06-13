# Model Serialisation

Implements a general method to serialise `QAbstractItemModel` based models to various common formats.

### Custom Types

To save custom classes stored in the model as `QVariant`s you will need to define and register their stream operators. For example, for class `MyClass` you will need to implement

    QDataStream& operator<<(QDataStream &stream, const MyClass& val);
    QDataStream& operator>>(QDataStream & stream, MyClass& val);

And resgister them before using any save/load functionality by calling

    qRegisterMetaType<MyClass>("MyClass");
    qRegisterMetaTypeStreamOperators<MyClass>("MyClass");

### Dependencies

+ Qt Core
+ Qt Gui (Optional to allow saving images in human-visible format)