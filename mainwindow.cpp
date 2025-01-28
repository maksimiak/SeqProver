#include "mainwindow.h"
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QProcess>
#include <QPdfDocument>
#include <QPdfView>
#include <QFormLayout>
#include <QRadioButton>
#include <QButtonGroup>

#include "BussProofConverter.h"
#include "Parser.h"
#include "ProofSearchExtended.h"
#include "FragmentChecker.h"

MyMainWindow::MyMainWindow(QWidget *parent)
    : QMainWindow(parent),
    m_pdfDocument(nullptr),
    m_pdfView(nullptr),
    m_widthInput(new QLineEdit(this)),
    m_heightInput(new QLineEdit(this))
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    QLabel *prompt = new QLabel(
        "Enter a sequent, e.g. \"(p \\lor q), \\neg (p) \\vdash q\":\n"
        "Then click Prove to generate 'proof.tex'.\n"
        "Next, click 'Compile & Show PDF' to run pdflatex and display the result."
        );
    prompt->setWordWrap(true);
    mainLayout->addWidget(prompt);

    // Input line for the sequent
    m_inputLine = new QLineEdit(this);
    mainLayout->addWidget(m_inputLine);

    // Use QFormLayout for Width and Height Inputs
    QFormLayout *sizeLayout = new QFormLayout();
    m_widthInput->setText("20");  // Default width
    m_widthInput->setFixedWidth(50);  // Compact width
    m_heightInput->setText("20");   // Default height
    m_heightInput->setFixedWidth(50); // Compact height

    sizeLayout->addRow("Page Width (in):", m_widthInput);
    sizeLayout->addRow("Page Height (in):", m_heightInput);

    mainLayout->addLayout(sizeLayout);


    // Radio Buttons for Logic Selection
    QHBoxLayout *radioLayout = new QHBoxLayout();
    QRadioButton *ltlRadio = new QRadioButton("LTL", this);
    QRadioButton *ltl1Radio = new QRadioButton("LTL1", this);
    ltlRadio->setChecked(true);  // Default to "LTL"

    // Group the radio buttons to allow single selection
    QButtonGroup *logicGroup = new QButtonGroup(this);
    logicGroup->addButton(ltlRadio);
    logicGroup->addButton(ltl1Radio);

    radioLayout->addWidget(ltlRadio);
    radioLayout->addWidget(ltl1Radio);
    mainLayout->addLayout(radioLayout);

    // Connect radio buttons to update the selected logic type
    connect(ltlRadio, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) m_selectedRule = NormalBoxRight;
    });
    connect(ltl1Radio, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) m_selectedRule = BoxOrRule;
    });


    // Buttons in a Horizontal Layout
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *fillSequentButton = new QPushButton("Fill Example Sequent", this);
    m_proveButton = new QPushButton("Prove", this);
    QPushButton *compileButton = new QPushButton("Compile and Show PDF", this);

    buttonLayout->addWidget(fillSequentButton);
    buttonLayout->addWidget(m_proveButton);
    buttonLayout->addWidget(compileButton);

    mainLayout->addLayout(buttonLayout);

    // Label for status messages
    m_resultLabel = new QLabel(this);
    mainLayout->addWidget(m_resultLabel);

    // PDF Viewer
    m_pdfDocument = new QPdfDocument(this);
    m_pdfView = new QPdfView(this);
    m_pdfView->setDocument(m_pdfDocument);
    mainLayout->addWidget(m_pdfView);

    // Connect signals
    connect(m_proveButton, &QPushButton::clicked,
            this, &MyMainWindow::onProveClicked);
    connect(compileButton, &QPushButton::clicked,
            this, &MyMainWindow::onCompileAndShowPdf);
    connect(fillSequentButton, &QPushButton::clicked,
            this, &MyMainWindow::onFillExampleSequentClicked);
}




void MyMainWindow::onProveClicked()
{
    QString inputText = m_inputLine->text();
    std::string sequentStr = inputText.toStdString();

    try {
        // parse sequent
        Parser parser(sequentStr);
        Sequent seq = parser.parseSequent();

        FragmentChecker checker(seq);
        if (!checker.sequentIsInAlphaOne() && m_selectedRule == BoxOrRule)
        {
            m_resultLabel->setText("Only alphaOne fragments in LTL1");
            return;
        }

        ProofSearchExtended prover(m_selectedRule);
        auto proof = prover.proveSequent(seq);

        // proof
        // auto proof = proveSequent(seq); // depricated - use ProofSearchExtended instead

        if (!proof) {
            m_resultLabel->setText("No proof found.");
            return;
        }

        // Get width and height
        std::string width = m_widthInput->text().toStdString();
        std::string height = m_heightInput->text().toStdString();

        // convert to bussproofs
        std::string latexCode = fullBussProofs(proof, width, height);

        // write to proof.tex
        QString filename = "proof.tex";
        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            m_resultLabel->setText("Could not open proof.tex for writing");
            return;
        }
        QTextStream out(&file);
        out << QString::fromStdString(latexCode);
        file.close();

        // Show success
        m_resultLabel->setText(
            "Proof found. Wrote LaTeX to proof.tex.\n"
            "Now click 'Compile & Show PDF' to see the result."
            );
    }
    catch (const std::exception &ex) {
        m_resultLabel->setText(QString("Error: ") + ex.what());
    }
}

void MyMainWindow::onCompileAndShowPdf()
{
    QProcess process;
    // If necessary, set the full path to pdflatex:
    // QString program = R"(C:\Path\To\pdflatex.exe)";
    QString program = "pdflatex";

    QStringList args;
    // Force non-stop mode (no user prompts)
    args << "-interaction=nonstopmode" << "proof.tex";

    m_resultLabel->setText("Running pdflatex...");

    process.start(program, args, QIODevice::ReadWrite);
    if (!process.waitForStarted()) {
        QProcess::ProcessError e = process.error();
        m_resultLabel->setText("Could not start pdflatex. Error code: " + QString::number(e));
        QByteArray errData = process.readAllStandardError();
        qDebug() << "stderr: " << errData;
        return;
    }

    // wait up to 30s:
    bool finishedOk = process.waitForFinished(30000);
    if (!finishedOk) {
        m_resultLabel->setText("pdflatex timed out or never finished.");
        process.kill();
        return;
    }

    int exitCode = process.exitCode();
    if (exitCode != 0) {
        QByteArray errData = process.readAllStandardError();
        m_resultLabel->setText("pdflatex failed with code " + QString::number(exitCode)
                               + "\nstderr: " + errData);
        return;
    }

    // If success, we have proof.pdf in same folder
    // Now load it into QPdfDocument
    QString pdfFile = "proof.pdf";
    QPdfDocument::Error status = m_pdfDocument->load(pdfFile);
    if (status != QPdfDocument::Error::None) {
        m_resultLabel->setText("Failed to open proof.pdf with QPdfDocument.");
        return;
    }

    // Show page 0
    m_pdfView->setPageMode(QPdfView::PageMode::SinglePage);
    // m_pdfView->setZoomMode(QPdfView::ZoomMode::FitInView);
    m_pdfView->setZoomMode(QPdfView::ZoomMode::Custom);

    m_resultLabel->setText("Compiled to proof.pdf and loaded in viewer below.");
}

void MyMainWindow::onFillExampleSequentClicked()
{
    // Fill the input line with a predefined sequent
    m_inputLine->setText("\\Box (p \\land q) \\vdash \\Box (p \\land x), \\Box q");
}
