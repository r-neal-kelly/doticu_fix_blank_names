/*
    Copyright © 2020 r-neal-kelly, aka doticu
*/

#pragma once

#include <unordered_map>

#include "doticu_skylib/skse_plugin.h"

namespace doticu_skylib { namespace fix_blank_names {

    class Plugin_t :
        public SKSE_Plugin_t
    {
    public:
        static std::unordered_map<Reference_t*, String_t> store;

    public:
        Plugin_t();

    public:
        virtual         ~Plugin_t();

        virtual void    On_After_Load_Data() override;
        virtual void    On_After_New_Game() override;
        virtual void    On_Before_Save_Game(const std::string& file_name) override;
        virtual void    On_After_Save_Game(const std::string& file_name) override;
        virtual void    On_Before_Load_Game(const std::string& file_name) override;
        virtual void    On_After_Load_Game(const std::string& file_name, Bool_t did_load_successfully) override;
        virtual void    On_Before_Delete_Game(const std::string& file_name) override;
        virtual void    On_Update(u32 time_stamp) override;

    public:
        void Store_Names();
        void Restore_Names();
        void Remove_Blanks(Vector_t<some<Reference_t*>> references);
    };

    extern Plugin_t plugin;

}}
