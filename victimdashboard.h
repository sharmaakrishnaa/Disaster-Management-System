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

    // Victim form input fields.
    QLineEdit *nameInput;
    QLineEdit *locationInput;

    QComboBox *disasterInput;
    QComboBox *urgencyInput;

    QTextEdit *helpInput;
    QTextEdit *descriptionInput;

    // Submit button and status.
    QPushButton *submitButton;
    QLabel *statusLabel;

    // Handles SOS request submission.
    void submitSOS();
};

#endif // VICTIMDASHBOARD_H
