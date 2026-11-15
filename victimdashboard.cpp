
#include "victimdashboard.h"

#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QPushButton>
#include <QMessageBox>
#include <QFrame>
#include <QPixmap>
#include <QWidget>
#include <QSizePolicy>

#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QStandardPaths>
#include <QUuid>
#include <QDateTime>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>


// INHERITANCE:
// VictimDashboard inherits from QWidget.

VictimDashboard::VictimDashboard(QWidget *parent)
    : QWidget(parent)
{
    // Configure the main window.
    setWindowTitle("SOS Connect - Victim Dashboard");
    setMinimumSize(850, 750);
    resize(1100, 900);

    // ----------------------------------------
    // 1. GLOBAL GUI STYLING
    // ----------------------------------------

    setStyleSheet(
        "QWidget {"
        " background-color: #F3F6FB;"
        " color: #172B46;"
        " font-family: 'Segoe UI';"
        " font-size: 11pt;"
        "}"

        "QLineEdit, QComboBox, QTextEdit {"
        " background-color: #FFFFFF;"
        " color: #172B46;"
        " border: 1px solid #AABBD0;"
        " border-radius: 7px;"
        " padding: 10px;"
        " font-size: 12pt;"
        " selection-background-color: #1672D5;"
        " selection-color: white;"
        "}"

        "QLineEdit { min-height: 25px; }"
        "QComboBox { min-height: 28px; }"
        "QTextEdit { min-height: 85px; }"

        "QLineEdit:focus, QComboBox:focus, QTextEdit:focus {"
        " border: 2px solid #1672D5;"
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


    // ----------------------------------------
    // 2. DASHBOARD HEADING
    // ----------------------------------------

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


    // ----------------------------------------
    // 3. EMERGENCY BANNER
    // ----------------------------------------

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


    // ----------------------------------------
    // 4. HOPE & SUPPORT SECTION
    // ----------------------------------------

    QLabel *hopeHeading = new QLabel("Hope & Support");

    hopeHeading->setStyleSheet(
        "font-size: 21pt;"
        "font-weight: bold;"
        "color: #102747;"
        );

    QLabel *hopeSubtitle = new QLabel(
        "Together, we can rebuild lives and create a better tomorrow."
        );

    hopeSubtitle->setStyleSheet("color: #64748B;");
    hopeSubtitle->setWordWrap(true);

    QHBoxLayout *hopeCardsLayout = new QHBoxLayout;
    hopeCardsLayout->setSpacing(15);

    // ABSTRACTION:
    // QNetworkAccessManager handles online image downloads.

    QNetworkAccessManager *imageManager =
        new QNetworkAccessManager(this);

    // LAMBDA FUNCTION:
    // Reusable function to create Hope & Support cards.

    auto createHopeCard =
        [this, imageManager, hopeCardsLayout]
        (QString imageUrl, QString title, QString message)
    {
        QFrame *card = new QFrame;

        card->setObjectName("hopeCard");

        card->setStyleSheet(
            "QFrame#hopeCard {"
            " background-color: white;"
            " border: 1px solid #DCE5F0;"
            " border-radius: 10px;"
            "}"
            );

        QVBoxLayout *cardLayout = new QVBoxLayout(card);

        cardLayout->setSpacing(8);
        cardLayout->setContentsMargins(10, 10, 10, 10);

        QLabel *imageLabel = new QLabel("Loading image...");

        imageLabel->setFixedSize(210, 115);
        imageLabel->setAlignment(Qt::AlignCenter);

        imageLabel->setStyleSheet(
            "background-color: #E8EEF7;"
            "color: #64748B;"
            "border-radius: 7px;"
            );

        QLabel *titleLabel = new QLabel(title);

        titleLabel->setStyleSheet(
            "font-weight: bold;"
            "font-size: 12pt;"
            "color: #102747;"
            );

        QLabel *messageLabel = new QLabel(message);

        messageLabel->setWordWrap(true);

        messageLabel->setStyleSheet(
            "font-size: 10pt;"
            "color: #64748B;"
            );

        cardLayout->addWidget(imageLabel, 0, Qt::AlignCenter);
        cardLayout->addWidget(titleLabel);
        cardLayout->addWidget(messageLabel);

        hopeCardsLayout->addWidget(card);

        // Download image asynchronously.
        QNetworkReply *reply =
            imageManager->get(
                QNetworkRequest(QUrl(imageUrl))
                );

        // SIGNAL AND SLOT:
        // Display the image after downloading finishes.

        connect(reply, &QNetworkReply::finished,
                this, [reply, imageLabel]()
                {
                    QPixmap picture;

                    if (picture.loadFromData(reply->readAll()))
                    {
                        imageLabel->setPixmap(
                            picture.scaled(
                                imageLabel->size(),
                                Qt::KeepAspectRatioByExpanding,
                                Qt::SmoothTransformation
                                )
                            );

                        imageLabel->setText("");
                    }
                    else
                    {
                        imageLabel->setText("Image unavailable");
                    }

                    reply->deleteLater();
                });
    };

    // Create three Hope & Support cards.

    createHopeCard(
        "https://images.unsplash.com/photo-1469571486292-0ba58a3f068b?w=500",
        "Community Support",
        "People working together can bring hope during difficult times."
        );

    createHopeCard(
        "https://images.unsplash.com/photo-1532629345422-7515f3d16bb6?w=500",
        "Helping Hands",
        "Every contribution can make a meaningful difference."
        );

    createHopeCard(
        "https://images.unsplash.com/photo-1488521787991-ed7bbaae773c?w=500",
        "A Better Tomorrow",
        "With care and support, every community can rebuild."
        );


    // ----------------------------------------
    // 5. EMERGENCY CONTACTS SECTION
    // ----------------------------------------

    QLabel *emergencyHeading =
        new QLabel("Emergency Contacts");

    emergencyHeading->setStyleSheet(
        "font-size: 21pt;"
        "font-weight: bold;"
        "color: #102747;"
        );

    QLabel *emergencySubtitle = new QLabel(
        "Important helpline numbers to contact during an emergency."
        );

    emergencySubtitle->setWordWrap(true);
    emergencySubtitle->setStyleSheet("color: #64748B;");

    QHBoxLayout *contactLayout = new QHBoxLayout;
    contactLayout->setSpacing(15);

    // LAMBDA FUNCTION:
    // Reusable function for creating emergency contact cards.

    auto createContactCard =
        [](QString service, QString number, QString details)
    {
        QFrame *card = new QFrame;

        card->setStyleSheet(
            "QFrame {"
            " background-color: white;"
            " border: 1px solid #DCE5F0;"
            " border-radius: 10px;"
            "}"
            );

        QVBoxLayout *layout = new QVBoxLayout(card);

        layout->setSpacing(8);

        QLabel *serviceLabel = new QLabel(service);

        serviceLabel->setStyleSheet(
            "font-weight: bold;"
            "font-size: 12pt;"
            "color: #102747;"
            "border: none;"
            );

        QLabel *numberLabel = new QLabel(number);

        numberLabel->setStyleSheet(
            "font-size: 22pt;"
            "font-weight: bold;"
            "color: #1672D5;"
            "border: none;"
            );

        QLabel *detailsLabel = new QLabel(details);

        detailsLabel->setWordWrap(true);

        detailsLabel->setStyleSheet(
            "font-size: 10pt;"
            "color: #64748B;"
            "border: none;"
            );

        layout->addWidget(serviceLabel);
        layout->addWidget(numberLabel);
        layout->addWidget(detailsLabel);

        return card;
    };

    // Create emergency contact cards.

    contactLayout->addWidget(
        createContactCard(
            "National Emergency",
            "112",
            "Police, fire and medical emergencies."
            )
        );

    contactLayout->addWidget(
        createContactCard(
            "Disaster Management",
            "1070",
            "Maharashtra State Emergency Operations Centre."
            )
        );

    contactLayout->addWidget(
        createContactCard(
            "Fire Service",
            "101",
            "Fire and rescue assistance."
            )
        );

    contactLayout->addWidget(
        createContactCard(
            "Ambulance",
            "108",
            "Emergency medical transport. Availability may vary by state."
            )
        );


    // ----------------------------------------
    // 6. DISASTER SAFETY GUIDELINES
    // ----------------------------------------

    QLabel *safetyHeading =
        new QLabel("Disaster Safety Guidelines");

    safetyHeading->setStyleSheet(
        "font-size: 21pt;"
        "font-weight: bold;"
        "color: #102747;"
        );

    QLabel *safetySubtitle = new QLabel(
        "Follow these basic precautions to protect yourself and others."
        );

    safetySubtitle->setWordWrap(true);
    safetySubtitle->setStyleSheet("color: #64748B;");

    QHBoxLayout *safetyLayout = new QHBoxLayout;
    safetyLayout->setSpacing(15);

    // LAMBDA FUNCTION:
    // Reusable function to create safety guideline cards.

    auto createSafetyCard =
        [](QString title, QString instructions)
    {
        QFrame *card = new QFrame;

        card->setStyleSheet(
            "QFrame {"
            " background-color: white;"
            " border: 1px solid #DCE5F0;"
            " border-radius: 10px;"
            "}"
            );

        QVBoxLayout *layout = new QVBoxLayout(card);

        layout->setSpacing(8);

        QLabel *titleLabel = new QLabel(title);

        titleLabel->setStyleSheet(
            "font-weight: bold;"
            "font-size: 13pt;"
            "color: #102747;"
            "border: none;"
            );

        QLabel *instructionsLabel = new QLabel(instructions);

        instructionsLabel->setWordWrap(true);

        instructionsLabel->setStyleSheet(
            "font-size: 10pt;"
            "color: #475569;"
            "border: none;"
            );

        layout->addWidget(titleLabel);
        layout->addWidget(instructionsLabel);

        return card;
    };

    // Flood safety.

    safetyLayout->addWidget(
        createSafetyCard(
            "Flood Safety",
            "• Move to higher ground.\n"
            "• Stay away from flooded roads.\n"
            "• Switch off electricity if safe.\n"
            "• Follow official evacuation instructions."
            )
        );

    // Earthquake safety.

    safetyLayout->addWidget(
        createSafetyCard(
            "Earthquake Safety",
            "• Drop, Cover and Hold On.\n"
            "• Stay away from windows.\n"
            "• Protect your head and neck.\n"
            "• Move outside only when shaking stops."
            )
        );

    // Fire safety.

    safetyLayout->addWidget(
        createSafetyCard(
            "Fire Safety",
            "• Raise the alarm and evacuate.\n"
            "• Stay low under smoke.\n"
            "• Never use lifts during a fire.\n"
            "• Call the fire service at 101."
            )
        );


    // ----------------------------------------
    // 7. SOS REQUEST FORM
    // ----------------------------------------

    // ENCAPSULATION:
    // These input widgets are private members of this class.

    nameInput = new QLineEdit;

    nameInput->setPlaceholderText("Enter your full name");
    nameInput->setMinimumHeight(48);

    locationInput = new QLineEdit;

    locationInput->setPlaceholderText(
        "Enter your location or nearby landmark"
        );

    locationInput->setMinimumHeight(48);

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

    disasterInput->setMinimumHeight(48);

    urgencyInput = new QComboBox;

    urgencyInput->addItems({
        "Select urgency",
        "Low",
        "Medium",
        "High",
        "Critical"
    });

    urgencyInput->setMinimumHeight(48);

    helpInput = new QTextEdit;

    helpInput->setPlaceholderText(
        "Example: food, drinking water, medicine, rescue..."
        );

    helpInput->setMinimumHeight(110);

    helpInput->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Minimum
        );

    descriptionInput = new QTextEdit;

    descriptionInput->setPlaceholderText(
        "Describe the emergency and any important details."
        );

    descriptionInput->setMinimumHeight(120);

    descriptionInput->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Minimum
        );

    // FORM LAYOUT:
    // Organizes labels and input fields.

    QFormLayout *form = new QFormLayout;

    form->setSpacing(18);

    form->setContentsMargins(0, 10, 0, 10);

    form->setFieldGrowthPolicy(
        QFormLayout::AllNonFixedFieldsGrow
        );

    form->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    form->addRow("Full Name:", nameInput);
    form->addRow("Location:", locationInput);
    form->addRow("Disaster Type:", disasterInput);
    form->addRow("Urgency:", urgencyInput);
    form->addRow("Help Required:", helpInput);
    form->addRow("Description:", descriptionInput);


    // ----------------------------------------
    // 8. SUBMIT BUTTON AND STATUS
    // ----------------------------------------

    submitButton = new QPushButton("Submit SOS Request");

    submitButton->setMinimumHeight(52);

    statusLabel = new QLabel(
        "Your request has not been submitted."
        );

    statusLabel->setWordWrap(true);

    statusLabel->setStyleSheet(
        "color: #64748B;"
        "font-weight: bold;"
        );


    // ----------------------------------------
    // 9. SCROLLABLE DASHBOARD LAYOUT
    // ----------------------------------------

    // QScrollArea allows the user to scroll
    // through the complete dashboard.

    QScrollArea *scrollArea = new QScrollArea(this);

    scrollArea->setWidgetResizable(true);

    scrollArea->setFrameShape(QFrame::NoFrame);

    scrollArea->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAsNeeded
        );

    scrollArea->setVerticalScrollBarPolicy(
        Qt::ScrollBarAsNeeded
        );

    QWidget *contentWidget = new QWidget;

    contentWidget->setMinimumWidth(750);

    QVBoxLayout *mainLayout = new QVBoxLayout(contentWidget);

    mainLayout->setContentsMargins(35, 25, 35, 25);

    mainLayout->setSpacing(18);

    // Dashboard heading.
    mainLayout->addWidget(heading);
    mainLayout->addWidget(subtitle);

    // Emergency banner.
    mainLayout->addWidget(banner);

    // Hope & Support section.
    mainLayout->addWidget(hopeHeading);
    mainLayout->addWidget(hopeSubtitle);
    mainLayout->addLayout(hopeCardsLayout);

    // Emergency Contacts section.
    mainLayout->addWidget(emergencyHeading);
    mainLayout->addWidget(emergencySubtitle);
    mainLayout->addLayout(contactLayout);

    // Safety Guidelines section.
    mainLayout->addWidget(safetyHeading);
    mainLayout->addWidget(safetySubtitle);
    mainLayout->addLayout(safetyLayout);

    // SOS request form.
    mainLayout->addLayout(form);
    mainLayout->addWidget(submitButton);
    mainLayout->addWidget(statusLabel);

    mainLayout->addStretch();

    // Place content inside scroll area.
    scrollArea->setWidget(contentWidget);

    QVBoxLayout *windowLayout = new QVBoxLayout(this);

    windowLayout->setContentsMargins(0, 0, 0, 0);

    windowLayout->addWidget(scrollArea);


    // ----------------------------------------
    // 10. SIGNAL AND SLOT CONNECTION
    // ----------------------------------------

    // When the user clicks the button,
    // submitSOS() executes.

    connect(submitButton, &QPushButton::clicked,
            this, &VictimDashboard::submitSOS);
}


