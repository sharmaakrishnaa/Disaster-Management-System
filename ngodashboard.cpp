
#include "ngodashboard.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QStandardPaths>
#include <QDateTime>
#include <QAbstractItemView>
#include <QScrollArea>
#include <QFrame>
#include <QColor>
#include <QStringList>
#include <QSizePolicy>

// ==================================================
// HELPER FUNCTIONS
// ==================================================

// Extract quantity from a saved resource record.
// This helper keeps quantity-reading logic reusable.
namespace
{
int getResourceQuantity(const QString &record)
{
    QStringList lines = record.split('\n');

    for (const QString &line : std::as_const(lines)){
        if (line.startsWith("Quantity:")) {
            return line.section(':', 1).trimmed().toInt();
        }
    }

    return 0;
}

// Convert the saved file into individual resource records.
QStringList readResourceRecords(const QString &filePath)
{
    QStringList records;
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return records;

    QTextStream in(&file);
    QString record;

    while (!in.atEnd()) {
        QString line = in.readLine();

        if (line == "----------------------------------------") {
            if (!record.trimmed().isEmpty())
                records.append(record.trimmed());

            record.clear();
        }
        else {
            if (!record.isEmpty())
                record += "\n";

            record += line;
        }
    }

    if (!record.trimmed().isEmpty())
        records.append(record.trimmed());

    file.close();

    return records;
}
}

// ==================================================
// NGO DASHBOARD CONSTRUCTOR
// ==================================================

// OOP CONCEPT: INHERITANCE
// NGODashboard inherits QWidget and uses its window features.
//
// OOP CONCEPT: ENCAPSULATION
// The dashboard stores its interface controls and
// resource-management functions inside one class.

