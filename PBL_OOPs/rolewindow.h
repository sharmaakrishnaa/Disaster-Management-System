
#ifndef ROLEWINDOW_H
#define ROLEWINDOW_H

#include <QWidget>
#include <QString>

class QPushButton;
class QLabel;

class RoleWindow : public QWidget
{
    Q_OBJECT

public:
    explicit RoleWindow(QWidget *parent = nullptr);

private:
    QLabel *messageLabel;

    QPushButton *victimButton;
    QPushButton *volunteerButton;
    QPushButton *ngoButton;
    QPushButton *adminButton;

    void openDashboard(const QString &role);
};

#endif // ROLEWINDOW_H
