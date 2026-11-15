
#include "sosrequestservice.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QString>
#include <QStringList>
#include <QTextStream>

namespace SOSConnect
{

namespace
{
// These are helper functions available only inside this source file.
// They convert data between Qt strings and standard C++ strings.
// ABSTRACTION: The service uses these helpers instead of repeating conversions.

QString toQString(const std::string& value)
{
    return QString::fromStdString(value);
}

std::string toStdString(const QString& value)
{
    return value.toStdString();
}

// These helper functions convert text into the appropriate enum values.
// ABSTRACTION: Other functions can work with DisasterType, Urgency,
// and RequestStatus instead of repeatedly processing text.

DisasterType disasterFromText(const QString& text)
{
    if (text.compare("Flood", Qt::CaseInsensitive) == 0)
        return DisasterType::Flood;
    if (text.compare("Earthquake", Qt::CaseInsensitive) == 0)
        return DisasterType::Earthquake;
    if (text.compare("Fire", Qt::CaseInsensitive) == 0)
        return DisasterType::Fire;
    if (text.compare("Landslide", Qt::CaseInsensitive) == 0)
        return DisasterType::Landslide;
    if (text.compare("Cyclone", Qt::CaseInsensitive) == 0)
        return DisasterType::Cyclone;
    if (text.compare("Medical Emergency", Qt::CaseInsensitive) == 0)
        return DisasterType::MedicalEmergency;

    return DisasterType::Other;
}

QString disasterToText(DisasterType type)
{
    switch (type)
    {
    case DisasterType::Flood: return "Flood";
    case DisasterType::Earthquake: return "Earthquake";
    case DisasterType::Fire: return "Fire";
    case DisasterType::Landslide: return "Landslide";
    case DisasterType::Cyclone: return "Cyclone";
    case DisasterType::MedicalEmergency: return "Medical Emergency";
    case DisasterType::Other: return "Other";
    }

    return "Other";
}

Urgency urgencyFromText(const QString& text)
{
    if (text.compare("Low", Qt::CaseInsensitive) == 0)
        return Urgency::Low;
    if (text.compare("Medium", Qt::CaseInsensitive) == 0)
        return Urgency::Medium;
    if (text.compare("High", Qt::CaseInsensitive) == 0)
        return Urgency::High;

    return Urgency::Critical;
}

QString urgencyToText(Urgency urgency)
{
    switch (urgency)
    {
    case Urgency::Low: return "Low";
    case Urgency::Medium: return "Medium";
    case Urgency::High: return "High";
    case Urgency::Critical: return "Critical";
    }

    return "Medium";
}

RequestStatus statusFromText(const QString& text)
{
    if (text.compare("Accepted", Qt::CaseInsensitive) == 0 ||
        text.compare("Assigned", Qt::CaseInsensitive) == 0)
        return RequestStatus::Assigned;

    if (text.compare("In Progress", Qt::CaseInsensitive) == 0)
        return RequestStatus::InProgress;

    if (text.compare("Resolved", Qt::CaseInsensitive) == 0)
        return RequestStatus::Resolved;

    return RequestStatus::Raised;
}

QString statusToText(RequestStatus status)
{
    switch (status)
    {
    case RequestStatus::Raised: return "Pending";
    case RequestStatus::Assigned: return "Accepted";
    case RequestStatus::InProgress: return "In Progress";
    case RequestStatus::Resolved: return "Resolved";
    }

    return "Pending";
}

// COLLECTION HANDLING:
// Converts a vector of assistance types into readable text.

QString assistanceToText(
    const std::vector<AssistanceType>& assistance)
{
    QStringList values;

    for (AssistanceType item : assistance)
    {
        switch (item)
        {
        case AssistanceType::Food:
            values << "Food";
            break;
        case AssistanceType::Water:
            values << "Water";
            break;
        case AssistanceType::Medicine:
            values << "Medicine";
            break;
        case AssistanceType::Blankets:
            values << "Blankets";
            break;
        case AssistanceType::Shelter:
            values << "Shelter";
            break;
        case AssistanceType::Rescue:
            values << "Rescue";
            break;
        }
    }

    return values.join(", ");
}

// Converts the saved assistance text back into a vector.
// This is data conversion, not a separate OOP pillar.

std::vector<AssistanceType> assistanceFromText(
    const QString& text)
{
    std::vector<AssistanceType> result;
    const QString lower = text.toLower();

    if (lower.contains("food"))
        result.push_back(AssistanceType::Food);

    if (lower.contains("water"))
        result.push_back(AssistanceType::Water);

    if (lower.contains("medicine") || lower.contains("medical"))
        result.push_back(AssistanceType::Medicine);

    if (lower.contains("blanket"))
        result.push_back(AssistanceType::Blankets);

    if (lower.contains("shelter"))
        result.push_back(AssistanceType::Shelter);

    if (lower.contains("rescue"))
        result.push_back(AssistanceType::Rescue);

    return result;
}

} // namespace


// OOP CONCEPT: CONSTRUCTOR
// Initializes a new SOSRequestService object.
// The member initializer list sets the initial request number.
//
// ABSTRACTION:
// The service prepares its own storage location and loads saved requests,
// so other modules do not need to implement these steps themselves.

SOSRequestService::SOSRequestService()
    : m_nextRequestNumber(1)
{
    QString dataDirectory =
        QStandardPaths::writableLocation(
            QStandardPaths::AppDataLocation);

    if (!dataDirectory.isEmpty())
        QDir().mkpath(dataDirectory);

    // Store the file path as part of this service's internal state.
    m_storageFilePath =
        toStdString(dataDirectory + "/sos_service_requests.txt");

    loadRequests();
}


// ENCAPSULATION + ABSTRACTION:
// This member function handles loading request records.
// The caller only needs to call loadRequests() rather than
// implement the file-reading and parsing process itself.

bool SOSRequestService::loadRequests()
{
    m_sosRequests.clear();
    m_nextRequestNumber = 1;

    QFile file(toQString(m_storageFilePath));

    if (!file.exists())
        return true;

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);
    QString record;
    QStringList records;

