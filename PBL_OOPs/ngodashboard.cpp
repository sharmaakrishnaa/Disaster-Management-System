#include "ngodashboard.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QListWidget>
#include <QPushButton>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QStandardPaths>
#include <QDateTime>
#include <QAbstractItemView>
#include <QStringList>

NGODashboard::NGODashboard(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("SOS Connect - NGO Dashboard");
    setMinimumSize(750, 650);

    setStyleSheet(
        "QWidget {"
        " background-color: #0B1B33;"
        " color: white;"
        " font-family: 'Segoe UI';"
        " font-size: 11pt;"
        "}"
        "QLineEdit, QComboBox, QListWidget {"
        " background-color: #FFFFFF;"
        " color: #172B4D;"
        " border: 1px solid #B7C9DD;"
        " border-radius: 6px;"
        " padding: 9px;"
        "}"
        "QPushButton {"
        " background-color: #1672D5;"
        " color: white;"
        " border: none;"
        " border-radius: 7px;"
        " padding: 11px;"
        " font-weight: bold;"
        "}"
        "QPushButton:hover {"
        " background-color: #328BEA;"
        "}"
        );

    QLabel *title = new QLabel("NGO Relief Dashboard");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(
        "font-size: 24pt;"
        "font-weight: bold;"
        );

    QLabel *subtitle = new QLabel(
        "Add and manage available disaster relief resources."
        );
    subtitle->setAlignment(Qt::AlignCenter);
    subtitle->setStyleSheet("color: #C6D5E8;");

    ngoNameInput = new QLineEdit;
    ngoNameInput->setPlaceholderText("NGO name");

    locationInput = new QLineEdit;
    locationInput->setPlaceholderText("Resource location");

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

    quantityInput = new QLineEdit;
    quantityInput->setPlaceholderText("Quantity (e.g. 50)");

    addButton = new QPushButton("Add Resource");
    refreshButton = new QPushButton("Refresh Resources");

    QPushButton *removeButton =
        new QPushButton("Remove Selected Resource");

    removeButton->setStyleSheet(
        "QPushButton {"
        " background-color: #D93636;"
        " color: white;"
        " border: none;"
        " border-radius: 7px;"
        " padding: 11px;"
        " font-weight: bold;"
        "}"
        "QPushButton:hover {"
        " background-color: #B82020;"
        "}"
        );

    resourceList = new QListWidget;
    resourceList->setSelectionMode(
        QAbstractItemView::SingleSelection
        );

    statusLabel = new QLabel(
        "Ready to manage relief resources."
        );
    statusLabel->setWordWrap(true);
    statusLabel->setStyleSheet("color: #A9D6FF;");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(35, 25, 35, 25);
    layout->setSpacing(13);

    layout->addWidget(title);
    layout->addWidget(subtitle);
    layout->addSpacing(10);
    layout->addWidget(ngoNameInput);
    layout->addWidget(locationInput);
    layout->addWidget(resourceInput);
    layout->addWidget(quantityInput);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(refreshButton);

    layout->addLayout(buttonLayout);

    QLabel *listTitle = new QLabel(
        "Available Relief Resources"
        );
    listTitle->setStyleSheet(
        "font-size: 15pt;"
        "font-weight: bold;"
        );

    layout->addWidget(listTitle);
    layout->addWidget(resourceList, 1);
    layout->addWidget(removeButton);
    layout->addWidget(statusLabel);

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

        QString filePath =
            folder + "/ngo_resources.txt";

        QFile file(filePath);

        if (!file.open(
                QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::critical(
                this,
                "File Error",
                "Could not open the resources file."
                );
            return;
        }

        QTextStream in(&file);
        QStringList records;
        QString record;

        while (!in.atEnd()) {
            QString line = in.readLine();

            if (line ==
                "----------------------------------------") {

                if (!record.trimmed().isEmpty())
                    records.append(record.trimmed());

                record.clear();
            } else {
                if (!record.isEmpty())
                    record += "\n";

                record += line;
            }
        }

        if (!record.trimmed().isEmpty())
            records.append(record.trimmed());

        file.close();

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
                "The resource could not be identified uniquely."
                );
            return;
        }

        records.removeAt(matchingIndex);

        // Write the remaining records to a temporary file first.
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

        for (const QString &savedRecord : records) {
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

        // Replace the original only after the temporary file is ready.
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
                "The original was removed, but the new file "
                "could not be renamed. Your updated records are "
                "in ngo_resources.tmp."
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

    loadResources();
}

void NGODashboard::addResource()
{
    QString ngoName =
        ngoNameInput->text().trimmed();

    QString location =
        locationInput->text().trimmed();

    QString quantityText =
        quantityInput->text().trimmed();

    QString resource =
        resourceInput->currentText();

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

    if (!file.open(
            QIODevice::Append | QIODevice::Text)) {

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

    if (!file.open(
            QIODevice::ReadOnly | QIODevice::Text)) {

        statusLabel->setText(
            "Could not read the saved resources."
            );
        return;
    }

    QTextStream in(&file);
    QString record;

    while (!in.atEnd()) {
        QString line = in.readLine();

        if (line ==
            "----------------------------------------") {

            if (!record.trimmed().isEmpty())
                resourceList->addItem(record.trimmed());

            record.clear();
        } else {
            if (!record.isEmpty())
                record += "\n";

            record += line;
        }
    }

    if (!record.trimmed().isEmpty())
        resourceList->addItem(record.trimmed());

    file.close();

    statusLabel->setText(
        QString("Resources loaded: %1")
            .arg(resourceList->count())
        );
}