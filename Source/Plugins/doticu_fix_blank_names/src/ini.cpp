/*
    Copyright © 2020 r-neal-kelly, aka doticu
*/

#pragma once

#include <fstream>

#include "doticu_skylib/cstring.h"
#include "doticu_skylib/intrinsic.h"

#include "ini.h"

namespace doticu_skylib { namespace fix_blank_names {

    INI_t::INI_t() :
        update_interval_in_seconds(DEFAULT_UPDATE_INTERVAL_IN_SECONDS),

        restore_blank_names_algorithm(DEFAULT_RESTORE_BLANK_NAMES_ALGORITHM),

        do_remove_blank_names(DEFAULT_DO_REMOVE_BLANK_NAMES),
        only_remove_blank_names_on_load_game(DEFAULT_ONLY_REMOVE_BLANK_NAMES_ON_LOAD_GAME),
        remove_blank_names_algorithm(DEFAULT_REMOVE_BLANK_NAMES_ALGORITHM)
    {
        std::ifstream file("./Data/SKSE/Plugins/doticu_fix_blank_names.ini");
        if (file && file.is_open()) {
            auto Read_Number = [](std::string& line, size_t start_idx)->u64
            {
                std::string value = "";
                for (size_t idx = start_idx, end = line.length(); idx < end; idx += 1) {
                    if (line[idx] >= '0' && line[idx] <= '9') {
                        value += line[idx];
                    }
                }
                return std::stoi(value);
            };

            std::string update_interval_in_seconds_string = "update_interval_in_seconds";

            std::string restore_blank_names_algorithm_string = "restore_blank_names_algorithm";

            std::string do_remove_blank_names_string = "do_remove_blank_names";
            std::string only_remove_blank_names_on_load_game_string = "only_remove_blank_names_on_load_game";
            std::string remove_blank_names_algorithm_string = "remove_blank_names_algorithm";

            for (std::string line; file && std::getline(file, line);) {
                if (!CString_t::Starts_With(line.c_str(), ";", true)) {
                    if (CString_t::Starts_With(line.c_str(), update_interval_in_seconds_string.c_str(), true)) {
                        u64 value = Read_Number(line, update_interval_in_seconds_string.length());
                        if (value < MIN_UPDATE_INTERVAL_IN_SECONDS) {
                            this->update_interval_in_seconds = MIN_UPDATE_INTERVAL_IN_SECONDS;
                        } else if (value > MAX_UPDATE_INTERVAL_IN_SECONDS) {
                            this->update_interval_in_seconds = MAX_UPDATE_INTERVAL_IN_SECONDS;
                        } else {
                            this->update_interval_in_seconds = value;
                        }

                    } else if (CString_t::Starts_With(line.c_str(), restore_blank_names_algorithm_string.c_str(), true)) {
                        u64 value = Read_Number(line, restore_blank_names_algorithm_string.length());
                        if (value > MAX_RESTORE_BLANK_NAMES_ALGORITHM) {
                            this->restore_blank_names_algorithm = DEFAULT_RESTORE_BLANK_NAMES_ALGORITHM;
                        } else {
                            this->restore_blank_names_algorithm = value;
                        }

                    } else if (CString_t::Starts_With(line.c_str(), do_remove_blank_names_string.c_str(), true)) {
                        u64 value = Read_Number(line, do_remove_blank_names_string.length());
                        this->do_remove_blank_names = value ? true : false;
                    } else if (CString_t::Starts_With(line.c_str(), only_remove_blank_names_on_load_game_string.c_str(), true)) {
                        u64 value = Read_Number(line, only_remove_blank_names_on_load_game_string.length());
                        this->only_remove_blank_names_on_load_game = value ? true : false;
                    } else if (CString_t::Starts_With(line.c_str(), remove_blank_names_algorithm_string.c_str(), true)) {
                        u64 value = Read_Number(line, remove_blank_names_algorithm_string.length());
                        if (value > MAX_REMOVE_BLANK_NAMES_ALGORITHM) {
                            this->remove_blank_names_algorithm = DEFAULT_REMOVE_BLANK_NAMES_ALGORITHM;
                        } else {
                            this->remove_blank_names_algorithm = value;
                        }
                    }
                }
            }
        }
    }

    INI_t::~INI_t()
    {
    }

    const INI_t ini;

}}
