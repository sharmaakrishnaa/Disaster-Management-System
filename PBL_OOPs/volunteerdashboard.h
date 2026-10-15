
#ifndef VOLUNTEERDASHBOARD_H
#define VOLUNTEERDASHBOARD_H

#include <QWidget>

class QListWidget;
class QLabel;
class QPushButton;

class VolunteerDashboard : public QWidget
{
    Q_OBJECT

public:
    explicit VolunteerDashboard(QWidget *parent = nullptr);

private:
    QListWidget *requestList;
    QLabel *statusLabel;
    QPushButton *refreshButton;
    QPushButton *acceptButton;

    void loadRequests();
    void acceptRequest();
};

#endif // VOLUNTEERDASHBOARD_H
