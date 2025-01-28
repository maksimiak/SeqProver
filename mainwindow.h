#pragma once

#include <QMainWindow>
#include "Token.h"

class QLineEdit;
class QPushButton;
class QLabel;
class QPdfDocument;
class QPdfView;

class MyMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MyMainWindow(QWidget *parent = nullptr);

private slots:
    void onProveClicked();
    // new slot: run pdflatex and display PDF
    void onCompileAndShowPdf();

    // filler
    void onFillExampleSequentClicked();

private:
    QLineEdit   *m_inputLine;
    QPushButton *m_proveButton;
    QLabel      *m_resultLabel;

    // For PDF rendering:
    QPdfDocument *m_pdfDocument;
    QPdfView     *m_pdfView;

    QLineEdit *m_widthInput;
    QLineEdit *m_heightInput;


    RuleType m_selectedRule = NormalBoxRight; // Default rule
};
