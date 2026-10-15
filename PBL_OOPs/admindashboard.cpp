
#include "admindashboard.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QListWidgetItem>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QStandardPaths>
#include <QMessageBox>
#include <QStringList>
#include <QAbstractItemView>
#include <QSizePolicy>
#include <QResizeEvent>

// ==================================================
// RESPONSIVE MULTILINE LIST
// ==================================================

ResponsiveListWidget::ResponsiveListWidget(QWidget *parent)
    : QListWidget(parent)
{
    setWordWrap(true);
    setUniformItemSizes(false);
    setSpacing(6);

    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Expanding
        );
}

void ResponsiveListWidget::addMultilineCard(const QString &text)
{
    QListWidgetItem *item = new QListWidgetItem(text);
    addItem(item);

    QLabel *details = new QLabel(text);

    details->setWordWrap(true);
    details->setContentsMargins(10, 8, 10, 8);
    details->setMinimumWidth(0);

    // Let mouse clicks reach the list item so it can be selected.
    details->setAttribute(Qt::WA_TransparentForMouseEvents);

    details->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Preferred
        );

    details->setStyleSheet(
        "QLabel {"
        " background-color: #F3F7FC;"
        " color: #172B4D;"
        " border-radius: 6px;"
        " padding: 6px;"
        "}"
        );

    setItemWidget(item, details);

    resizeCards();
}

void ResponsiveListWidget::resizeEvent(QResizeEvent *event)
{
    QListWidget::resizeEvent(event);
    resizeCards();
}

void ResponsiveListWidget::resizeCards()
{
    int availableWidth = viewport()->width() - 8;

    if (availableWidth < 100)
        return;

    for (int i = 0; i < count(); ++i) {
        QListWidgetItem *currentItem = item(i);
        QLabel *details =
            qobject_cast<QLabel *>(itemWidget(currentItem));

        if (!details)
            continue;

        details->setFixedWidth(availableWidth);
        details->adjustSize();

        currentItem->setSizeHint(
            QSize(availableWidth, details->sizeHint().height())
            );
    }
}

// ==================================================
// ADMIN DASHBOARD CONSTRUCTOR
// ==================================================

