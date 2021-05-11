/*
    Copyright © 2020 r-neal-kelly, aka doticu
*/

#pragma once

#include "doticu_skylib/intrinsic.h"

namespace doticu_skylib { namespace fix_blank_names {

    class INI_t
    {
    public:
        u64 fix_interval_in_seconds;
        u64 blank_name_algorithm;

    public:
        INI_t();
        INI_t(const INI_t& other) = delete;
        INI_t(INI_t&& other) noexcept = delete;
        INI_t& operator =(const INI_t& other) = delete;
        INI_t& operator =(INI_t&& other) noexcept = delete;
        ~INI_t();
    };

    extern const INI_t ini;

}}
