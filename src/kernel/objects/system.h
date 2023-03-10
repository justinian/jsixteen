#pragma once
/// \file system.h
/// Definition of kobject type representing the system

#include <j6/cap_flags.h>
#include "objects/kobject.h"

namespace obj {

class system :
    public kobject
{
public:
    /// Capabilities on system given to init
    static constexpr j6_cap_t init_caps = j6_cap_system_all;
    static constexpr kobject::type type = kobject::type::system;

    inline static system & get() { return s_instance; }

protected:
    /// Don't delete the system object on no handles.
    virtual void on_no_handles() override {}

private:
    static system s_instance;
    system() : kobject(type::system) {}
};

} // namespace obj