AdminDashboard::AdminDashboard(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("SOS Connect - Administrator Dashboard");
    setMinimumSize(1050, 750);

    setStyleSheet(
        "QWidget {"
        " background-color: #0B1B33;"
        " color: white;"
        " font-family: 'Segoe UI';"
        " font-size: 11pt;"
        "}"
        "QComboBox {"
        " background-color: white;"
        " color: #172B4D;"
        " border: 1px solid #B7C9DD;"
        " border-radius: 7px;"
        " padding: 7px;"
        "}"
        "QPushButton {"
        " background-color: #1672D5;"
        " color: white;"
        " border: none;"
        " border-radius: 7px;"
        " padding: 10px 14px;"
        " font-weight: bold;"
        "}"
        "QPushButton:hover {"
        " background-color: #328BEA;"
        "}"
        );

    // Main heading
    QLabel *title = new QLabel("Administrator Dashboard");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(
        "font-size: 25pt;"
        "font-weight: bold;"
        );

    QLabel *subtitle = new QLabel(
        "Monitor requests and coordinate relief activities."
        );
    subtitle->setAlignment(Qt::AlignCenter);
    subtitle->setStyleSheet("color: #C6D5E8;");

    // Statistics labels
    requestCountLabel = new QLabel("Total SOS Requests: 0");
    resourceCountLabel = new QLabel("Relief Resources: 0");

    pendingCountLabel = new QLabel("Pending: 0");
    acceptedCountLabel = new QLabel("Accepted: 0");
    progressCountLabel = new QLabel("In Progress: 0");
    resolvedCountLabel = new QLabel("Resolved: 0");

    QString statsStyle =
        "QLabel {"
        " background-color: #142B49;"
        " color: white;"
        " border: 1px solid #2E527B;"
        " border-radius: 8px;"
        " padding: 9px;"
        " font-size: 12pt;"
        " font-weight: bold;"
        "}";

    requestCountLabel->setStyleSheet(statsStyle);
    resourceCountLabel->setStyleSheet(statsStyle);
    pendingCountLabel->setStyleSheet(statsStyle);
    acceptedCountLabel->setStyleSheet(statsStyle);
    progressCountLabel->setStyleSheet(statsStyle);
    resolvedCountLabel->setStyleSheet(statsStyle);

    // Section headings
    QLabel *statsHeading = new QLabel("Live Overview");
    statsHeading->setStyleSheet(
        "font-size: 16pt; font-weight: bold;"
        );

    QLabel *requestHeading =
        new QLabel("Emergency SOS Requests");

    requestHeading->setStyleSheet(
        "font-size: 15pt; font-weight: bold;"
        );

    QLabel *resourceHeading =
        new QLabel("NGO Relief Resources");

    resourceHeading->setStyleSheet(
        "font-size: 15pt; font-weight: bold;"
        );

    // Multiline request and resource lists
    requestList = new ResponsiveListWidget(this);
    resourceList = new ResponsiveListWidget(this);

    // Status controls
    statusInput = new QComboBox(this);

    statusInput->addItems({
        "Pending",
        "Accepted",
        "In Progress",
        "Resolved"
    });

    updateStatusButton =
        new QPushButton("Update Selected Request", this);

    refreshButton =
        new QPushButton("Refresh Dashboard", this);

    statusLabel = new QLabel("Dashboard ready.", this);
    statusLabel->setWordWrap(true);
    statusLabel->setStyleSheet("color: #A9D6FF;");

    // Summary row
    QHBoxLayout *summaryLayout = new QHBoxLayout;
    summaryLayout->setSpacing(10);
    summaryLayout->addWidget(requestCountLabel);
    summaryLayout->addWidget(resourceCountLabel);

    // Status statistics
    QGridLayout *statsLayout = new QGridLayout;
    statsLayout->setSpacing(8);

    statsLayout->addWidget(pendingCountLabel, 0, 0);
    statsLayout->addWidget(acceptedCountLabel, 0, 1);
    statsLayout->addWidget(progressCountLabel, 1, 0);
    statsLayout->addWidget(resolvedCountLabel, 1, 1);

    // Request panel
    QVBoxLayout *requestsLayout = new QVBoxLayout;
    requestsLayout->setSpacing(8);

    requestsLayout->addWidget(requestHeading);
    requestsLayout->addWidget(requestList, 1);

    QHBoxLayout *statusLayout = new QHBoxLayout;
    statusLayout->setSpacing(8);

    QLabel *newStatusLabel = new QLabel("New status:");

    statusLayout->addWidget(newStatusLabel);
    statusLayout->addWidget(statusInput, 1);
    statusLayout->addWidget(updateStatusButton);

    requestsLayout->addLayout(statusLayout);

    // Resource panel
    QVBoxLayout *resourcesLayout = new QVBoxLayout;
    resourcesLayout->setSpacing(8);

    resourcesLayout->addWidget(resourceHeading);
    resourcesLayout->addWidget(resourceList, 1);

    // Side-by-side panels
    QHBoxLayout *listsLayout = new QHBoxLayout;
    listsLayout->setSpacing(16);

    listsLayout->addLayout(requestsLayout, 3);
    listsLayout->addLayout(resourcesLayout, 2);

    // Main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    mainLayout->setContentsMargins(24, 18, 24, 18);
    mainLayout->setSpacing(9);

    mainLayout->addWidget(title);
    mainLayout->addWidget(subtitle);
    mainLayout->addLayout(summaryLayout);
    mainLayout->addWidget(statsHeading);
    mainLayout->addLayout(statsLayout);
    mainLayout->addLayout(listsLayout, 1);
    mainLayout->addWidget(refreshButton);
    mainLayout->addWidget(statusLabel);

    // Connections
    connect(
        refreshButton,
        &QPushButton::clicked,
        this,
        &AdminDashboard::refreshDashboard
        );

    connect(
        updateStatusButton,
        &QPushButton::clicked,
        this,
        &AdminDashboard::updateRequestStatus
        );

    refreshDashboard();
}

