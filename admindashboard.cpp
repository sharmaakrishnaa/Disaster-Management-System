
#include "admindashboard.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
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
#include <QScrollArea>
#include <QFrame>
#include <QSize>
#include <QDateTime>
#include <QRegularExpression>
#include <utility>

// ==================================================
// RESPONSIVE LIST WIDGET
// ==================================================

// INHERITANCE:
// This class extends QListWidget with responsive cards.

ResponsiveListWidget::ResponsiveListWidget(QWidget *parent)
    : QListWidget(parent)
{
    setWordWrap(true);
    setUniformItemSizes(false);
    setSpacing(8);

    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setSelectionMode(QAbstractItemView::SingleSelection);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Keep the selected card readable.
    connect(this, &QListWidget::itemSelectionChanged,
            this, [this]() {

                for (int i = 0; i < count(); ++i) {
                    QListWidgetItem *currentItem = item(i);

                    QLabel *details =
                        qobject_cast<QLabel *>(itemWidget(currentItem));

                    if (!details)
                        continue;

                    if (currentItem->isSelected()) {
                        details->setStyleSheet(
                            "QLabel {"
                            " background-color: #17365D;"
                            " color: white;"
                            " border-radius: 8px;"
                            " padding: 12px;"
                            "}"
                            );
                    }
                    else {
                        details->setStyleSheet(
                            "QLabel {"
                            " background-color: #F3F7FC;"
                            " color: #172B4D;"
                            " border-radius: 8px;"
                            " padding: 12px;"
                            "}"
                            );
                    }
                }
            });
}

// ENCAPSULATION:
// Creates one multiline information card.

void ResponsiveListWidget::addMultilineCard(const QString &text)
{
    QListWidgetItem *newItem = new QListWidgetItem(text);
    addItem(newItem);

    QLabel *details = new QLabel(text);
    details->setWordWrap(true);
    details->setContentsMargins(4, 4, 4, 4);
    details->setMinimumWidth(0);

    // Mouse clicks should reach the list item.
    details->setAttribute(Qt::WA_TransparentForMouseEvents);

    details->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Preferred
        );

    details->setStyleSheet(
        "QLabel {"
        " background-color: #F3F7FC;"
        " color: #172B4D;"
        " border-radius: 8px;"
        " padding: 12px;"
        "}"
        );

    setItemWidget(newItem, details);
    resizeCards();
}

// POLYMORPHISM:
// Overrides the inherited resize event.

void ResponsiveListWidget::resizeEvent(QResizeEvent *event)
{
    QListWidget::resizeEvent(event);
    resizeCards();
}

// ENCAPSULATION:
// Adjusts the size of every card.

void ResponsiveListWidget::resizeCards()
{
    int availableWidth = viewport()->width() - 12;

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
            QSize(availableWidth, details->sizeHint().height() + 8)
            );
    }
}

// ==================================================
// ADMIN DASHBOARD CONSTRUCTOR
// ==================================================

// INHERITANCE: AdminDashboard inherits QWidget.
// ENCAPSULATION: Dashboard components and operations
// are grouped inside this class.