NGODashboard::NGODashboard(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("SOS Connect - NGO Dashboard");
    setMinimumSize(750, 650);
    resize(1100, 900);

    // Overall dashboard styling.
    setStyleSheet(
        "QWidget {"
        " background-color: #F3F6FB;"
        " color: #172B46;"
        " font-family: 'Segoe UI';"
        " font-size: 11pt;"
        "}"

        "QLabel {"
        " color: #172B46;"
        "}"

        "QLineEdit, QComboBox {"
        " background-color: white;"
        " color: #172B46;"
        " border: 1px solid #C5D2E2;"
        " border-radius: 7px;"
        " padding: 11px;"
        " font-size: 11pt;"
        " min-height: 25px;"
        "}"

        "QLineEdit:focus, QComboBox:focus {"
        " border: 2px solid #1672D5;"
        "}"

        "QPushButton {"
        " background-color: #1672D5;"
        " color: white;"
        " border: none;"
        " border-radius: 7px;"
        " padding: 12px;"
        " font-weight: bold;"
        " min-height: 22px;"
        "}"

        "QPushButton:hover {"
        " background-color: #328BEA;"
        "}"

        // RESOURCE LIST STYLING
        "QListWidget {"
        " background-color: white;"
        " color: #172B46;"
        " border: 1px solid #D5DFEA;"
        " border-radius: 9px;"
        " padding: 8px;"
        " font-size: 10pt;"
        " outline: none;"
        "}"

        "QListWidget::item {"
        " padding: 12px;"
        " margin: 4px;"
        " border-radius: 6px;"
        "}"

        // Dark navy selection prevents white-on-white text.
        "QListWidget::item:selected {"
        " background-color: #17365D;"
        " color: #FFFFFF;"
        " border: 2px solid #0B1B33;"
        "}"

        "QListWidget::item:selected:active {"
        " background-color: #17365D;"
        " color: #FFFFFF;"
        "}"

        "QListWidget::item:selected:!active {"
        " background-color: #294D75;"
        " color: #FFFFFF;"
        "}"

        "QScrollArea {"
        " border: none;"
        " background-color: #F3F6FB;"
        "}"
        );

    // ==================================================
    // SCROLL AREA
    // ==================================================

    // OOP CONCEPT: ABSTRACTION
    // QScrollArea handles scrolling automatically.

    QScrollArea *scrollArea = new QScrollArea(this);

    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    QWidget *contentWidget = new QWidget;
    contentWidget->setMinimumWidth(650);

    QVBoxLayout *mainLayout = new QVBoxLayout(contentWidget);

    mainLayout->setContentsMargins(35, 30, 35, 30);
    mainLayout->setSpacing(20);

    // ==================================================
    // HEADER
    // ==================================================

    QLabel *title = new QLabel("NGO Relief Dashboard");

    title->setAlignment(Qt::AlignCenter);

    title->setStyleSheet(
        "font-size: 25pt;"
        "font-weight: bold;"
        "color: #102747;"
        );

    QLabel *subtitle = new QLabel(
        "Manage and monitor disaster relief resources."
        );

    subtitle->setAlignment(Qt::AlignCenter);

    subtitle->setStyleSheet(
        "font-size: 11pt;"
        "color: #64748B;"
        );

    mainLayout->addWidget(title);
    mainLayout->addWidget(subtitle);

    // ==================================================
    // INTRODUCTION BANNER
    // ==================================================

    QFrame *banner = new QFrame;

    banner->setStyleSheet(
        "QFrame {"
        " background-color: #102747;"
        " border-radius: 12px;"
        "}"
        );

    QVBoxLayout *bannerLayout = new QVBoxLayout(banner);

    bannerLayout->setContentsMargins(22, 18, 22, 18);

    QLabel *bannerTitle = new QLabel(
        "Relief Resource Management"
        );

    bannerTitle->setStyleSheet(
        "color: white;"
        "font-size: 16pt;"
        "font-weight: bold;"
        );

    QLabel *bannerText = new QLabel(
        "Add available supplies, track quantities and help "
        "coordinate disaster relief efforts."
        );

    bannerText->setWordWrap(true);

    bannerText->setStyleSheet(
        "color: #D5E5F8;"
        "font-size: 10pt;"
        );

    bannerLayout->addWidget(bannerTitle);
    bannerLayout->addWidget(bannerText);

    mainLayout->addWidget(banner);

    // ==================================================
    // ADD RESOURCE SECTION
    // ==================================================

    QFrame *formCard = new QFrame;

    formCard->setStyleSheet(
        "QFrame {"
        " background-color: white;"
        " border: 1px solid #DCE5EF;"
        " border-radius: 12px;"
        "}"
        );

    QVBoxLayout *formCardLayout = new QVBoxLayout(formCard);

    formCardLayout->setContentsMargins(22, 22, 22, 22);
    formCardLayout->setSpacing(15);

    QLabel *formTitle = new QLabel("Add New Relief Resource");

    formTitle->setStyleSheet(
        "font-size: 16pt;"
        "font-weight: bold;"
        "color: #102747;"
        "border: none;"
        );

    QLabel *formSubtitle = new QLabel(
        "Enter the details of the resources available with your NGO."
        );

    formSubtitle->setWordWrap(true);

    formSubtitle->setStyleSheet(
        "color: #64748B;"
        "border: none;"
        "font-size: 10pt;"
        );

    formCardLayout->addWidget(formTitle);
    formCardLayout->addWidget(formSubtitle);

    ngoNameInput = new QLineEdit;
    ngoNameInput->setPlaceholderText("Enter NGO name");
    ngoNameInput->setMinimumHeight(45);

    locationInput = new QLineEdit;
    locationInput->setPlaceholderText("Enter resource location");
    locationInput->setMinimumHeight(45);

    resourceInput = new QComboBox;

    resourceInput->addItems({
        "Food",
        "Drinking Water",
        "Medicines",
        "Blankets",
        "Clothing",
        "Shelter",
        "First Aid Kits",
        "Other"
    });

    resourceInput->setMinimumHeight(45);

    quantityInput = new QLineEdit;
    quantityInput->setPlaceholderText("Enter quantity (e.g. 50)");
    quantityInput->setMinimumHeight(45);

    QFormLayout *form = new QFormLayout;

    form->setSpacing(17);
    form->setContentsMargins(0, 8, 0, 8);

    form->setFieldGrowthPolicy(
        QFormLayout::AllNonFixedFieldsGrow
        );

    form->addRow("NGO Name:", ngoNameInput);
    form->addRow("Location:", locationInput);
    form->addRow("Resource Type:", resourceInput);
    form->addRow("Quantity:", quantityInput);

    formCardLayout->addLayout(form);

    // ==================================================
    // ACTION BUTTONS
    // ==================================================

    addButton = new QPushButton("Add Resource");
    refreshButton = new QPushButton("Refresh Resources");

    QHBoxLayout *buttonLayout = new QHBoxLayout;

    buttonLayout->setSpacing(12);
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(refreshButton);

    formCardLayout->addLayout(buttonLayout);

    mainLayout->addWidget(formCard);

    // ==================================================
    // RESOURCE LIST SECTION
    // ==================================================

    QFrame *listCard = new QFrame;

    listCard->setStyleSheet(
        "QFrame {"
        " background-color: white;"
        " border: 1px solid #DCE5EF;"
        " border-radius: 12px;"
        "}"
        );

    QVBoxLayout *listCardLayout = new QVBoxLayout(listCard);

    listCardLayout->setContentsMargins(22, 22, 22, 22);
    listCardLayout->setSpacing(13);

    QLabel *listTitle = new QLabel("Available Relief Resources");

    listTitle->setStyleSheet(
        "font-size: 16pt;"
        "font-weight: bold;"
        "color: #102747;"
        "border: none;"
        );

    QLabel *listSubtitle = new QLabel(
        "Green indicates sufficient stock. Red indicates low stock."
        );

    listSubtitle->setWordWrap(true);

    listSubtitle->setStyleSheet(
        "color: #64748B;"
        "font-size: 10pt;"
        "border: none;"
        );

    resourceList = new QListWidget;

    resourceList->setMinimumHeight(300);

    resourceList->setSelectionMode(
        QAbstractItemView::SingleSelection
        );

    resourceList->setSelectionBehavior(
        QAbstractItemView::SelectItems
        );

    resourceList->setSpacing(3);

    resourceList->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Expanding
        );

    listCardLayout->addWidget(listTitle);
    listCardLayout->addWidget(listSubtitle);
    listCardLayout->addWidget(resourceList);

    // Remove button.
    QPushButton *removeButton =
        new QPushButton("Remove Selected Resource");

    removeButton->setStyleSheet(
        "QPushButton {"
        " background-color: #D93636;"
        " color: white;"
        " border: none;"
        " border-radius: 7px;"
        " padding: 12px;"
        " font-weight: bold;"
        "}"
        "QPushButton:hover {"
        " background-color: #B82020;"
        "}"
        );

    listCardLayout->addWidget(removeButton);

    mainLayout->addWidget(listCard);

    // ==================================================
    // STATUS LABEL
    // ==================================================

    statusLabel = new QLabel(
        "Ready to manage relief resources."
        );

    statusLabel->setWordWrap(true);

    statusLabel->setStyleSheet(
        "background-color: #E2EEFC;"
        "color: #174A82;"
        "border-radius: 7px;"
        "padding: 12px;"
        "font-weight: bold;"
        );

    mainLayout->addWidget(statusLabel);
    mainLayout->addStretch();

    // Place content inside the scroll area.
    scrollArea->setWidget(contentWidget);

    QVBoxLayout *windowLayout = new QVBoxLayout(this);

    windowLayout->setContentsMargins(0, 0, 0, 0);
    windowLayout->addWidget(scrollArea);

    // ==================================================
    // SIGNALS AND SLOTS
    // ==================================================

    // OOP CONCEPT: EVENT-DRIVEN PROGRAMMING
    // Button signals call the corresponding member functions.

    connect(
        addButton,
        &QPushButton::clicked,
        this,
        &NGODashboard::addResource
        );

    connect(
        refreshButton,
        &QPushButton::clicked,
        this,
        &NGODashboard::loadResources
        );

    // Lambda function for removing a selected resource.
    connect(removeButton, &QPushButton::clicked, this, [this]() {

        int row = resourceList->currentRow();

        if (row < 0) {
            QMessageBox::warning(
                this,
                "No Selection",
                "Please select a resource to remove."
                );
            return;
        }

        QString selectedRecord =
            resourceList->item(row)->text();

        QMessageBox::StandardButton answer =
            QMessageBox::question(
                this,
                "Confirm Removal",
                "Are you sure you want to remove this resource?",
                QMessageBox::Yes | QMessageBox::No
                );

        if (answer != QMessageBox::Yes)
            return;

        QString folder =
            QStandardPaths::writableLocation(
                QStandardPaths::AppDataLocation
                );

        QString filePath = folder + "/ngo_resources.txt";

        if (!QFile::exists(filePath)) {
            QMessageBox::warning(
                this,
                "File Not Found",
                "The saved resource file does not exist."
                );
            return;
        }

        QStringList records = readResourceRecords(filePath);

        int matchingIndex = -1;
        int matches = 0;

        for (int i = 0; i < records.size(); i++) {
            if (records[i] == selectedRecord) {
                matchingIndex = i;
                matches++;
            }
        }

        if (matches != 1) {
            QMessageBox::warning(
                this,
                "Removal Failed",
                "The selected resource could not be identified uniquely."
                );
            return;
        }

        records.removeAt(matchingIndex);

        // Write updated records into a temporary file first.
        QString tempPath = folder + "/ngo_resources.tmp";

        QFile tempFile(tempPath);

        if (!tempFile.open(
                QIODevice::WriteOnly |
                QIODevice::Text |
                QIODevice::Truncate)) {

            QMessageBox::critical(
                this,
                "File Error",
                "Could not prepare the updated resource file."
                );
            return;
        }

        QTextStream out(&tempFile);

        for (const QString &savedRecord : std::as_const(records)){
            out << savedRecord << "\n";
            out << "----------------------------------------\n";
        }

        out.flush();

        if (out.status() != QTextStream::Ok) {
            tempFile.close();
            QFile::remove(tempPath);

            QMessageBox::critical(
                this,
                "Save Error",
                "Could not write the updated resources."
                );
            return;
        }

        tempFile.close();

        if (!QFile::remove(filePath)) {
            QFile::remove(tempPath);

            QMessageBox::critical(
                this,
                "File Error",
                "Could not replace the original resource file."
                );
            return;
        }

        if (!QFile::rename(tempPath, filePath)) {
            QMessageBox::critical(
                this,
                "File Error",
                "Updated records are saved in ngo_resources.tmp, "
                "but the original file could not be replaced."
                );
            return;
        }

        loadResources();

        statusLabel->setText(
            "Resource removed successfully."
            );

        QMessageBox::information(
            this,
            "Success",
            "Resource removed successfully."
            );
    });

    // Load saved resources when the dashboard opens.
    loadResources();
}

