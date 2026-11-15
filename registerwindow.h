
#ifndef REGISTERWINDOW_H
#define REGISTERWINDOW_H

#include <QWidget>

class QLineEdit;
class QLabel;
class QPushButton;

class RegisterWindow : public QWidget
{
    Q_OBJECT

public:
    explicit RegisterWindow(QWidget *parent = nullptr);

private:
    QLineEdit *nameInput;
    QLineEdit *emailInput;
    QLineEdit *passwordInput;
    QLineEdit *confirmPasswordInput;

    QLabel *messageLabel;
    QPushButton *registerButton;
    QPushButton *backButton;
};

#endif // REGISTERWINDOW_H