AdminDashboard::AdminDashboard(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("SOS Connect - Administrator Dashboard");
    setMinimumSize(750, 650);
    resize(1150, 900);

    setStyleSheet(
        "QWidget {"
        " font-family: 'Segoe UI';"
        " color: #172B4D;"
        " font-size: 10pt;"
        "}"
        "QScrollArea {"
        " border: none;"
        " background-color: #F3F6FB;"
        "}"
        "QPushButton {"
        " background-color: #1672D5;"
        " color: white;"
        " border: none;"
        " border-radius: 8px;"
        " padding: 11px 18px;"
        " font-weight: bold;"
        "}"
        "QPushButton:hover {"
        " background-color: #328BEA;"
        "}"
        "QComboBox {"
        " background-color: white;"
        " color: #172B4D;"
        " border: 1px solid #CBD5E1;"
        " border-radius: 7px;"
        " padding: 9px;"
        " min-height: 24px;"
        "}"
        "QLineEdit {"
        " background-color: white;"
        " color: #172B4D;"
        " border: 1px solid #CBD5E1;"
        " border-radius: 7px;"
        " padding: 10px;"
        " min-height: 22px;"
        "}"
        );

    // ==============================================
    // MAIN SCROLL AREA
    // ==============================================

    // ABSTRACTION:
    // QScrollArea handles scrolling automatically.

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    QWidget *contentWidget = new QWidget;
    contentWidget->setStyleSheet("background-color: #F3F6FB;");

    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(24, 22, 24, 28);
    contentLayout->setSpacing(20);

    // ==============================================
    // HEADER
    // ==============================================

    QFrame *headerFrame = new QFrame;
    headerFrame->setStyleSheet(
        "QFrame {"
        " background-color: #0B1B33;"
        " border-radius: 14px;"
        "}"
        );

    QVBoxLayout *headerLayout = new QVBoxLayout(headerFrame);
    headerLayout->setContentsMargins(24, 24, 24, 24);
    headerLayout->setSpacing(8);

    QLabel *title = new QLabel("Administrator Dashboard");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(
        "color: white;"
        "font-size: 25pt;"
        "font-weight: bold;"
        );

    QLabel *subtitle = new QLabel(
        "Monitor emergency requests and coordinate relief activities."
        );
    subtitle->setAlignment(Qt::AlignCenter);
    subtitle->setWordWrap(true);
    subtitle->setStyleSheet(
        "color: #C6D5E8;"
        "font-size: 11pt;"
        );

    headerLayout->addWidget(title);
    headerLayout->addWidget(subtitle);

    contentLayout->addWidget(headerFrame);

    // ==============================================
    // STATISTICS
    // ==============================================

    QLabel *statsHeading = new QLabel("Live Overview");
    statsHeading->setStyleSheet(
        "font-size: 18pt;"
        "font-weight: bold;"
        "color: #0B1B33;"
        );

    contentLayout->addWidget(statsHeading);

    requestCountLabel = new QLabel("Total SOS Requests: 0");
    resourceCountLabel = new QLabel("Relief Resources: 0");

    pendingCountLabel = new QLabel("Pending: 0");
    acceptedCountLabel = new QLabel("Accepted: 0");
    progressCountLabel = new QLabel("In Progress: 0");
    resolvedCountLabel = new QLabel("Resolved: 0");

    QString statsStyle =
        "QLabel {"
        " background-color: white;"
        " color: #17365D;"
        " border: 1px solid #D8E2EF;"
        " border-radius: 10px;"
        " padding: 17px;"
        " font-size: 12pt;"
        " font-weight: bold;"
        "}";

    requestCountLabel->setStyleSheet(statsStyle);
    resourceCountLabel->setStyleSheet(statsStyle);
    pendingCountLabel->setStyleSheet(statsStyle);
    acceptedCountLabel->setStyleSheet(statsStyle);
    progressCountLabel->setStyleSheet(statsStyle);
    resolvedCountLabel->setStyleSheet(statsStyle);

    requestCountLabel->setMinimumHeight(55);
    resourceCountLabel->setMinimumHeight(55);

    pendingCountLabel->setMinimumHeight(50);
    acceptedCountLabel->setMinimumHeight(50);
    progressCountLabel->setMinimumHeight(50);
    resolvedCountLabel->setMinimumHeight(50);

    QGridLayout *statsLayout = new QGridLayout;
    statsLayout->setSpacing(12);

    statsLayout->addWidget(requestCountLabel, 0, 0);
    statsLayout->addWidget(resourceCountLabel, 0, 1);
    statsLayout->addWidget(pendingCountLabel, 1, 0);
    statsLayout->addWidget(acceptedCountLabel, 1, 1);
    statsLayout->addWidget(progressCountLabel, 2, 0);
    statsLayout->addWidget(resolvedCountLabel, 2, 1);

    contentLayout->addLayout(statsLayout);

    // ==============================================
    // SOS REQUESTS SECTION
    // ==============================================

    QFrame *requestFrame = new QFrame;
    requestFrame->setStyleSheet(
        "QFrame {"
        " background-color: white;"
        " border: 1px solid #D8E2EF;"
        " border-radius: 12px;"
        "}"
        );

    QVBoxLayout *requestsLayout = new QVBoxLayout(requestFrame);
    requestsLayout->setContentsMargins(20, 20, 20, 20);
    requestsLayout->setSpacing(14);

    QLabel *requestHeading =
        new QLabel("Emergency SOS Requests");

    requestHeading->setStyleSheet(
        "font-size: 17pt;"
        "font-weight: bold;"
        "color: #0B1B33;"
        "border: none;"
        );

    QLabel *requestDescription = new QLabel(
        "View submitted emergency requests and update their status."
        );
    requestDescription->setWordWrap(true);
    requestDescription->setStyleSheet(
        "color: #64748B;"
        "border: none;"
        );

    requestList = new ResponsiveListWidget;
    requestList->setMinimumHeight(380);

    requestList->setStyleSheet(
        "QListWidget {"
        " background-color: #FFFFFF;"
        " color: #172B4D;"
        " border: 1px solid #D5DFEA;"
        " border-radius: 9px;"
        " padding: 8px;"
        " outline: none;"
        "}"
        "QListWidget::item {"
        " padding: 5px;"
        " margin: 4px;"
        "}"
        );

    statusInput = new QComboBox;
    statusInput->addItems({
        "Pending",
        "Accepted",
        "In Progress",
        "Resolved"
    });

    updateStatusButton =
        new QPushButton("Update Selected Request");

    updateStatusButton->setObjectName("updateStatusButton");
    updateStatusButton->setMinimumHeight(46);

    // Specific button styling overrides the global bright blue style.
    updateStatusButton->setStyleSheet(
        "QPushButton#updateStatusButton {"
        " background-color: #0B1B33;"
        " color: white;"
        " border: 1px solid #0B1B33;"
        " border-radius: 8px;"
        " padding: 12px 18px;"
        " font-weight: bold;"
        "}"
        "QPushButton#updateStatusButton:hover {"
        " background-color: #17365D;"
        " color: white;"
        "}"
        "QPushButton#updateStatusButton:pressed {"
        " background-color: #071426;"
        " color: white;"
        "}"
        "QPushButton#updateStatusButton:disabled {"
        " background-color: #94A3B8;"
        " color: white;"
        "}"
        );

    QLabel *newStatusLabel = new QLabel("New request status:");
    newStatusLabel->setStyleSheet(
        "font-weight: bold;"
        "border: none;"
        );

    QVBoxLayout *statusControlLayout = new QVBoxLayout;
    statusControlLayout->setSpacing(8);

    statusControlLayout->addWidget(newStatusLabel);
    statusControlLayout->addWidget(statusInput);
    statusControlLayout->addWidget(updateStatusButton);

    requestsLayout->addWidget(requestHeading);
    requestsLayout->addWidget(requestDescription);
    requestsLayout->addWidget(requestList, 1);
    requestsLayout->addLayout(statusControlLayout);

    contentLayout->addWidget(requestFrame);

    // ==============================================
    // NGO AND ADMIN RESOURCES SECTION
    // ==============================================

    QFrame *resourceFrame = new QFrame;
    resourceFrame->setStyleSheet(
        "QFrame {"
        " background-color: white;"
        " border: 1px solid #D8E2EF;"
        " border-radius: 12px;"
        "}"
        );

    QVBoxLayout *resourcesLayout = new QVBoxLayout(resourceFrame);
    resourcesLayout->setContentsMargins(20, 20, 20, 20);
    resourcesLayout->setSpacing(14);

    QLabel *resourceHeading =
        new QLabel("NGO & Administrator Relief Resources");

    resourceHeading->setStyleSheet(
        "font-size: 17pt;"
        "font-weight: bold;"
        "color: #0B1B33;"
        "border: none;"
        );

    QLabel *resourceDescription = new QLabel(
        "View resources added by NGOs and add additional resources "
        "from the administrator account."
        );

    resourceDescription->setWordWrap(true);
    resourceDescription->setStyleSheet(
        "color: #64748B;"
        "border: none;"
        );

    // Resource list displays records from the shared file.
    resourceList = new ResponsiveListWidget;
    resourceList->setMinimumHeight(300);

    resourceList->setStyleSheet(
        "QListWidget {"
        " background-color: #FFFFFF;"
        " color: #172B4D;"
        " border: 1px solid #D5DFEA;"
        " border-radius: 9px;"
        " padding: 8px;"
        " outline: none;"
        "}"
        "QListWidget::item {"
        " padding: 5px;"
        " margin: 4px;"
        "}"
        );

    // ==============================================
    // ADMIN RESOURCE ADD FORM
    // ==============================================

    QLabel *addResourceHeading =
        new QLabel("Add Relief Resource");

    addResourceHeading->setStyleSheet(
        "font-size: 14pt;"
        "font-weight: bold;"
        "color: #17365D;"
        "border: none;"
        );

    QLabel *typeLabel = new QLabel("Resource Type");
    QLabel *quantityLabel = new QLabel("Quantity");
    QLabel *locationLabel = new QLabel("Location");

    typeLabel->setStyleSheet("border: none; font-weight: bold;");
    quantityLabel->setStyleSheet("border: none; font-weight: bold;");
    locationLabel->setStyleSheet("border: none; font-weight: bold;");

    adminResourceType = new QComboBox;
    adminResourceType->addItems({
        "Food",
        "Drinking Water",
        "Medicines",
        "Blankets",
        "Clothing",
        "Shelter",
        "First Aid Kits",
        "Other"
    });

    adminResourceQuantity = new QLineEdit;
    adminResourceQuantity->setPlaceholderText(
        "Enter available quantity"
        );

    adminResourceLocation = new QLineEdit;
    adminResourceLocation->setPlaceholderText(
        "Enter storage or distribution location"
        );

    QGridLayout *resourceFormLayout = new QGridLayout;
    resourceFormLayout->setHorizontalSpacing(12);
    resourceFormLayout->setVerticalSpacing(8);

    resourceFormLayout->addWidget(typeLabel, 0, 0);
    resourceFormLayout->addWidget(quantityLabel, 0, 1);

    resourceFormLayout->addWidget(adminResourceType, 1, 0);
    resourceFormLayout->addWidget(adminResourceQuantity, 1, 1);

    resourceFormLayout->addWidget(locationLabel, 2, 0, 1, 2);
    resourceFormLayout->addWidget(adminResourceLocation, 3, 0, 1, 2);

    addResourceButton = new QPushButton("Add Resource");
    addResourceButton->setMinimumHeight(45);

    addResourceButton->setStyleSheet(
        "QPushButton {"
        " background-color: #087E65;"
        " color: white;"
        " border: none;"
        " border-radius: 8px;"
        " padding: 12px 18px;"
        " font-weight: bold;"
        "}"
        "QPushButton:hover {"
        " background-color: #066650;"
        "}"
        "QPushButton:pressed {"
        " background-color: #044D3D;"
        "}"
        );

    resourcesLayout->addWidget(resourceHeading);
    resourcesLayout->addWidget(resourceDescription);
    resourcesLayout->addWidget(resourceList, 1);

    resourcesLayout->addWidget(addResourceHeading);
    resourcesLayout->addLayout(resourceFormLayout);
    resourcesLayout->addWidget(addResourceButton);

    contentLayout->addWidget(resourceFrame);

    // ==============================================
    // REFRESH AND STATUS
    // ==============================================

    refreshButton = new QPushButton("Refresh Dashboard");
    refreshButton->setMinimumHeight(45);

    statusLabel = new QLabel("Dashboard ready.");
    statusLabel->setWordWrap(true);
    statusLabel->setStyleSheet(
        "background-color: #E3F2FD;"
        "color: #174A75;"
        "border: 1px solid #BFDBFE;"
        "border-radius: 8px;"
        "padding: 12px;"
        );

    contentLayout->addWidget(refreshButton);
    contentLayout->addWidget(statusLabel);

    contentLayout->addStretch();

    // Put all dashboard content inside the scroll area.
    scrollArea->setWidget(contentWidget);

    QVBoxLayout *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->addWidget(scrollArea);

    // ==============================================
    // SIGNALS AND SLOTS
    // ==============================================

    // Event-driven programming:
    // Button clicks call the corresponding member functions.

    connect(refreshButton, &QPushButton::clicked,
            this, &AdminDashboard::refreshDashboard);

    connect(updateStatusButton, &QPushButton::clicked,
            this, &AdminDashboard::updateRequestStatus);

    connect(addResourceButton, &QPushButton::clicked,
            this, &AdminDashboard::addAdminResource);

    refreshDashboard();
}

