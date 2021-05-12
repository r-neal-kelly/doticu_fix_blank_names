/*
    Copyright © 2020 r-neal-kelly, aka doticu
*/

#pragma once

#include <fstream>

#include "doticu_skylib/cstring.h"

#include "ini.h"

namespace doticu_skylib { namespace fix_blank_names {

    INI_t::INI_t() :
        update_interval_in_seconds(5),
        remove_blank_name_algorithm(1),
        only_remove_blank_names_on_load_game(true),
        only_prevent_blank_names(false)
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
            std::string remove_blank_name_algorithm_string = "remove_blank_name_algorithm";
            std::string only_remove_blank_names_on_load_game_string = "only_remove_blank_names_on_load_game";
            std::string only_prevent_blank_names_string = "only_prevent_blank_names";

            for (std::string line; file && std::getline(file, line);) {
                if (!CString_t::Starts_With(line.c_str(), ";", true)) {
                    if (CString_t::Starts_With(line.c_str(), update_interval_in_seconds_string.c_str(), true)) {
                        u64 value = Read_Number(line, update_interval_in_seconds_string.length());
                        if (value < 3) {
                            this->update_interval_in_seconds = 3;
                        } else if (value > 30) {
                            this->update_interval_in_seconds = 30;
                        } else {
                            this->update_interval_in_seconds = value;
                        }

                    } else if (CString_t::Starts_With(line.c_str(), remove_blank_name_algorithm_string.c_str(), true)) {
                        u64 value = Read_Number(line, remove_blank_name_algorithm_string.length());
                        if (value < 2) {
                            this->remove_blank_name_algorithm = value;
                        } else {
                            this->remove_blank_name_algorithm = 1;
                        }

                    } else if (CString_t::Starts_With(line.c_str(), only_remove_blank_names_on_load_game_string.c_str(), true)) {
                        u64 value = Read_Number(line, only_remove_blank_names_on_load_game_string.length());
                        this->only_remove_blank_names_on_load_game = value ? true : false;

                    } else if (CString_t::Starts_With(line.c_str(), only_prevent_blank_names_string.c_str(), true)) {
                        u64 value = Read_Number(line, only_prevent_blank_names_string.length());
                        this->only_prevent_blank_names = value ? true : false;
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