// ==================================================
// LOAD SOS REQUESTS
// ==================================================

void AdminDashboard::loadRequests()
{
    requestList->clear();

    int total = 0;
    int pending = 0;
    int accepted = 0;
    int inProgress = 0;
    int resolved = 0;

    QString folder =
        QStandardPaths::writableLocation(
            QStandardPaths::AppDataLocation
            );

    QDir().mkpath(folder);

    QFile file(folder + "/sos_requests.txt");

    if (!file.exists()) {
        requestCountLabel->setText("Total SOS Requests: 0");
        pendingCountLabel->setText("Pending: 0");
        acceptedCountLabel->setText("Accepted: 0");
        progressCountLabel->setText("In Progress: 0");
        resolvedCountLabel->setText("Resolved: 0");

        requestList->addItem("No SOS requests found.");
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        requestCountLabel->setText(
            "Total SOS Requests: unavailable"
            );
        requestList->addItem("Unable to read SOS requests.");
        return;
    }

    QTextStream in(&file);
    QString record;

    auto processRecord = [&](const QString &savedRecord) {
        QString cleanedRecord = savedRecord.trimmed();

        if (cleanedRecord.isEmpty())
            return;

        requestList->addMultilineCard(cleanedRecord);
        total++;

        QStringList lines = cleanedRecord.split('\n');

        for (const QString &field : lines) {
            QString value = field.trimmed();

            if (!value.startsWith(
                    "Status:",
                    Qt::CaseInsensitive
                    )) {
                continue;
            }

            QString status = value.section(':', 1).trimmed();

            if (status.compare("Pending", Qt::CaseInsensitive) == 0)
                pending++;
            else if (status.compare("Accepted", Qt::CaseInsensitive) == 0)
                accepted++;
            else if (status.compare("In Progress", Qt::CaseInsensitive) == 0)
                inProgress++;
            else if (status.compare("Resolved", Qt::CaseInsensitive) == 0)
                resolved++;

            break;
        }
    };

    while (!in.atEnd()) {
        QString line = in.readLine();

        if (line.trimmed() ==
            "----------------------------------------") {
            processRecord(record);
            record.clear();
        }
        else {
            if (!record.isEmpty())
                record += "\n";

            record += line;
        }
    }

    processRecord(record);
    file.close();

    requestCountLabel->setText(
        QString("Total SOS Requests: %1").arg(total)
        );

    pendingCountLabel->setText(
        QString("Pending: %1").arg(pending)
        );

    acceptedCountLabel->setText(
        QString("Accepted: %1").arg(accepted)
        );

    progressCountLabel->setText(
        QString("In Progress: %1").arg(inProgress)
        );

    resolvedCountLabel->setText(
        QString("Resolved: %1").arg(resolved)
        );
}

// ==================================================
// LOAD NGO RESOURCES
// ==================================================

