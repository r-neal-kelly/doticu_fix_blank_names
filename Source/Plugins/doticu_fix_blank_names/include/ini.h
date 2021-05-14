/*
    Copyright © 2020 r-neal-kelly, aka doticu
*/

#pragma once

#include "doticu_skylib/intrinsic.h"

namespace doticu_skylib { namespace fix_blank_names {

    class INI_t
    {
    public:
        static constexpr u64    DEFAULT_UPDATE_INTERVAL_IN_SECONDS              = 5;
        static constexpr u64    MIN_UPDATE_INTERVAL_IN_SECONDS                  = 3;
        static constexpr u64    MAX_UPDATE_INTERVAL_IN_SECONDS                  = 30;

        static constexpr u64    DEFAULT_RESTORE_BLANK_NAMES_ALGORITHM           = 0;
        static constexpr u64    MAX_RESTORE_BLANK_NAMES_ALGORITHM               = 1;

        static constexpr Bool_t DEFAULT_DO_REMOVE_BLANK_NAMES                   = true;
        static constexpr Bool_t DEFAULT_ONLY_REMOVE_BLANK_NAMES_ON_LOAD_GAME    = true;
        static constexpr u64    DEFAULT_REMOVE_BLANK_NAMES_ALGORITHM            = 1;
        static constexpr u64    MAX_REMOVE_BLANK_NAMES_ALGORITHM                = 1;

    public:
        u64     update_interval_in_seconds;

        u64     restore_blank_names_algorithm;

        Bool_t  do_remove_blank_names;
        Bool_t  only_remove_blank_names_on_load_game;
        u64     remove_blank_names_algorithm;

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
