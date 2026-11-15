
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

// OOP CONCEPT 1: INHERITANCE
// RoleWindow inherits from QWidget.
// This allows it to use QWidget's features, such as window settings and styling.

RoleWindow::RoleWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("SOS Connect - Select Role");
    setMinimumSize(550, 600);

    // ABSTRACTION:
    // Qt provides ready-made functions for styling the interface.
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

    // ABSTRACTION:
    // QLabel displays text without requiring us to implement text rendering.
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

    // GUI COMPONENTS:
    // Separate buttons allow users to select their role.
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

    // ABSTRACTION:
    // QVBoxLayout automatically arranges the widgets vertically.
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

    // QT SIGNALS AND SLOTS / EVENT-DRIVEN PROGRAMMING:
    // When a button is clicked, openDashboard() is called
    // with the selected role.
    // The lambda functions are callbacks, not OOP pillars themselves.

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

// ENCAPSULATION:
// openDashboard() groups the dashboard-selection logic inside RoleWindow.
// If this function is declared private in rolewindow.h,
// it also demonstrates controlled access to the operation.
//
// ABSTRACTION:
// Other parts of the application can call openDashboard(role)
// without needing to manage all the window-opening steps themselves.

void RoleWindow::openDashboard(const QString &role)
{
    // Select and open the Victim dashboard.
    if (role == "Victim") {

        // OBJECT CREATION:
        // A new VictimDashboard object is created.
        VictimDashboard *dashboard = new VictimDashboard();

        // Qt deletes the dashboard object when its window closes.
        dashboard->setAttribute(Qt::WA_DeleteOnClose);

        // SIGNAL AND SLOT:
        // When the dashboard object is destroyed, show RoleWindow again.
        connect(dashboard, &QObject::destroyed,
                this, [this]() {
                    this->show();
                });

        dashboard->show();
        this->hide();
    }

    // Select and open the Volunteer dashboard.
    else if (role == "Volunteer") {

        // OBJECT CREATION:
        // A new VolunteerDashboard object is created.
        VolunteerDashboard *dashboard = new VolunteerDashboard();

        dashboard->setAttribute(Qt::WA_DeleteOnClose);

        connect(dashboard, &QObject::destroyed,
                this, [this]() {
                    this->show();
                });

        dashboard->show();
        this->hide();
    }

    // Select and open the NGO dashboard.
    else if (role == "NGO") {

        // OBJECT CREATION:
        // A new NGODashboard object is created.
        NGODashboard *dashboard = new NGODashboard();

        dashboard->setAttribute(Qt::WA_DeleteOnClose);

        connect(dashboard, &QObject::destroyed,
                this, [this]() {
                    this->show();
                });

        dashboard->show();
        this->hide();
    }

    // Select and open the Administrator dashboard.
    else if (role == "Administrator") {

        // OBJECT CREATION:
        // A new AdminDashboard object is created.
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
