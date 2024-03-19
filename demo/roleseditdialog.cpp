#include "roleseditdialog.h"
#include "ui_roleseditdialog.h"
#include <QRegularExpressionValidator>
#include <QColorDialog>
#include <QColor>
#include <QFontDialog>
#include <QButtonGroup>
RolesEditDialog::RolesEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RolesEditDialog)
{
    ui->setupUi(this);
    auto colorValidator = new QRegularExpressionValidator(QRegularExpression(QStringLiteral(R"**(#(?:\d|[A-F]){6})**"),QRegularExpression::CaseInsensitiveOption),this);
    ui->decorationEdit->setValidator(colorValidator);
    ui->backgroundEdit->setValidator(colorValidator);
    ui->foregroundEdit->setValidator(colorValidator);
    QButtonGroup* checkStateGorup = new QButtonGroup(this);
    checkStateGorup->addButton(ui->checkstateRadioSet);
    checkStateGorup->addButton(ui->checkstateRadioNotSet);
    connect(ui->decorationButton,&QPushButton::clicked,this,std::bind(&RolesEditDialog::getColor,this,ui->decorationEdit));
    connect(ui->backgroundButton,&QPushButton::clicked,this,std::bind(&RolesEditDialog::getColor,this,ui->backgroundEdit));
    connect(ui->foregroundButton,&QPushButton::clicked,this,std::bind(&RolesEditDialog::getColor,this,ui->foregroundEdit));
    connect(ui->fontButton,&QPushButton::clicked,this,&RolesEditDialog::getFont);
    connect(ui->checkstateRadioSet,&QRadioButton::toggled,ui->checkStateBox,&QCheckBox::setEnabled);
    ui->alignHComboBox->addItem(tr("Not Set"),0);
    ui->alignHComboBox->addItem(tr("Left"),Qt::AlignLeft);
    ui->alignHComboBox->addItem(tr("Right"),Qt::AlignRight);
    ui->alignHComboBox->addItem(tr("Center"),Qt::AlignHCenter);
    ui->alignHComboBox->addItem(tr("Justify"),Qt::AlignJustify);
    ui->alignVComboBox->addItem(tr("Not Set"),0);
    ui->alignVComboBox->addItem(tr("Top"),Qt::AlignTop);
    ui->alignVComboBox->addItem(tr("Bottom"),Qt::AlignBottom);
    ui->alignVComboBox->addItem(tr("Center"),Qt::AlignVCenter);
    ui->alignVComboBox->addItem(tr("Baseline"),Qt::AlignBaseline);

}

RolesEditDialog::~RolesEditDialog()
{
    delete ui;
}

void RolesEditDialog::loadRoles(const QMap<int, QVariant> &roles)
{
    ui->displayEdit->setText(roles.value(Qt::DisplayRole).toString());
    QColor colorValue = roles.value(Qt::DisplayRole).value<QColor>();
    ui->decorationEdit->setText(colorValue.isValid() ? colorValue.name():QString());
    ui->editEdit->setText(roles.value(Qt::EditRole).toString());
    ui->tooltipEdit->setText(roles.value(Qt::ToolTipRole).toString());
    ui->statustipEdit->setText(roles.value(Qt::StatusTipRole).toString());
    ui->whatsthisEdit->setText(roles.value(Qt::WhatsThisRole).toString());
    colorValue = roles.value(Qt::ForegroundRole).value<QColor>();
    ui->foregroundEdit->setText(colorValue.isValid() ? colorValue.name():QString());
    colorValue = roles.value(Qt::BackgroundRole).value<QColor>();
    ui->backgroundEdit->setText(colorValue.isValid() ? colorValue.name():QString());
    const QSize sizeHintData = roles.value(Qt::SizeHintRole).value<QSize>();
    ui->sizehintHSpin->setValue(sizeHintData.isValid() ? sizeHintData.height():0);
    ui->sizehintWSpin->setValue(sizeHintData.isValid() ? sizeHintData.width():0);
    const QVariant fontVariant=roles.value(Qt::FontRole);
    if(fontVariant.isValid())
        ui->fontEdit->setText(fontVariant.value<QFont>().toString());
    else
        ui->fontEdit->clear();
    const QVariant checkStateVariant = roles.value(Qt::CheckStateRole);
    if(checkStateVariant.isValid()){
        ui->checkstateRadioSet->setChecked(true);
        ui->checkStateBox->setCheckState(checkStateVariant.value<Qt::CheckState>());
    }
    else{
        ui->checkstateRadioNotSet->setChecked(true);
        ui->checkStateBox->setCheckState(Qt::Unchecked);
    }
    const int alignData = roles.value(Qt::TextAlignmentRole).toInt();
    for(int i=0, maxI=ui->alignHComboBox->count();i<maxI;++i){
        if(ui->alignHComboBox->itemData(i).toInt()== (alignData & 0x000F)){
            ui->alignHComboBox->setCurrentIndex(i);
            break;
        }
    }
    for(int i=0, maxI=ui->alignVComboBox->count();i<maxI;++i){
        if(ui->alignVComboBox->itemData(i).toInt()== (alignData & (~0x000F))){
            ui->alignVComboBox->setCurrentIndex(i);
            break;
        }
    }
    ui->userRoleEdit->setText(roles.value(Qt::UserRole).toString());

}

