/*
    Copyright © 2020 r-neal-kelly, aka doticu
*/

#pragma once

#include <fstream>

#include "doticu_skylib/cstring.h"

#include "ini.h"

namespace doticu_skylib { namespace fix_blank_names {

    INI_t::INI_t() :
        fix_interval_in_seconds(5),
        blank_name_algorithm(0)
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

            std::string fix_interval_in_seconds_string = "fix_interval_in_seconds";
            std::string blank_name_algorithm_string = "blank_name_algorithm";

            for (std::string line; file && std::getline(file, line);) {
                if (CString_t::Starts_With(line.c_str(), fix_interval_in_seconds_string.c_str(), true)) {
                    u64 value = Read_Number(line, fix_interval_in_seconds_string.length());
                    if (value < 3) {
                        this->fix_interval_in_seconds = 3;
                    } else if (value > 30) {
                        this->fix_interval_in_seconds = 30;
                    } else {
                        this->fix_interval_in_seconds = value;
                    }
                } else if (CString_t::Starts_With(line.c_str(), blank_name_algorithm_string.c_str(), true)) {
                    u64 value = Read_Number(line, blank_name_algorithm_string.length());
                    if (value < 2) {
                        this->blank_name_algorithm = value;
                    } else {
                        this->blank_name_algorithm = 0;
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
