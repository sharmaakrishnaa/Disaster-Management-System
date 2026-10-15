
#include "victimdashboard.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QPushButton>
#include <QMessageBox>
#include <QFrame>

#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QStandardPaths>
#include <QUuid>
#include <QDateTime>

VictimDashboard::VictimDashboard(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("SOS Connect - Victim Dashboard");
    setMinimumSize(850, 750);

    setStyleSheet(
        "QWidget {"
        " background-color: #F3F6FB;"
        " color: #172B46;"
        " font-family: 'Segoe UI';"
        " font-size: 11pt;"
        "}"
        "QLineEdit, QComboBox, QTextEdit {"
        " background-color: white;"
        " border: 1px solid #CBD5E1;"
        " border-radius: 6px;"
        " padding: 9px;"
        "}"
        "QPushButton {"
        " background-color: #1672D5;"
        " color: white;"
        " border: none;"
        " border-radius: 7px;"
        " padding: 12px;"
        " font-weight: bold;"
        "}"
        "QPushButton:hover {"
        " background-color: #0F5EB5;"
        "}"
        );

    // Heading
    QLabel *heading = new QLabel("Victim Dashboard");
    heading->setStyleSheet(
        "font-size: 25pt;"
        "font-weight: bold;"
        "color: #102747;"
        );

    QLabel *subtitle = new QLabel(
        "Request emergency assistance and relief."
        );
    subtitle->setStyleSheet("color: #64748B;");

    // Banner
    QFrame *banner = new QFrame;
    banner->setStyleSheet(
        "QFrame {"
        " background-color: #102747;"
        " border-radius: 12px;"
        "}"
        );

    QVBoxLayout *bannerLayout = new QVBoxLayout(banner);

    QLabel *bannerTitle = new QLabel("Need Emergency Help?");
    bannerTitle->setStyleSheet(
        "color: white;"
        "font-size: 19pt;"
        "font-weight: bold;"
        );

    QLabel *bannerText = new QLabel(
        "Fill in the details below to prepare your SOS request."
        );
    bannerText->setStyleSheet("color: #D7E5F7;");
    bannerText->setWordWrap(true);

    bannerLayout->addWidget(bannerTitle);
    bannerLayout->addWidget(bannerText);

    // Form fields
    nameInput = new QLineEdit;
    nameInput->setPlaceholderText("Enter your full name");

    locationInput = new QLineEdit;
    locationInput->setPlaceholderText(
        "Enter your location or nearby landmark"
        );

    disasterInput = new QComboBox;
    disasterInput->addItems({
        "Select disaster type",
        "Flood",
        "Earthquake",
        "Landslide",
        "Fire",
        "Cyclone",
        "Medical Emergency",
        "Other"
    });

    urgencyInput = new QComboBox;
    urgencyInput->addItems({
        "Select urgency",
        "Low",
        "Medium",
        "High",
        "Critical"
    });

    helpInput = new QTextEdit;
    helpInput->setPlaceholderText(
        "Example: food, drinking water, medicine, rescue..."
        );
    helpInput->setMaximumHeight(85);

    descriptionInput = new QTextEdit;
    descriptionInput->setPlaceholderText(
        "Describe the emergency and any important details."
        );
    descriptionInput->setMaximumHeight(100);

    QFormLayout *form = new QFormLayout;
    form->setSpacing(14);

    form->addRow("Full Name:", nameInput);
    form->addRow("Location:", locationInput);
    form->addRow("Disaster Type:", disasterInput);
    form->addRow("Urgency:", urgencyInput);
    form->addRow("Help Required:", helpInput);
    form->addRow("Description:", descriptionInput);

    submitButton = new QPushButton("Submit SOS Request");
    submitButton->setMinimumHeight(48);

    statusLabel = new QLabel(
        "Your request has not been submitted."
        );
    statusLabel->setWordWrap(true);
    statusLabel->setStyleSheet("color: #64748B;");

    // Main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(35, 25, 35, 25);
    mainLayout->setSpacing(18);

    mainLayout->addWidget(heading);
    mainLayout->addWidget(subtitle);
    mainLayout->addWidget(banner);
    mainLayout->addLayout(form);
    mainLayout->addWidget(submitButton);
    mainLayout->addWidget(statusLabel);
    mainLayout->addStretch();

    // Connect button to the actual submission function
    connect(submitButton, &QPushButton::clicked,
            this, &VictimDashboard::submitSOS);
}

void VictimDashboard::submitSOS()
{
    QString name = nameInput->text().trimmed();
    QString location = locationInput->text().trimmed();
    QString help = helpInput->toPlainText().trimmed();
    QString description = descriptionInput->toPlainText().trimmed();

    // Validate required fields
    if (name.isEmpty() || location.isEmpty() || help.isEmpty()) {
        QMessageBox::warning(
            this,
            "Missing Information",
            "Please enter your name, location, and required help."
            );
        return;
    }

    if (disasterInput->currentIndex() == 0 ||
        urgencyInput->currentIndex() == 0) {
        QMessageBox::warning(
            this,
            "Missing Information",
            "Please select the disaster type and urgency."
            );
        return;
    }

    // Generate a unique request ID
    QString requestId =
        QUuid::createUuid()
            .toString(QUuid::WithoutBraces)
            .left(8)
            .toUpper();

    QString folder =
        QStandardPaths::writableLocation(
            QStandardPaths::AppDataLocation
            );

    if (folder.isEmpty() || !QDir().mkpath(folder)) {
        QMessageBox::critical(
            this,
            "Storage Error",
            "Could not create the application data folder."
            );
        return;
    }

    QString filePath = folder + "/sos_requests.txt";
    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly |
                   QIODevice::Append |
                   QIODevice::Text)) {
        QMessageBox::critical(
            this,
            "Storage Error",
            "Could not save your SOS request."
            );
        return;
    }

    QTextStream out(&file);

    // Use a separator between fields so the record remains readable.
    out << "Request ID: " << requestId << '\n'
        << "Victim: " << name << '\n'
        << "Location: " << location << '\n'
        << "Disaster: " << disasterInput->currentText() << '\n'
        << "Urgency: " << urgencyInput->currentText() << '\n'
        << "Help Required: " << help << '\n'
        << "Description: " << description << '\n'
        << "Created At: "
        << QDateTime::currentDateTime().toString(Qt::ISODate)
        << '\n'
        << "Status: Pending\n"
        << "----------------------------------------\n";

    file.close();

    statusLabel->setStyleSheet(
        "color: #167244; font-weight: bold;"
        );

    statusLabel->setText(
        "SOS request saved successfully!\n"
        "Request ID: " + requestId +
        "\nStatus: Pending"
        );

    QMessageBox::information(
        this,
        "SOS Request Created",
        "Your SOS request has been saved locally.\n\n"
        "Request ID: " + requestId +
            "\nStatus: Pending"
        );
}