    while (!in.atEnd())
    {
        QString line = in.readLine();

        if (line.trimmed() ==
            "----------------------------------------")
        {
            if (!record.trimmed().isEmpty())
                records << record;

            record.clear();
        }
        else
        {
            record += line + "\n";
        }
    }

    if (!record.trimmed().isEmpty())
        records << record;

    file.close();

    for (const QString& currentRecord : records)
    {
        QString requestId;
        QString victimName;
        QString location;
        QString disaster;
        QString urgency;
        QString help;
        QString description;
        QString createdAt;
        QString status = "Pending";

        const QStringList lines = currentRecord.split('\n');

        for (const QString& rawLine : lines)
        {
            QString line = rawLine.trimmed();

            if (line.startsWith("Request ID:"))
                requestId = line.mid(11).trimmed();
            else if (line.startsWith("Victim:"))
                victimName = line.mid(7).trimmed();
            else if (line.startsWith("Location:"))
                location = line.mid(9).trimmed();
            else if (line.startsWith("Disaster:"))
                disaster = line.mid(9).trimmed();
            else if (line.startsWith("Urgency:"))
                urgency = line.mid(8).trimmed();
            else if (line.startsWith("Help Required:"))
                help = line.mid(14).trimmed();
            else if (line.startsWith("Description:"))
                description = line.mid(12).trimmed();
            else if (line.startsWith("Created At:"))
                createdAt = line.mid(11).trimmed();
            else if (line.startsWith("Status:"))
                status = line.mid(7).trimmed();
        }

        if (requestId.isEmpty())
            continue;

        // OBJECT CREATION:
        // A new SOSRequest object is created using the saved record.
        // This demonstrates interaction between SOSRequestService
        // and the SOSRequest class.

        SOSRequest request(
            toStdString(requestId),
            toStdString(victimName),
            disasterFromText(disaster),
            toStdString(location),
            urgencyFromText(urgency),
            assistanceFromText(help),
            toStdString(description)
            );

        // ENCAPSULATION:
        // The request is updated through setter functions rather
        // than directly changing its internal data members.
        request.setVictimName(toStdString(victimName));
        request.setHelpRequired(toStdString(help));
        request.setCreatedAt(toStdString(createdAt));
        request.setRequestStatus(statusFromText(status));

        // COLLECTION:
        // Store the request object in the service's vector.
        m_sosRequests.push_back(request);

        // Generate the next ID number without reusing existing IDs.
        if (requestId.startsWith("SOS-"))
        {
            bool ok = false;
            unsigned int number = requestId.mid(4).toUInt(&ok);

            if (ok && number >= m_nextRequestNumber)
                m_nextRequestNumber = number + 1;
        }
    }

    return true;
}


