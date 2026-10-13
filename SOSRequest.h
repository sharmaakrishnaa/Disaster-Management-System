#ifndef SOSREQUEST_H
#define SOSREQUEST_H
#include "AppTypes.h"

#include <string>

#include <vector>

namespace SOSConnect

{

class SOSRequest

{

private:

    // Unique identifiers

    std::string m_requestId;

    std::string m_victimId;

    // Disaster details

    DisasterType m_disasterType;

    std::string m_requestLocation;

    Urgency m_requestUrgency;

    std::vector<AssistanceType> m_requiredAssistance;

    std::string m_requestDescription;

    // Request tracking

    RequestStatus m_requestStatus;

    std::string m_assignedVolunteerId;

    std::string m_createdAt;

    std::string m_updatedAt;

public:

    // Constructor

    SOSRequest(

        const std::string& requestId,

        const std::string& victimId,

        DisasterType disasterType,

        const std::string& requestLocation,

        Urgency requestUrgency,

        const std::vector<AssistanceType>& requiredAssistance,

        const std::string& requestDescription

        );

    // Getters

    const std::string& getRequestId() const;

    const std::string& getVictimId() const;

    DisasterType getDisasterType() const;

    const std::string& getRequestLocation() const;

    Urgency getRequestUrgency() const;

    const std::vector<AssistanceType>& getRequiredAssistance() const;

    const std::string& getRequestDescription() const;

    RequestStatus getRequestStatus() const;

    const std::string& getAssignedVolunteerId() const;

    const std::string& getCreatedAt() const;

    const std::string& getUpdatedAt() const;

    // Setters

    void setRequestStatus(RequestStatus newStatus);

    void setAssignedVolunteerId(

        const std::string& volunteerId

        );

    void setUpdatedAt(

        const std::string& updatedAt

        );

};

} // namespace SOSConnect

#endif // SOSCONNECT_SOSREQUEST_H

