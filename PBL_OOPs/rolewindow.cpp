
#include "rolewindow.h"
#include "victimdashboard.h"
#include "volunteerdashboard.h"
#include "ngodashboard.h"
#include "admindashboard.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QString>

RoleWindow::RoleWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("SOS Connect - Select Role");
    setMinimumSize(550, 600);

    setStyleSheet(
        "QWidget {"
        " background-color: #0B1B33;"
        " color: white;"
        " font-family: 'Segoe UI';"
        " font-size: 11pt;"
        "}"
        "QPushButton {"
        " background-color: #1672D5;"
        " color: white;"
        " border: none;"
        " border-radius: 8px;"
        " padding: 14px;"
        " font-size: 12pt;"
        " font-weight: bold;"
        "}"
        "QPushButton:hover {"
        " background-color: #328BEA;"
        "}"
        );

    QLabel *title = new QLabel("Choose Your Role");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(
        "font-size: 25pt;"
        "font-weight: bold;"
        );

    QLabel *subtitle = new QLabel(
        "Welcome to SOS Connect\n"
        "Select your role to continue."
        );
    subtitle->setAlignment(Qt::AlignCenter);
    subtitle->setStyleSheet("color: #C6D5E8;");

    messageLabel = new QLabel;
    messageLabel->setAlignment(Qt::AlignCenter);
    messageLabel->setWordWrap(true);

    victimButton = new QPushButton(
        "Victim - Request Emergency Help"
        );

    volunteerButton = new QPushButton(
        "Volunteer - Provide Assistance"
        );

    ngoButton = new QPushButton(
        "NGO - Manage Relief Resources"
        );

    adminButton = new QPushButton("Administrator");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(45, 40, 45, 40);
    layout->setSpacing(18);

    layout->addWidget(title);
    layout->addWidget(subtitle);
    layout->addSpacing(15);
    layout->addWidget(victimButton);
    layout->addWidget(volunteerButton);
    layout->addWidget(ngoButton);
    layout->addWidget(adminButton);
    layout->addWidget(messageLabel);
    layout->addStretch();

    connect(victimButton, &QPushButton::clicked,
            this, [this]() {
                openDashboard("Victim");
            });

    connect(volunteerButton, &QPushButton::clicked,
            this, [this]() {
                openDashboard("Volunteer");
            });

    connect(ngoButton, &QPushButton::clicked,
            this, [this]() {
                openDashboard("NGO");
            });

    connect(adminButton, &QPushButton::clicked,
            this, [this]() {
                openDashboard("Administrator");
            });
}

void RoleWindow::openDashboard(const QString &role)
{
    if (role == "Victim") {
        VictimDashboard *dashboard = new VictimDashboard();

        dashboard->setAttribute(Qt::WA_DeleteOnClose);

        connect(dashboard, &QObject::destroyed,
                this, [this]() {
                    this->show();
                });

        dashboard->show();
        this->hide();
    }
    else if (role == "Volunteer") {
        VolunteerDashboard *dashboard = new VolunteerDashboard();

        dashboard->setAttribute(Qt::WA_DeleteOnClose);

        connect(dashboard, &QObject::destroyed,
                this, [this]() {
                    this->show();
                });

        dashboard->show();
        this->hide();
    }
    else if (role == "NGO") {
        NGODashboard *dashboard = new NGODashboard();

        dashboard->setAttribute(Qt::WA_DeleteOnClose);

        connect(dashboard, &QObject::destroyed,
                this, [this]() {
                    this->show();
                });

        dashboard->show();
        this->hide();
    }
    else if (role == "Administrator") {
        AdminDashboard *dashboard = new AdminDashboard();

        dashboard->setAttribute(Qt::WA_DeleteOnClose);

        connect(dashboard, &QObject::destroyed,
                this, [this]() {
                    this->show();
                });

        dashboard->show();
        this->hide();
    }
}
