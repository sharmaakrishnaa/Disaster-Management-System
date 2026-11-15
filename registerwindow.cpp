
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

// OOP CONCEPT 1: INHERITANCE
// RegisterWindow inherits from QWidget.
// This allows RegisterWindow to use QWidget's features,
// such as setWindowTitle(), setMinimumSize() and setStyleSheet().

RegisterWindow::RegisterWindow(QWidget *parent)
    : QWidget(parent)
{
    // ABSTRACTION:
    // Qt provides ready-made functions for window configuration.
    // We use these functions without implementing their internal working.
    setWindowTitle("SOS Connect - Create Account");
    setMinimumSize(450, 550);

    // GUI DESIGN:
    // Styles are applied using Qt's stylesheet feature.
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

    // ABSTRACTION:
    // QLabel is a ready-made Qt class used to display text.
    QLabel *title = new QLabel("Create Your Account");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 22pt; font-weight: bold;");

    QLabel *subtitle =
        new QLabel("Join SOS Connect to help communities");
    subtitle->setAlignment(Qt::AlignCenter);

    // ENCAPSULATION (related to class data):
    // The input widgets collect registration information.
    // If these members are declared private in registerwindow.h,
    // they demonstrate data hiding within RegisterWindow.

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

    // ABSTRACTION:
    // QFormLayout arranges labels and input fields in a form.
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

    // ABSTRACTION:
    // QVBoxLayout manages the vertical arrangement of GUI components.
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

    // QT SIGNALS AND SLOTS / EVENT-DRIVEN PROGRAMMING:
    // When the Create Account button is clicked, this lambda function runs.
    // The lambda is an anonymous function, not an OOP pillar itself.
    connect(registerButton, &QPushButton::clicked,
            this, [this]() {

                // Accessing the input widgets through this class.
                QString name = nameInput->text().trimmed();
                QString email = emailInput->text().trimmed();
                QString password = passwordInput->text();
                QString confirmPassword = confirmPasswordInput->text();

                // VALIDATION:
                // Check whether all required fields are filled.
                if (name.isEmpty() || email.isEmpty() ||
                    password.isEmpty() || confirmPassword.isEmpty()) {
                    messageLabel->setText("Please fill in all the fields.");
                    return;
                }

                // Check the basic email format.
                if (!email.contains('@') || !email.contains('.')) {
                    messageLabel->setText("Please enter a valid email address.");
                    return;
                }

                // Check minimum password length.
                if (password.length() < 6) {
                    messageLabel->setText(
                        "Password must contain at least 6 characters.");
                    return;
                }

                // Check whether both passwords match.
                if (password != confirmPassword) {
                    messageLabel->setText("Passwords do not match.");
                    return;
                }

                // Prevent characters that would break the text-file format.
                if (name.contains('|') || email.contains('|') ||
                    name.contains('\n') || email.contains('\n')) {
                    messageLabel->setText(
                        "Name and email cannot contain | or a newline.");
                    return;
                }

                // ABSTRACTION:
                // QStandardPaths provides the appropriate application-data path.
                // QDir creates the directory without us implementing filesystem logic.
                QString dataDir =
                    QStandardPaths::writableLocation(
                        QStandardPaths::AppDataLocation);

                if (dataDir.isEmpty() || !QDir().mkpath(dataDir)) {
                    messageLabel->setText(
                        "Could not create the account storage directory.");
                    return;
                }

                QString filePath = dataDir + "/accounts.txt";

                // ABSTRACTION:
                // QFile provides functions to access and manage files.
                QFile file(filePath);

                // Check whether this email is already registered.
                if (file.exists()) {
                    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                        messageLabel->setText(
                            "Could not read the accounts file.");
                        return;
                    }

                    // QTextStream simplifies reading text from the file.
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

                // SECURITY:
                // Hash the password before storing it.
                // This is a security feature, not an OOP pillar.
                QByteArray passwordHash =
                    QCryptographicHash::hash(
                        password.toUtf8(),
                        QCryptographicHash::Sha256).toHex();

                // FILE HANDLING:
                // Open the file in append mode to preserve existing accounts.
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

                // ABSTRACTION:
                // QMessageBox displays a standard success dialog.
                QMessageBox::information(
                    this,
                    "Registration Successful",
                    "Your account has been saved successfully!"
                    );

                // Clear the fields after successful registration.
                nameInput->clear();
                emailInput->clear();
                passwordInput->clear();
                confirmPasswordInput->clear();
                messageLabel->clear();
            });

    // SIGNAL AND SLOT:
    // Clicking Close Registration calls QWidget's close() function.
    connect(backButton, &QPushButton::clicked,
            this, &QWidget::close);
}