// ==================================================
// ADD RESOURCE
// ==================================================

// OOP CONCEPT: ENCAPSULATION
// This function handles resource validation and saving.

void NGODashboard::addResource()
{
    QString ngoName = ngoNameInput->text().trimmed();
    QString location = locationInput->text().trimmed();
    QString quantityText = quantityInput->text().trimmed();

    QString resource = resourceInput->currentText();

    bool validQuantity = false;
    int quantity = quantityText.toInt(&validQuantity);

    if (ngoName.isEmpty() ||
        location.isEmpty() ||
        quantityText.isEmpty()) {

        QMessageBox::warning(
            this,
            "Missing Information",
            "Please enter the NGO name, location and quantity."
            );
        return;
    }

    if (!validQuantity || quantity <= 0) {
        QMessageBox::warning(
            this,
            "Invalid Quantity",
            "Please enter a positive whole number for quantity."
            );
        return;
    }

    // ABSTRACTION: QStandardPaths provides the application data location.
    QString folder =
        QStandardPaths::writableLocation(
            QStandardPaths::AppDataLocation
            );

    if (folder.isEmpty() || !QDir().mkpath(folder)) {
        QMessageBox::critical(
            this,
            "Storage Error",
            "Could not create the local data folder."
            );
        return;
    }

    QFile file(folder + "/ngo_resources.txt");

    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        QMessageBox::critical(
            this,
            "File Error",
            "Could not save the resource."
            );
        return;
    }

    QTextStream out(&file);

    out << "NGO: " << ngoName << "\n";
    out << "Resource: " << resource << "\n";
    out << "Quantity: " << quantity << "\n";
    out << "Location: " << location << "\n";

    out << "Added: "
        << QDateTime::currentDateTime().toString(
               "dd-MM-yyyy hh:mm AP"
               )
        << "\n";

    out << "----------------------------------------\n";

    out.flush();

    if (out.status() != QTextStream::Ok) {
        file.close();

        QMessageBox::critical(
            this,
            "Save Error",
            "The resource could not be saved properly."
            );
        return;
    }

    file.close();

    quantityInput->clear();

    statusLabel->setText(
        "Resource added successfully!"
        );

    loadResources();

    QMessageBox::information(
        this,
        "Success",
        "Relief resource saved successfully."
        );
}

