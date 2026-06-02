#pragma once
#include <string>

class ResBase {
public:
    ResBase() = default;
    virtual ~ResBase() = default;

    virtual std::string GetId() const = 0;

    virtual void OnLoad() {

    }

    bool operator<(const ResBase& other) const {
        return GetId() < other.GetId();
    }

    bool operator>(const ResBase& other) const {
        return GetId() > other.GetId();
    }

    bool operator<=(const ResBase& other) const {
        return GetId() <= other.GetId();
    }

    bool operator>=(const ResBase& other) const {
        return GetId() >= other.GetId();
    }

    bool operator==(const ResBase& other) const {
        return GetId() == other.GetId();
    }

    bool operator!=(const ResBase& other) const {
        return GetId() != other.GetId();
    }

    virtual bool LoadFromPb(std::string data) = 0;
};
