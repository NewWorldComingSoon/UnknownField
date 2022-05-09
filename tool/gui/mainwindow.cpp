#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), mQStringFilename("") {
  ui->setupUi(this);

  setDefaultDisposition();
  setDefaultComponent();

  // signal-slot
  connect(mQPushButtonRun, SIGNAL(clicked()), this, SLOT(qPushButtonRunSlot()));
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::setDefaultComponent() {

  mQLabelExternalInclude = new QLabel("External include directory");
  ui->verticalLayout_frame_top->addWidget(mQLabelExternalInclude);
  mQTextEditExternalInclude = new QTextEdit();
  ui->verticalLayout_frame_top->addWidget(mQTextEditExternalInclude);

  mQLabelSourceFile = new QLabel("Source file");
  ui->verticalLayout_frame_middle->addWidget(mQLabelSourceFile);

  mQTextEditSourceFile = new QTextEdit();
  QFont qFont("Courier", 8);
  qFont.setStyleHint(QFont::Monospace);
  qFont.setBold(true);
  qFont.setFixedPitch(true);
  mQTextEditSourceFile->setFont(qFont);
  ui->verticalLayout_frame_middle->addWidget(mQTextEditSourceFile);

  mQPushButtonRun = new QPushButton("Run");
  ui->verticalLayout_frame_middle->addWidget(mQPushButtonRun);

  mQLabelLog = new QLabel("Log");
  ui->verticalLayout_frame_bottom->addWidget(mQLabelLog);
  mQTextEditLog = new QTextEdit();
  mQTextEditLog->setReadOnly(true);
  mQTextEditLog->setUndoRedoEnabled(false);
  ui->verticalLayout_frame_bottom->addWidget(mQTextEditLog);
}

void MainWindow::setDefaultDisposition() {
  int iHeight0 = ui->splitter->widget(0)->size().height();
  int iHeight1 = ui->splitter->widget(1)->size().height();
  int iHeight2 = ui->splitter->widget(2)->size().height();
  int iTotalHeight = iHeight0 + iHeight1 + iHeight2;

  QList<int> qListSizes;
  // 20%
  qListSizes.append(iTotalHeight * 20 / 100);
  // 70%
  qListSizes.append(iTotalHeight * 70 / 100);
  // 10%
  qListSizes.append(iTotalHeight * 10 / 100);
  ui->splitter->setSizes(qListSizes);
}

void MainWindow::readSourceFileIntoTextEdit(QString filename) {
  // clean content first
  mQTextEditSourceFile->setText(QString(""));

  QFile qFile(filename);
  if (!qFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::information(nullptr, QString("Error"),
                             QString("Open file failed!"));
    return;
  }

  QTextStream qTextStream(&qFile);
  while (!qTextStream.atEnd()) {
    QString qsLine = qTextStream.readLine();
    mQTextEditSourceFile->append(qsLine);
  }
}

void MainWindow::qPushButtonRunSlot() {
  if (mQStringFilename.length() == 0) {
    return;
  }

  QProcess qProcess;
  QString qsCmd = "UnknownField-cli.exe ";
  qsCmd += mQStringFilename;
  QString qsExternalInclude = mQTextEditExternalInclude->toPlainText();
  if (qsExternalInclude.length() != 0) {
    QStringList qsList =
        qsExternalInclude.split(QRegExp("[\n|;]"), QString::SkipEmptyParts);
    qsCmd += " --";
    for (int i = 0; i < qsList.size(); ++i) {
      QString qsInc = qsList[i].trimmed();
      if (qsInc.length() > 0) {
        qsCmd += " -I";
        qsCmd += "\"";
        qsCmd += qsInc;
        qsCmd += "\"";
      }
    }
  }
  qProcess.start(qsCmd);
  qProcess.waitForFinished();
  mQTextEditLog->append(qProcess.readAllStandardError());
  mQTextEditLog->append(qProcess.readAllStandardOutput());
  readSourceFileIntoTextEdit(mQStringFilename);
}

void MainWindow::on_actionOpen_triggered() {
  QString qsFilename = QFileDialog::getOpenFileName(
      this, "Open file", 0, "Source file (*.h *.hpp *.c *.cpp *.cc)");
  if (qsFilename.length() == 0) {
    return;
  }
  qsFilename = QDir::toNativeSeparators(qsFilename);
  // Set mQStringFilename
  mQStringFilename = qsFilename;

  readSourceFileIntoTextEdit(qsFilename);
}
