
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "registerwindow.h"
#include "rolewindow.h"

#include <QMessageBox>
#include <QLineEdit>
#include <QPushButton>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QCryptographicHash>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->loginButton, &QPushButton::clicked,
            this, [this]() {

                QString email = ui->emailInput->text().trimmed();
                QString password = ui->passwordInput->text();

                if (email.isEmpty() || password.isEmpty()) {
                    ui->messageLabel->setText(
                        "Please enter both email and password.");
                    return;
                }

                if (!email.contains('@') || !email.contains('.')) {
                    ui->messageLabel->setText(
                        "Please enter a valid email address.");
                    return;
                }

                // Use the same account file as registration.
                QString dataDir =
                    QStandardPaths::writableLocation(
                        QStandardPaths::AppDataLocation);

                QString filePath = dataDir + "/accounts.txt";
                QFile file(filePath);

                if (!file.exists()) {
                    QMessageBox::warning(
                        this,
                        "Login Failed",
                        "No registered accounts found.\n"
                        "Please create an account first.");
                    return;
                }

                if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    QMessageBox::critical(
                        this,
                        "File Error",
                        "Could not read the account file.");
                    return;
                }

                // Hash the entered password in the same way as registration.
                QByteArray enteredHash =
                    QCryptographicHash::hash(
                        password.toUtf8(),
                        QCryptographicHash::Sha256).toHex();

                bool loginSuccessful = false;
                QTextStream in(&file);

                while (!in.atEnd()) {
                    QString line = in.readLine();
                    QStringList fields = line.split('|');

                    if (fields.size() != 3) {
                        continue;
                    }

                    QString savedEmail = fields[1].trimmed();
                    QString savedHash = fields[2].trimmed();

                    if (savedEmail.compare(
                            email, Qt::CaseInsensitive) == 0 &&
                        savedHash == QString::fromLatin1(enteredHash)) {
                        loginSuccessful = true;
                        break;
                    }
                }

                file.close();

                if (!loginSuccessful) {
                    ui->messageLabel->setStyleSheet("color: #FF7777;");
                    ui->messageLabel->setText(
                        "Incorrect email or password.");
                    return;
                }

                ui->messageLabel->setStyleSheet("color: #16803C;");
                ui->messageLabel->setText("Login successful!");

                QMessageBox::information(
                    this,
                    "SOS Connect",
                    "Login successful!\nPlease select your role."
                    );

                // Open role selection after every successful login.
                RoleWindow *roleWindow = new RoleWindow;
                roleWindow->setAttribute(Qt::WA_DeleteOnClose);
                roleWindow->show();
            });

    connect(ui->registerButton, &QPushButton::clicked,
            this, [this]() {

                RegisterWindow *registerWindow = new RegisterWindow;
                registerWindow->setAttribute(Qt::WA_DeleteOnClose);
                registerWindow->show();
            });
}

MainWindow::~MainWindow()
{
    delete ui;
}
