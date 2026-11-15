
#include "sosrequest.h"

namespace SOSConnect
{

// OOP CONCEPT 1: CONSTRUCTOR
// This constructor initializes a new SOSRequest object.
// It assigns the values received as arguments to the object's data members.
//
// OOP CONCEPT 2: ENCAPSULATION
// The m_ variables store the request's internal data.
// If these data members are declared private in sosrequest.h,
// they demonstrate data hiding.

SOSRequest::SOSRequest(
    const std::string& requestId,
    const std::string& victimId,
    DisasterType disasterType,
    const std::string& requestLocation,
    Urgency requestUrgency,
    const std::vector<AssistanceType>& requiredAssistance,
    const std::string& requestDescription
    )
    : m_requestId(requestId),
    m_victimId(victimId),
    m_victimName(""),
    m_disasterType(disasterType),
    m_requestLocation(requestLocation),
    m_requestUrgency(requestUrgency),
    m_requiredAssistance(requiredAssistance),
    m_helpRequired(""),
    m_requestDescription(requestDescription),
    m_requestStatus(RequestStatus::Raised),
    m_assignedVolunteerId(""),
    m_createdAt(""),
    m_updatedAt("")
{
}

// ======================================================
// ENCAPSULATION: GETTERS
// Getters provide controlled access to the object's data.
// The const at the end prevents these functions from
// modifying the object's state.
// Returning const references avoids unnecessary copies
// for string and vector members.
// ======================================================

const std::string& SOSRequest::getRequestId() const
{
    return m_requestId;
}

const std::string& SOSRequest::getVictimId() const
{
    return m_victimId;
}

const std::string& SOSRequest::getVictimName() const
{
    return m_victimName;
}

DisasterType SOSRequest::getDisasterType() const
{
    return m_disasterType;
}

const std::string& SOSRequest::getRequestLocation() const
{
    return m_requestLocation;
}

Urgency SOSRequest::getRequestUrgency() const
{
    return m_requestUrgency;
}

const std::vector<AssistanceType>&
SOSRequest::getRequiredAssistance() const
{
    return m_requiredAssistance;
}

const std::string& SOSRequest::getHelpRequired() const
{
    return m_helpRequired;
}

const std::string& SOSRequest::getRequestDescription() const
{
    return m_requestDescription;
}

RequestStatus SOSRequest::getRequestStatus() const
{
    return m_requestStatus;
}

const std::string& SOSRequest::getAssignedVolunteerId() const
{
    return m_assignedVolunteerId;
}

const std::string& SOSRequest::getCreatedAt() const
{
    return m_createdAt;
}

const std::string& SOSRequest::getUpdatedAt() const
{
    return m_updatedAt;
}

// ======================================================
// ENCAPSULATION: SETTERS
// Setters allow selected data members to be updated
// through class functions instead of being modified
// directly from outside the class.
// ======================================================

void SOSRequest::setVictimName(
    const std::string& victimName)
{
    m_victimName = victimName;
}

void SOSRequest::setHelpRequired(
    const std::string& helpRequired)
{
    m_helpRequired = helpRequired;
}

void SOSRequest::setCreatedAt(
    const std::string& createdAt)
{
    m_createdAt = createdAt;
}

void SOSRequest::setRequestStatus(
    RequestStatus newStatus)
{
    m_requestStatus = newStatus;
}

void SOSRequest::setAssignedVolunteerId(
    const std::string& volunteerId)
{
    m_assignedVolunteerId = volunteerId;
}

void SOSRequest::setUpdatedAt(
    const std::string& updatedAt)
{
    m_updatedAt = updatedAt;
}

// ABSTRACTION:
// Other modules can use functions such as getRequestStatus()
// and setRequestStatus() without needing to know how the
// request's data members are stored internally.
//
// DATA ORGANIZATION:
// The SOSConnect namespace groups this class with related
// project components and helps avoid naming conflicts.

} // namespace SOSConnect
