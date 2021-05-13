/*
    Copyright © 2020 r-neal-kelly, aka doticu
*/

#pragma once

#include "doticu_skylib/skse_plugin.h"

namespace doticu_skylib { namespace fix_blank_names {

    class Plugin_t :
        public SKSE_Plugin_t
    {
    public:
        Plugin_t();

    public:
        virtual         ~Plugin_t();

        virtual void    On_After_Load_Data() override;
        virtual void    On_After_New_Game() override;
        virtual void    On_Before_Save_Game() override;
        virtual void    On_After_Save_Game() override;
        virtual void    On_Before_Load_Game(some<const char*> file_path, u32 file_path_length) override;
        virtual void    On_After_Load_Game(Bool_t did_load_successfully) override;
        virtual void    On_Before_Delete_Game(some<const char*> file_path, u32 file_path_length) override;
        virtual void    On_Update(u32 time_stamp) override;

    public:
        void Prevent();
        void Remove_All();
        void Remove_Grid();
    };

    extern Plugin_t plugin;

}}