// ENCAPSULATION + ABSTRACTION:
// Saves all request objects managed by this service.
// The const at the end indicates that this member function
// does not modify the service's member state directly.

bool SOSRequestService::saveRequests() const
{
    QFile file(toQString(m_storageFilePath));

    if (!file.open(
            QIODevice::WriteOnly |
            QIODevice::Truncate |
            QIODevice::Text))
    {
        return false;
    }

    QTextStream out(&file);

    // OBJECT INTERACTION:
    // Each SOSRequest object provides its information through getters.
    for (const SOSRequest& request : m_sosRequests)
    {
        out << "Request ID: "
            << toQString(request.getRequestId()) << "\n";

        out << "Victim: "
            << toQString(request.getVictimName()) << "\n";

        out << "Location: "
            << toQString(request.getRequestLocation()) << "\n";

        out << "Disaster: "
            << disasterToText(request.getDisasterType()) << "\n";

        out << "Urgency: "
            << urgencyToText(request.getRequestUrgency()) << "\n";

        out << "Help Required: "
            << toQString(request.getHelpRequired()) << "\n";

        out << "Description: "
            << toQString(request.getRequestDescription()) << "\n";

        out << "Created At: "
            << toQString(request.getCreatedAt()) << "\n";

        out << "Status: "
            << statusToText(request.getRequestStatus()) << "\n";

        out << "----------------------------------------\n";
    }

    file.close();

    return file.error() == QFileDevice::NoError;
}


// ENCAPSULATION + ABSTRACTION:
// createRequest() handles the complete request-creation process.
// Other modules call this function instead of managing IDs,
// timestamps, the vector, and file saving separately.

std::string SOSRequestService::createRequest(
    const std::string& victimId,
    DisasterType disasterType,
    const std::string& requestLocation,
    Urgency requestUrgency,
    const std::vector<AssistanceType>& requiredAssistance,
    const std::string& requestDescription)
{
    QString newRequestId;

    // Generate a unique request ID.
    do
    {
        newRequestId = QString("SOS-%1")
        .arg(m_nextRequestNumber++, 4, 10, QChar('0'));
    }
    while (findRequestById(toStdString(newRequestId)) != nullptr);

    // OBJECT CREATION:
    // Create an SOSRequest object from the supplied details.
    SOSRequest request(
        toStdString(newRequestId),
        victimId,
        disasterType,
        requestLocation,
        requestUrgency,
        requiredAssistance,
        requestDescription
        );

    // ENCAPSULATION:
    // Update request information using its public setter methods.
    request.setVictimName(victimId);
    request.setHelpRequired(
        toStdString(assistanceToText(requiredAssistance)));

    // ABSTRACTION:
    // QDateTime provides a ready-made way to obtain the current time.
    QString now =
        QDateTime::currentDateTime().toString(Qt::ISODate);

    request.setCreatedAt(toStdString(now));
    request.setUpdatedAt(toStdString(now));

    // Store the new object in the collection.
    m_sosRequests.push_back(request);

    // If saving fails, remove the newly added object.
    if (!saveRequests())
    {
        m_sosRequests.pop_back();
        return "";
    }

    return toStdString(newRequestId);
}


// This overload accepts a request ID provided by the Victim Dashboard.
// OVERLOADING:
// This is an example of function overloading only if sosrequestservice.h
// declares another createRequestWithId() with the same name but
// a different parameter list. The code shown here alone does not prove it.

std::string SOSRequestService::createRequestWithId(
    const std::string& requestId,
    const std::string& victimId,
    const std::string& victimName,
    DisasterType disasterType,
    const std::string& requestLocation,
    Urgency requestUrgency,
    const std::string& helpRequired,
    const std::string& requestDescription,
    const std::string& createdAt)
{
    // Validate the ID and prevent duplicate requests.
    if (requestId.empty() ||
        findRequestById(requestId) != nullptr)
    {
        return "";
    }

    std::vector<AssistanceType> requiredAssistance =
        assistanceFromText(toQString(helpRequired));

    // OBJECT CREATION:
    // Create an SOSRequest object using the supplied details.
    SOSRequest request(
        requestId,
        victimId,
        disasterType,
        requestLocation,
        requestUrgency,
        requiredAssistance,
        requestDescription
        );

    // ENCAPSULATION:
    // Set values through the request's setter functions.
    request.setVictimName(victimName);
    request.setHelpRequired(helpRequired);
    request.setCreatedAt(createdAt);
    request.setRequestStatus(RequestStatus::Raised);
    request.setUpdatedAt(createdAt);

    m_sosRequests.push_back(request);

    // Roll back the in-memory addition if saving fails.
    if (!saveRequests())
    {
        m_sosRequests.pop_back();
        return "";
    }

    return requestId;
}


