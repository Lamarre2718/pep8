#include <QList>
#include <QStringList>
#include <QTextCursor>
#include "sourcecodepane.h"
#include "ui_sourcecodepane.h"
#include "code.h"

#include <QDebug>

SourceCodePane::SourceCodePane(QWidget *parent) :
        QWidget(parent),
        m_ui(new Ui::SourceCodePane)
{
    m_ui->setupUi(this);
}

SourceCodePane::~SourceCodePane()
{
    delete m_ui;
}

bool SourceCodePane::assemble()
{
    QString sourceLine;
    QString errorString;
    QStringList sourceCodeList;
    Code *code;
    QList<Code *> codeList;
    int byteCount = 0;
    int lineNum = 0;
    bool dotEndDetected = false;

    removeErrorMessages();
    Asm::listOfReferencedSymbols.clear();
    Pep::memAddrssToAssemblerListing.clear();
    Pep::symbolTable.clear();
    QString sourceCode = m_ui->pepSourceCodeTextEdit->toPlainText();
    sourceCodeList = sourceCode.split('\n');
    while (lineNum < sourceCodeList.size() && !dotEndDetected) {
        sourceLine = sourceCodeList[lineNum];
        if (!Asm::processSourceLine(sourceLine, lineNum, code, errorString, byteCount, dotEndDetected)) {
            appendMessageInSourceCodePaneAt(lineNum, errorString, Qt::red);
            return false;
        }
        lineNum++;
    }
    if (!dotEndDetected) {
        errorString = ";ERROR: Missing .END sentinel.";
        appendMessageInSourceCodePaneAt(0, errorString, Qt::red);
        return false;
    }
    if (byteCount > 65535) {
        errorString = ";ERROR: Object code size too large to fit into memory.";
        appendMessageInSourceCodePaneAt(0, errorString, Qt::red);
        return false;
    }

//    qDebug() << "====================";
//    QMapIterator<QString, int> i(Pep::symbolTable);
//    while (i.hasNext()) {
//        i.next();
//        qDebug() << i.key() << ": " << i.value();
//    }
//    qDebug() << "====================";

    for (int i = 0; i < Asm::listOfReferencedSymbols.length(); i++) {
        if (!Pep::symbolTable.contains(Asm::listOfReferencedSymbols[i])) {
            errorString = ";ERROR: Symbol " + Asm::listOfReferencedSymbols[i] + " is used but not defined.";
            appendMessageInSourceCodePaneAt(Asm::listOfReferencedSymbolLineNums[i], errorString, Qt::red);
            return false;
        }
    }
    qDebug() << "byteCount == " << byteCount;
    return true;

}

QList<int> SourceCodePane::getObjectCode() { return objectCode; }
QStringList SourceCodePane::getAssemblerListingList() { return assemblerListingList; }
QStringList SourceCodePane::getListingTraceList() { return listingTraceList; }
QList<bool> SourceCodePane::getHasCheckBox() { return hasCheckBox; }

void SourceCodePane::appendMessageInSourceCodePaneAt(int lineNumber, QString message, Qt::GlobalColor color)
{
    QTextCursor cursor(m_ui->pepSourceCodeTextEdit->document());
    cursor.setPosition(0);
    for (int i = 0; i < lineNumber; i++) {
        cursor.movePosition(QTextCursor::NextBlock);
    }
    cursor.movePosition(QTextCursor::EndOfLine);
    m_ui->pepSourceCodeTextEdit->setTextCursor(cursor);
    m_ui->pepSourceCodeTextEdit->setTextColor(color);
    m_ui->pepSourceCodeTextEdit->textCursor().insertText(message);
    m_ui->pepSourceCodeTextEdit->setTextColor(Qt::black);
}

void SourceCodePane::removeErrorMessages()
{
    QTextCursor cursor(m_ui->pepSourceCodeTextEdit->document()->find(";ERROR:"));
    while (!cursor.isNull()) {
        cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
        cursor.removeSelectedText();
        cursor = m_ui->pepSourceCodeTextEdit->document()->find(";ERROR:", cursor);
    }
    cursor = m_ui->pepSourceCodeTextEdit->document()->find(";WARNING:");
    while (!cursor.isNull()) {
        cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
        cursor.removeSelectedText();
        cursor = m_ui->pepSourceCodeTextEdit->document()->find(";WARNING:", cursor);
    }
}