QMap<int, QVariant> RolesEditDialog::roles() const
{
    QMap<int, QVariant> result;
    if(!ui->displayEdit->text().isEmpty())
        result[Qt::DisplayRole]=ui->displayEdit->text();
    if(!ui->decorationEdit->text().isEmpty())
        result[Qt::DecorationRole]=QColor::fromString(ui->decorationEdit->text());
    if(!ui->editEdit->text().isEmpty())
        result[Qt::EditRole]=ui->editEdit->text();
    if(!ui->tooltipEdit->text().isEmpty())
        result[Qt::ToolTipRole]=ui->tooltipEdit->text();
    if(!ui->statustipEdit->text().isEmpty())
        result[Qt::StatusTipRole]=ui->statustipEdit->text();
    if(!ui->whatsthisEdit->text().isEmpty())
        result[Qt::WhatsThisRole]=ui->whatsthisEdit->text();
    if(!ui->foregroundEdit->text().isEmpty())
        result[Qt::ForegroundRole]=QColor::fromString(ui->foregroundEdit->text());
    if(!ui->backgroundEdit->text().isEmpty())
        result[Qt::BackgroundRole]=QColor::fromString(ui->backgroundEdit->text());
    if(ui->sizehintHSpin->value()>0 && ui->sizehintWSpin->value()>0)
        result[Qt::SizeHintRole]=QSize(ui->sizehintWSpin->value(),ui->sizehintHSpin->value());
    if(!ui->fontEdit->text().isEmpty()){
        QFont tempFont;
        Q_ASSUME(tempFont.fromString(ui->fontEdit->text()));
        result[Qt::FontRole]=ui->fontEdit->text();
    }
    if(ui->checkstateRadioSet->isChecked())
        result[Qt::CheckStateRole]=ui->checkStateBox->checkState();
    if(ui->alignHComboBox->currentIndex()>0 && ui->alignVComboBox->currentIndex()>0)
        result[Qt::TextAlignmentRole] = ui->alignHComboBox->currentData().toInt() | ui->alignVComboBox->currentData().toInt();
    else if(ui->alignHComboBox->currentIndex()>0)
        result[Qt::TextAlignmentRole] = ui->alignHComboBox->currentData().toInt();
    else if( ui->alignVComboBox->currentIndex()>0)
        result[Qt::TextAlignmentRole] = ui->alignVComboBox->currentData().toInt();
    if(!ui->userRoleEdit->text().isEmpty())
        result[Qt::UserRole]=ui->userRoleEdit->text();
    return result;
}

void RolesEditDialog::getColor(QLineEdit *edit)
{
    const QColor result = QColorDialog::getColor(edit->text().isEmpty() ? Qt::red : QColor::fromString(edit->text()));
    if(result.isValid())
        edit->setText(result.name());
    else
        edit->clear();
}

void RolesEditDialog::getFont()
{
    bool ok=false;
    QFont result;
    if(ui->fontEdit->text().isEmpty()){
        result=QFontDialog::getFont(&ok,this);
    }
    else{
        Q_ASSUME(result.fromString(ui->fontEdit->text()));
        result=QFontDialog::getFont(&ok, result,this);
    }
    if(ok)
        ui->fontEdit->setText(result.toString());
    else
        ui->fontEdit->clear();
}
