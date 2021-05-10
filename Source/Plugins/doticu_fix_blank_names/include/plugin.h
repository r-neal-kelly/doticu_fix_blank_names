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
        virtual Bool_t  On_Register(some<Virtual::Machine_t*> machine) override;
        virtual void    On_Message(some<SKSE_Message_t*> message) override;

    public:
        void    Access_Data();
        void    New_Game();
        void    Before_Save();
        void    Before_Load();
        void    After_Load();

    public:
        void    Begin();
    };

    extern Plugin_t plugin;

}}