void AdminDashboard::loadResources()
{
    resourceList->clear();

    QString folder =
        QStandardPaths::writableLocation(
            QStandardPaths::AppDataLocation
            );

    QDir().mkpath(folder);

    QFile file(folder + "/ngo_resources.txt");

    if (!file.exists()) {
        resourceCountLabel->setText("Relief Resources: 0");
        resourceList->addItem("No relief resources found.");
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        resourceCountLabel->setText(
            "Relief Resources: unavailable"
            );
        resourceList->addItem("Unable to read relief resources.");
        return;
    }

    QTextStream in(&file);
    QString record;
    int count = 0;

    auto processRecord = [&](const QString &savedRecord) {
        QString cleanedRecord = savedRecord.trimmed();

        if (cleanedRecord.isEmpty())
            return;

        resourceList->addMultilineCard(cleanedRecord);
        count++;
    };

    while (!in.atEnd()) {
        QString line = in.readLine();

        if (line.trimmed() ==
            "----------------------------------------") {
            processRecord(record);
            record.clear();
        }
        else {
            if (!record.isEmpty())
                record += "\n";

            record += line;
        }
    }

    processRecord(record);
    file.close();

    resourceCountLabel->setText(
        QString("Relief Resources: %1").arg(count)
        );
}

// ==================================================
// REFRESH DASHBOARD
// ==================================================

void AdminDashboard::refreshDashboard()
{
    loadRequests();
    loadResources();

    statusLabel->setText(
        "Dashboard refreshed from local storage."
        );
}

// ==================================================
// UPDATE REQUEST STATUS
// ==================================================

void AdminDashboard::updateRequestStatus()
{
    int selectedRow = requestList->currentRow();

    if (selectedRow < 0) {
        QMessageBox::warning(
            this,
            "No Request Selected",
            "Please select an SOS request first."
            );
        return;
    }

    QString selectedRecord =
        requestList->item(selectedRow)->text();

    QString requestIdLine =
        selectedRecord.section('\n', 0, 0).trimmed();

    if (!requestIdLine.startsWith(
            "Request ID:",
            Qt::CaseInsensitive
            )) {
        QMessageBox::warning(
            this,
            "Invalid Selection",
            "Select a valid SOS request first."
            );
        return;
    }

    QString requestId =
        requestIdLine.section(':', 1).trimmed();

    QString folder =
        QStandardPaths::writableLocation(
            QStandardPaths::AppDataLocation
            );

    QString filePath = folder + "/sos_requests.txt";

    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(
            this,
            "File Error",
            "Could not open the SOS request file."
            );
        return;
    }

    QTextStream in(&file);
    QStringList records;
    QString record;

    while (!in.atEnd()) {
        QString line = in.readLine();

        if (line.trimmed() ==
            "----------------------------------------") {
            if (!record.isEmpty())
                records.append(record);

            record.clear();
        }
        else {
            if (!record.isEmpty())
                record += "\n";

            record += line;
        }
    }

    if (!record.isEmpty())
        records.append(record);

    file.close();

    bool found = false;
    QString newStatus = statusInput->currentText();

    for (QString &savedRecord : records) {
        QString savedIdLine =
            savedRecord.section('\n', 0, 0).trimmed();

        if (savedIdLine.compare(
                requestIdLine,
                Qt::CaseInsensitive
                ) != 0) {
            continue;
        }

        QStringList lines = savedRecord.split('\n');

        for (QString &line : lines) {
            if (line.startsWith(
                    "Status:",
                    Qt::CaseInsensitive
                    )) {
                line = "Status: " + newStatus;
                found = true;
                break;
            }
        }

        if (found)
            savedRecord = lines.join('\n');

        break;
    }

    if (!found) {
        QMessageBox::warning(
            this,
            "Update Failed",
            "Request not found, or its status field is missing."
            );
        return;
    }

    if (!file.open(
            QIODevice::WriteOnly |
            QIODevice::Text |
            QIODevice::Truncate
            )) {
        QMessageBox::critical(
            this,
            "File Error",
            "Could not save the updated request."
            );
        return;
    }

    QTextStream out(&file);

    for (const QString &savedRecord : records) {
        out << savedRecord << "\n";
        out << "----------------------------------------\n";
    }

    file.close();

    refreshDashboard();

    statusLabel->setText(
        "Request " + requestId +
        " updated to: " + newStatus
        );

    QMessageBox::information(
        this,
        "Status Updated",
        "The selected SOS request status has been updated."
        );
}
