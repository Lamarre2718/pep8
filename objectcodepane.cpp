#include <QFontDialog>
#include "objectcodepane.h"
#include "ui_objectcodepane.h"
#include "pep.h"

ObjectCodePane::ObjectCodePane(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::ObjectCodePane)
{
    m_ui->setupUi(this);

    connect(m_ui->pepObjectCodeTextEdit->document(), SIGNAL(modificationChanged(bool)), this, SLOT(setLabelToModified(bool)));

    connect(m_ui->pepObjectCodeTextEdit, SIGNAL(undoAvailable(bool)), this, SIGNAL(undoAvailable(bool)));
    connect(m_ui->pepObjectCodeTextEdit, SIGNAL(redoAvailable(bool)), this, SIGNAL(redoAvailable(bool)));

    if (Pep::getSystem() != "Mac") {
        m_ui->pepObjectCodeLabel->setFont(QFont("Verdana"));
        m_ui->pepObjectCodeTextEdit->setFont(QFont("Dark Courier"));
    }
}

ObjectCodePane::~ObjectCodePane()
{
    delete m_ui;
}

void ObjectCodePane::setObjectCode(QList<int> objectCode)
{
    QString objectCodeString = "";
    for (int i = 0; i < objectCode.length(); i++) {
        objectCodeString.append(QString("%1").arg(objectCode[i], 2, 16, QLatin1Char('0')).toUpper());
        objectCodeString.append((i % 16) == 15 ? '\n' : ' ');
    }
    objectCodeString.append("zz");
    m_ui->pepObjectCodeTextEdit->clear();
    m_ui->pepObjectCodeTextEdit->setText(objectCodeString);
}

void ObjectCodePane::setObjectCodePaneText(QString string)
{
	m_ui->pepObjectCodeTextEdit->setText(string);
}

bool ObjectCodePane::getObjectCode(QList<int> &objectCodeList)
{
    QString objectString = m_ui->pepObjectCodeTextEdit->toPlainText();
    while (objectString.length() > 0) {
        if (objectString.at(1) == QChar('z')) {
            return true;
        }
        if (objectString.length() < 3) {
            return false;
        }
        QString s = objectString.left(2); // Get the two-char hex number
        objectString.remove(0, 3); // Removes the number and trailing whitespace
        bool ok;
        objectCodeList.append(s.toInt(&ok, 16));
        if (!ok) {
            return false;
        }
    }
    return false;
}

void ObjectCodePane::ObjectCodePane::clearObjectCode()
{
    m_ui->pepObjectCodeTextEdit->clear();
}

bool ObjectCodePane::isModified()
{
    return m_ui->pepObjectCodeTextEdit->document()->isModified();
}

void ObjectCodePane::setModifiedFalse()
{
    m_ui->pepObjectCodeTextEdit->document()->setModified(false);
}

QString ObjectCodePane::toPlainText()
{
    return m_ui->pepObjectCodeTextEdit->toPlainText();
}

void ObjectCodePane::setCurrentFile(QString string)
{
    if (!string.isEmpty()) {
        m_ui->pepObjectCodeLabel->setText("Object Code - " + string);
    }
    else {
        m_ui->pepObjectCodeLabel->setText("Object Code - untitled.pepo");
    }
}

void ObjectCodePane::highlightOnFocus()
{
    if (m_ui->pepObjectCodeTextEdit->hasFocus()) {
        m_ui->pepObjectCodeLabel->setAutoFillBackground(true);
    }
    else {
        m_ui->pepObjectCodeLabel->setAutoFillBackground(false);
    }
}

bool ObjectCodePane::hasFocus()
{
    return m_ui->pepObjectCodeTextEdit->hasFocus();
}

void ObjectCodePane::undo()
{
    m_ui->pepObjectCodeTextEdit->undo();
}

void ObjectCodePane::redo()
{
    m_ui->pepObjectCodeTextEdit->redo();
}

bool ObjectCodePane::isUndoable()
{
    return m_ui->pepObjectCodeTextEdit->document()->isUndoAvailable();
}

bool ObjectCodePane::isRedoable()
{
    return m_ui->pepObjectCodeTextEdit->document()->isRedoAvailable();
}

void ObjectCodePane::cut()
{
    m_ui->pepObjectCodeTextEdit->cut();
}

void ObjectCodePane::copy()
{
    m_ui->pepObjectCodeTextEdit->copy();
}

void ObjectCodePane::paste()
{
    m_ui->pepObjectCodeTextEdit->paste();
}

void ObjectCodePane::setFont()
{
    bool ok = false;
    QFont font = QFontDialog::getFont(&ok, QFont(m_ui->pepObjectCodeTextEdit->font()), this, "Set Object Code Font", QFontDialog::DontUseNativeDialog);
    if (ok) {
        m_ui->pepObjectCodeTextEdit->setFont(font);
    }
}

void ObjectCodePane::setReadOnly(bool b)
{
    m_ui->pepObjectCodeTextEdit->setReadOnly(b);
}

void ObjectCodePane::setLabelToModified(bool modified)
{
    QString temp = m_ui->pepObjectCodeLabel->text();
    if (modified) {
        m_ui->pepObjectCodeLabel->setText(temp.append(temp.endsWith(QChar('*')) ? "" : "*"));
    }
    else if (temp.endsWith(QChar('*'))) {
        temp.chop(1);
        m_ui->pepObjectCodeLabel->setText(temp);
    }
}