// ==================================================
// LOAD RESOURCES
// ==================================================

// OOP CONCEPT: ENCAPSULATION
// This function reads and displays saved resources.
//
// OOP CONCEPT: ABSTRACTION
// QFile and QTextStream simplify file handling.

void NGODashboard::loadResources()
{
    resourceList->clear();

    QString folder =
        QStandardPaths::writableLocation(
            QStandardPaths::AppDataLocation
            );

    QFile file(folder + "/ngo_resources.txt");

    if (!file.exists()) {
        statusLabel->setText(
            "No resources added yet. Add your first resource above."
            );
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        statusLabel->setText(
            "Could not read the saved resources."
            );
        return;
    }

    QTextStream in(&file);

    QString record;
    int lowStockCount = 0;

    // RGB values avoid the Clazy QColor-from-literal warning.
    const QColor lowStockBackground(254, 226, 226);
    const QColor normalStockBackground(220, 252, 231);

    while (!in.atEnd()) {

        QString line = in.readLine();

        if (line == "----------------------------------------") {

            if (!record.trimmed().isEmpty()) {

                QString savedRecord = record.trimmed();

                int quantity = getResourceQuantity(savedRecord);

                QListWidgetItem *item =
                    new QListWidgetItem(savedRecord);

                if (quantity <= 10) {
                    item->setBackground(lowStockBackground);
                    item->setToolTip("Warning: Low stock available!");
                    lowStockCount++;
                }
                else {
                    item->setBackground(normalStockBackground);
                    item->setToolTip("Resource availability is good.");
                }

                resourceList->addItem(item);
            }

            record.clear();
        }
        else {
            if (!record.isEmpty())
                record += "\n";

            record += line;
        }
    }

    // Handle the final record if no separator exists at the end.
    if (!record.trimmed().isEmpty()) {

        QString savedRecord = record.trimmed();

        int quantity = getResourceQuantity(savedRecord);

        QListWidgetItem *item =
            new QListWidgetItem(savedRecord);

        if (quantity <= 10) {
            item->setBackground(lowStockBackground);
            item->setToolTip("Warning: Low stock available!");
            lowStockCount++;
        }
        else {
            item->setBackground(normalStockBackground);
            item->setToolTip("Resource availability is good.");
        }

        resourceList->addItem(item);
    }

    file.close();

    statusLabel->setText(
        QString("Total Resources: %1  |  Low Stock Items: %2")
            .arg(resourceList->count())
            .arg(lowStockCount)
        );
}
