
#include "registerwindow.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDir>
#include <QCryptographicHash>

RegisterWindow::RegisterWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("SOS Connect - Create Account");
    setMinimumSize(450, 550);

    setStyleSheet(
        "QWidget {"
        " background-color: #0B1B33;"
        " color: white;"
        " font-family: 'Segoe UI';"
        " font-size: 11pt;"
        "}"
        "QLineEdit {"
        " background-color: white;"
        " color: #172B46;"
        " border: 1px solid #CFD9E6;"
        " border-radius: 6px;"
        " padding: 10px;"
        "}"
        "QPushButton {"
        " background-color: #1672D5;"
        " color: white;"
        " border: none;"
        " border-radius: 6px;"
        " padding: 12px;"
        " font-weight: bold;"
        "}"
        );

    QLabel *title = new QLabel("Create Your Account");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 22pt; font-weight: bold;");

    QLabel *subtitle =
        new QLabel("Join SOS Connect to help communities");
    subtitle->setAlignment(Qt::AlignCenter);

    nameInput = new QLineEdit;
    nameInput->setPlaceholderText("Enter your full name");

    emailInput = new QLineEdit;
    emailInput->setPlaceholderText("Enter your email");

    passwordInput = new QLineEdit;
    passwordInput->setPlaceholderText("Create a password");
    passwordInput->setEchoMode(QLineEdit::Password);

    confirmPasswordInput = new QLineEdit;
    confirmPasswordInput->setPlaceholderText("Confirm your password");
    confirmPasswordInput->setEchoMode(QLineEdit::Password);

    QFormLayout *form = new QFormLayout;
    form->addRow("Full Name:", nameInput);
    form->addRow("Email:", emailInput);
    form->addRow("Password:", passwordInput);
    form->addRow("Confirm Password:", confirmPasswordInput);

    messageLabel = new QLabel;
    messageLabel->setWordWrap(true);
    messageLabel->setStyleSheet("color: #FF9999;");

    registerButton = new QPushButton("Create Account");
    backButton = new QPushButton("Close Registration");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(35, 30, 35, 30);
    layout->setSpacing(18);

    layout->addWidget(title);
    layout->addWidget(subtitle);
    layout->addSpacing(10);
    layout->addLayout(form);
    layout->addWidget(messageLabel);
    layout->addWidget(registerButton);
    layout->addWidget(backButton);
    layout->addStretch();

    connect(registerButton, &QPushButton::clicked,
            this, [this]() {

                QString name = nameInput->text().trimmed();
                QString email = emailInput->text().trimmed();
                QString password = passwordInput->text();
                QString confirmPassword = confirmPasswordInput->text();

                // Validate required fields
                if (name.isEmpty() || email.isEmpty() ||
                    password.isEmpty() || confirmPassword.isEmpty()) {
                    messageLabel->setText("Please fill in all the fields.");
                    return;
                }

                if (!email.contains('@') || !email.contains('.')) {
                    messageLabel->setText("Please enter a valid email address.");
                    return;
                }

                if (password.length() < 6) {
                    messageLabel->setText(
                        "Password must contain at least 6 characters.");
                    return;
                }

                if (password != confirmPassword) {
                    messageLabel->setText("Passwords do not match.");
                    return;
                }

                // Our text-file format uses | as a separator.
                if (name.contains('|') || email.contains('|') ||
                    name.contains('\n') || email.contains('\n')) {
                    messageLabel->setText(
                        "Name and email cannot contain | or a newline.");
                    return;
                }

                // Get a writable directory for application data.
                QString dataDir =
                    QStandardPaths::writableLocation(
                        QStandardPaths::AppDataLocation);

                if (dataDir.isEmpty() || !QDir().mkpath(dataDir)) {
                    messageLabel->setText(
                        "Could not create the account storage directory.");
                    return;
                }

                QString filePath = dataDir + "/accounts.txt";
                QFile file(filePath);

                // Check whether this email is already registered.
                if (file.exists()) {
                    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                        messageLabel->setText(
                            "Could not read the accounts file.");
                        return;
                    }

                    QTextStream in(&file);

                    while (!in.atEnd()) {
                        QString line = in.readLine();
                        QString savedEmail = line.section('|', 1, 1).trimmed();

                        if (savedEmail.compare(
                                email, Qt::CaseInsensitive) == 0) {
                            file.close();
                            messageLabel->setText(
                                "This email is already registered.");
                            return;
                        }
                    }

                    file.close();
                }

                // Hash the password before saving it.
                QByteArray passwordHash =
                    QCryptographicHash::hash(
                        password.toUtf8(),
                        QCryptographicHash::Sha256).toHex();

                // Append the account to the file.
                if (!file.open(QIODevice::WriteOnly |
                               QIODevice::Append |
                               QIODevice::Text)) {
                    messageLabel->setText(
                        "Could not save the account.");
                    return;
                }

                QTextStream out(&file);
                out << name << "|"
                    << email << "|"
                    << QString::fromLatin1(passwordHash)
                    << "\n";

                file.close();

                QMessageBox::information(
                    this,
                    "Registration Successful",
                    "Your account has been saved successfully!"
                    );

                nameInput->clear();
                emailInput->clear();
                passwordInput->clear();
                confirmPasswordInput->clear();
                messageLabel->clear();
            });

    connect(backButton, &QPushButton::clicked,
            this, &QWidget::close);
}
