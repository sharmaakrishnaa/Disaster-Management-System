
#ifndef VICTIMDASHBOARD_H
#define VICTIMDASHBOARD_H

#include <QWidget>

class QLineEdit;
class QComboBox;
class QTextEdit;
class QLabel;
class QPushButton;

class VictimDashboard : public QWidget
{
    Q_OBJECT

public:
    explicit VictimDashboard(QWidget *parent = nullptr);

private:
    QLineEdit *nameInput;
    QLineEdit *locationInput;

    QComboBox *disasterInput;
    QComboBox *urgencyInput;

    QTextEdit *helpInput;
    QTextEdit *descriptionInput;

    QPushButton *submitButton;
    QLabel *statusLabel;

    void submitSOS();
};

#endif // VICTIMDASHBOARD_H