// ABSTRACTION:
// Provides one function to search for a request using its ID.
// The caller does not need to write the search loop itself.

SOSRequest* SOSRequestService::findRequestById(
    const std::string& requestId)
{
    // COLLECTION HANDLING:
    // Search through all stored SOSRequest objects.
    for (SOSRequest& request : m_sosRequests)
    {
        if (request.getRequestId() == requestId)
            return &request;
    }

    return nullptr;
}


// ENCAPSULATION:
// Provides read-only access to the collection through a const reference.
// The caller cannot modify the vector through this returned reference.

const std::vector<SOSRequest>&
SOSRequestService::getAllRequests() const
{
    return m_sosRequests;
}


// ABSTRACTION:
// Other modules can update a request's status by calling this function.
// The service handles searching, updating the timestamp, saving,
// and restoring the old values if saving fails.

bool SOSRequestService::updateRequestStatus(
    const std::string& requestId,
    RequestStatus newStatus)
{
    SOSRequest* request = findRequestById(requestId);

    if (request == nullptr)
        return false;

    // Save the old values so they can be restored if saving fails.
    RequestStatus oldStatus = request->getRequestStatus();
    std::string oldUpdatedAt = request->getUpdatedAt();

    // ENCAPSULATION:
    // Update the request through its setter.
    request->setRequestStatus(newStatus);

    QString now =
        QDateTime::currentDateTime().toString(Qt::ISODate);

    request->setUpdatedAt(toStdString(now));

    if (!saveRequests())
    {
        // Restore the original values if persistence fails.
        request->setRequestStatus(oldStatus);
        request->setUpdatedAt(oldUpdatedAt);
        return false;
    }

    return true;
}


// ABSTRACTION:
// Handles volunteer assignment as one service operation.
// The caller does not need to manage the request status,
// timestamp, or file persistence separately.

bool SOSRequestService::assignVolunteer(
    const std::string& requestId,
    const std::string& volunteerId)
{
    if (volunteerId.empty())
        return false;

    SOSRequest* request = findRequestById(requestId);

    if (request == nullptr)
        return false;

    // Save the old state in case saving fails.
    std::string oldVolunteer =
        request->getAssignedVolunteerId();

    RequestStatus oldStatus = request->getRequestStatus();
    std::string oldUpdatedAt = request->getUpdatedAt();

    // ENCAPSULATION:
    // Update the request through its public setter functions.
    request->setAssignedVolunteerId(volunteerId);
    request->setRequestStatus(RequestStatus::Assigned);

    QString now =
        QDateTime::currentDateTime().toString(Qt::ISODate);

    request->setUpdatedAt(toStdString(now));

    if (!saveRequests())
    {
        // Restore the original state on failure.
        request->setAssignedVolunteerId(oldVolunteer);
        request->setRequestStatus(oldStatus);
        request->setUpdatedAt(oldUpdatedAt);
        return false;
    }

    return true;
}


// ABSTRACTION:
// Returns the number of stored requests without exposing
// how the service internally stores or counts them.

std::size_t SOSRequestService::getRequestCount() const
{
    return m_sosRequests.size();
}


// ABSTRACTION + ENCAPSULATION:
// Updates the victim-related details of a particular request.
// The service controls the update and saving process.

bool SOSRequestService::updateVictimDetails(
    const std::string& requestId,
    const std::string& victimName,
    const std::string& helpRequired,
    const std::string& createdAt)
{
    SOSRequest* request = findRequestById(requestId);

    if (request == nullptr)
        return false;

    // Preserve the original values in case saving fails.
    std::string oldName = request->getVictimName();
    std::string oldHelp = request->getHelpRequired();
    std::string oldCreatedAt = request->getCreatedAt();

    // ENCAPSULATION:
    // Modify the SOSRequest object through setter methods.
    request->setVictimName(victimName);
    request->setHelpRequired(helpRequired);
    request->setCreatedAt(createdAt);

    if (!saveRequests())
    {
        // Restore the original values on failure.
        request->setVictimName(oldName);
        request->setHelpRequired(oldHelp);
        request->setCreatedAt(oldCreatedAt);
        return false;
    }

    return true;
}

} // namespace SOSConnect
