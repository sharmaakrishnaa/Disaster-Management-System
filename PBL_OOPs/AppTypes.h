#ifndef APPTYPES_H
#define APPTYPES_H

namespace SOSConnect
{

enum class UserRole
{
    Victim,
    Volunteer,
    NGO,
    Administrator
};

enum class Urgency
{
    Low,
    Medium,
    High,
    Critical
};

enum class RequestStatus
{
    Raised,
    Assigned,
    InProgress,
    Resolved
};

enum class DisasterType
{
    Flood,
    Earthquake,
    Fire,
    Landslide,
    Cyclone,
    MedicalEmergency,
    Other
};

enum class AssistanceType
{
    Food,
    Water,
    Medicine,
    Blankets,
    Shelter,
    Rescue
};

} // namespace SOSConnect

#endif // APPTYPES_H