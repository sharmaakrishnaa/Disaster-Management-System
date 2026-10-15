
#include "volunteerdashboard.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QStandardPaths>
#include <QMessageBox>

VolunteerDashboard::VolunteerDashboard(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("SOS Connect - Volunteer Dashboard");
    resize(850, 650);

    setStyleSheet(
        "QWidget {"
        " background-color: #F3F6FB;"
        " color: #172B46;"
        " font-family: 'Segoe UI';"
        " font-size: 11pt;"
        "}"
        "QListWidget {"
        " background-color: white;"
        " border: 1px solid #CBD5E1;"
        " border-radius: 8px;"
        " padding: 8px;"
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

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 25, 30, 25);
    mainLayout->setSpacing(15);

    QLabel *heading = new QLabel("Volunteer Dashboard");
    heading->setStyleSheet(
        "font-size: 25pt;"
        "font-weight: bold;"
        "color: #102747;"
        );

    QLabel *subtitle = new QLabel(
        "View pending SOS requests and coordinate assistance."
        );
    subtitle->setStyleSheet("color: #64748B;");

    requestList = new QListWidget;
    requestList->setWordWrap(true);

    refreshButton = new QPushButton("Refresh Requests");
    acceptButton = new QPushButton("Accept Selected Request");

    statusLabel = new QLabel("Loading saved SOS requests...");
    statusLabel->setWordWrap(true);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(refreshButton);
    buttonLayout->addWidget(acceptButton);

    mainLayout->addWidget(heading);
    mainLayout->addWidget(subtitle);
    mainLayout->addWidget(requestList, 1);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(statusLabel);

    connect(refreshButton, &QPushButton::clicked,
            this, &VolunteerDashboard::loadRequests);

    connect(acceptButton, &QPushButton::clicked,
            this, &VolunteerDashboard::acceptRequest);

    loadRequests();
}

void VolunteerDashboard::loadRequests()
{
    requestList->clear();

    QString folder =
        QStandardPaths::writableLocation(
            QStandardPaths::AppDataLocation
            );

    QFile file(folder + "/sos_requests.txt");

    if (!file.exists()) {
        statusLabel->setText(
            "No saved requests found yet. "
            "Submit an SOS from the Victim Dashboard first."
            );
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        statusLabel->setText("Unable to read the SOS request file.");
        return;
    }

    QTextStream in(&file);
    QString currentRecord;

    while (!in.atEnd()) {
        QString line = in.readLine();

        if (line.startsWith("----------------------------------------")) {
            if (!currentRecord.trimmed().isEmpty()) {
                QListWidgetItem *item =
                    new QListWidgetItem(currentRecord.trimmed());

                requestList->addItem(item);
                currentRecord.clear();
            }
        } else {
            currentRecord += line + "\n";
        }
    }

    if (!currentRecord.trimmed().isEmpty()) {
        requestList->addItem(currentRecord.trimmed());
    }

    file.close();

    statusLabel->setText(
        QString("Loaded %1 saved request(s).")
            .arg(requestList->count())
        );
}

void VolunteerDashboard::acceptRequest()
{
    QListWidgetItem *selectedItem = requestList->currentItem();

    if (!selectedItem) {
        QMessageBox::warning(
            this,
            "No Request Selected",
            "Please select an SOS request first."
            );
        return;
    }

    QString record = selectedItem->text();

    if (record.contains("Status: Accepted")) {
        QMessageBox::information(
            this,
            "Already Accepted",
            "This request has already been accepted."
            );
        return;
    }

    if (!record.contains("Status: Pending")) {
        QMessageBox::information(
            this,
            "Request Status",
            "Only pending requests can be accepted."
            );
        return;
    }

    QString folder =
        QStandardPaths::writableLocation(
            QStandardPaths::AppDataLocation
            );

    QString filePath = folder + "/sos_requests.txt";
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(
            this,
            "Error",
            "Could not open the saved requests."
            );
        return;
    }

    QStringList records;
    QTextStream in(&file);
    QString currentRecord;

    while (!in.atEnd()) {
        QString line = in.readLine();

        if (line.startsWith("----------------------------------------")) {
            if (!currentRecord.trimmed().isEmpty()) {
                records.append(currentRecord.trimmed());
                currentRecord.clear();
            }
        } else {
            currentRecord += line + "\n";
        }
    }

    if (!currentRecord.trimmed().isEmpty()) {
        records.append(currentRecord.trimmed());
    }

    file.close();

    bool found = false;

    for (QString &savedRecord : records) {
        QString requestIdLine = record.section('\n', 0, 0);

        if (savedRecord.section('\n', 0, 0) == requestIdLine) {
            savedRecord.replace("Status: Pending", "Status: Accepted");
            found = true;
            break;
        }
    }

    if (!found) {
        QMessageBox::warning(
            this,
            "Request Not Found",
            "Could not match this request in saved records."
            );
        loadRequests();
        return;
    }

    if (!file.open(QIODevice::WriteOnly |
                   QIODevice::Truncate |
                   QIODevice::Text)) {
        QMessageBox::critical(
            this,
            "Save Error",
            "Could not update the request status."
            );
        return;
    }

    QTextStream out(&file);

    for (const QString &savedRecord : records) {
        out << savedRecord << "\n"
            << "----------------------------------------\n";
    }

    file.close();

    QMessageBox::information(
        this,
        "Request Accepted",
        "The SOS request has been marked as Accepted."
        );

    loadRequests();
}
