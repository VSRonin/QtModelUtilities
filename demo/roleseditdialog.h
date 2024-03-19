#ifndef ROLESEDITDIALOG_H
#define ROLESEDITDIALOG_H

#include <QDialog>
#include <QMap>
#include <QVariant>
namespace Ui {
class RolesEditDialog;
}
class QLineEdit;
class RolesEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RolesEditDialog(QWidget *parent = nullptr);
    ~RolesEditDialog();
    void loadRoles(const QMap<int,QVariant>& roles);
    QMap<int,QVariant> roles() const;
private:
    Ui::RolesEditDialog *ui;
    void getColor(QLineEdit* edit);
    void getFont();
};

#endif // ROLESEDITDIALOG_H