// ==================================================
// LOAD SOS REQUESTS
// ==================================================

// ENCAPSULATION:
// Reads saved SOS requests and updates the statistics.

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
        requestCountLabel->setText("Total SOS Requests: unavailable");
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

        for (const QString &field : std::as_const(lines)) {
            QString value = field.trimmed();

            if (!value.startsWith("Status:", Qt::CaseInsensitive))
                continue;

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

        if (line.trimmed() == "----------------------------------------") {
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

    if (total == 0)
        requestList->addItem("No SOS requests found.");

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
// LOAD NGO AND ADMIN RESOURCES
// ==================================================

// ENCAPSULATION:
// Loads all resources from the shared resource file.
// Both NGO-added and administrator-added records appear here.

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
        resourceCountLabel->setText("Relief Resources: unavailable");
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

        if (line.trimmed() == "----------------------------------------") {
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

    if (count == 0)
        resourceList->addItem("No relief resources found.");

    resourceCountLabel->setText(
        QString("Relief Resources: %1").arg(count)
        );
}

// ==================================================
// ADD ADMINISTRATOR RESOURCE
// ==================================================

// ENCAPSULATION:
// This function validates and saves a new resource.
//
// ABSTRACTION:
// QFile and QTextStream handle file operations.

void AdminDashboard::addAdminResource()
{
    QString resourceType = adminResourceType->currentText().trimmed();
    QString quantityText = adminResourceQuantity->text().trimmed();
    QString location = adminResourceLocation->text().trimmed();

    // Check that all fields are filled.
    if (resourceType.isEmpty() ||
        quantityText.isEmpty() ||
        location.isEmpty()) {

        QMessageBox::warning(
            this,
            "Missing Information",
            "Please fill in the resource type, quantity and location."
            );
        return;
    }

    // Validate quantity.
    bool validQuantity = false;
    int quantity = quantityText.toInt(&validQuantity);

    if (!validQuantity || quantity <= 0) {
        QMessageBox::warning(
            this,
            "Invalid Quantity",
            "Please enter a valid positive whole number for quantity."
            );
        return;
    }

    QString folder =
        QStandardPaths::writableLocation(
            QStandardPaths::AppDataLocation
            );

    QDir().mkpath(folder);

    QFile file(folder + "/ngo_resources.txt");

    if (!file.open(QIODevice::WriteOnly |
                   QIODevice::Append |
                   QIODevice::Text)) {

        QMessageBox::critical(
            this,
            "File Error",
            "Unable to save the resource. Please try again."
            );
        return;
    }

    // Use the same record format as the NGO dashboard.
    // This allows both dashboards to read the same resource file.

    QTextStream out(&file);

    out << "NGO: SOS Connect Admin\n";
    out << "Resource: " << resourceType << "\n";
    out << "Quantity: " << quantity << "\n";
    out << "Location: " << location << "\n";
    out << "Added: "
        << QDateTime::currentDateTime().toString("dd MMM yyyy, hh:mm AP")
        << "\n";
    out << "----------------------------------------\n";

    file.close();

    // Clear the form after successful saving.
    adminResourceQuantity->clear();
    adminResourceLocation->clear();
    adminResourceType->setCurrentIndex(0);

    loadResources();

    statusLabel->setText(
        "Administrator resource added successfully."
        );

    QMessageBox::information(
        this,
        "Resource Added",
        "The relief resource has been saved successfully."
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
// UPDATE SOS REQUEST STATUS
// ==================================================

// ENCAPSULATION:
// Updates the selected request in the saved file.

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

    if (!requestIdLine.startsWith("Request ID:", Qt::CaseInsensitive)) {
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

        if (line.trimmed() == "----------------------------------------") {
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
            if (line.startsWith("Status:", Qt::CaseInsensitive)) {
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

    if (!file.open(QIODevice::WriteOnly |
                   QIODevice::Text |
                   QIODevice::Truncate)) {

        QMessageBox::critical(
            this,
            "File Error",
            "Could not save the updated request."
            );
        return;
    }

    QTextStream out(&file);

    for (const QString &savedRecord : std::as_const(records)) {
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