// ============================================
// SOS SUBMISSION FUNCTION
// ============================================

// ABSTRACTION:
// This function handles validation, ID generation and file storage.

void VictimDashboard::submitSOS()
{
    // Read information entered by the victim.

    QString name = nameInput->text().trimmed();

    QString location = locationInput->text().trimmed();

    QString help = helpInput->toPlainText().trimmed();

    QString description =
        descriptionInput->toPlainText().trimmed();


    // ----------------------------------------
    // 1. VALIDATION
    // ----------------------------------------

    if (name.isEmpty() || location.isEmpty() || help.isEmpty())
    {
        QMessageBox::warning(
            this,
            "Missing Information",
            "Please enter your name, location, and required help."
            );

        return;
    }

    if (disasterInput->currentIndex() == 0 ||
        urgencyInput->currentIndex() == 0)
    {
        QMessageBox::warning(
            this,
            "Missing Information",
            "Please select the disaster type and urgency."
            );

        return;
    }


    // ----------------------------------------
    // 2. GENERATE UNIQUE REQUEST ID
    // ----------------------------------------

    // QUuid generates a unique identifier.

    QString requestId =
        QUuid::createUuid()
            .toString(QUuid::WithoutBraces)
            .left(8)
            .toUpper();


    // ----------------------------------------
    // 3. CREATE STORAGE DIRECTORY
    // ----------------------------------------

    QString folder =
        QStandardPaths::writableLocation(
            QStandardPaths::AppDataLocation
            );

    if (folder.isEmpty() || !QDir().mkpath(folder))
    {
        QMessageBox::critical(
            this,
            "Storage Error",
            "Could not create the application data folder."
            );

        return;
    }


    // ----------------------------------------
    // 4. SAVE SOS REQUEST
    // ----------------------------------------

    QString filePath = folder + "/sos_requests.txt";

    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly |
                   QIODevice::Append |
                   QIODevice::Text))
    {
        QMessageBox::critical(
            this,
            "Storage Error",
            "Could not save your SOS request."
            );

        return;
    }

    // QTextStream writes information into the file.

    QTextStream out(&file);

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


    // ----------------------------------------
    // 5. UPDATE DASHBOARD STATUS
    // ----------------------------------------

    statusLabel->setStyleSheet(
        "color: #167244;"
        "font-weight: bold;"
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
