/*
    Copyright © 2020 r-neal-kelly, aka doticu
*/

#include "doticu_skylib/reference.h"
#include "doticu_skylib/version.h"
#include "doticu_skylib/virtual_callback.h"
#include "doticu_skylib/virtual_utility.h"

#include "plugin.h"

namespace doticu_skylib { namespace fix_blank_names {

    Plugin_t::Plugin_t() :
        SKSE_Plugin_t("doticu_fix_blank_names",
                      Version_t<u16>(1, 5, 97),
                      Operator_e::EQUAL_TO,
                      Version_t<u16>(2, 0, 17),
                      Operator_e::GREATER_THAN_OR_EQUAL_TO)
    {
    }

    Plugin_t::~Plugin_t()
    {
    }

    Bool_t Plugin_t::On_Register(some<Virtual::Machine_t*> machine)
    {
        return true;
    }

    void Plugin_t::On_Message(some<SKSE_Message_t*> message)
    {
        if (message->type == SKSEMessagingInterface::kMessage_SaveGame) {
            Before_Save();
        } else if (message->type == SKSEMessagingInterface::kMessage_PreLoadGame) {
            Before_Load();
        } else if (message->type == SKSEMessagingInterface::kMessage_PostLoadGame && message->data) {
            After_Load();
        } else if (message->type == SKSEMessagingInterface::kMessage_NewGame) {
            New_Game();
        } else if (message->type == SKSEMessagingInterface::kMessage_DataLoaded) {
            Access_Data();
        }
    }

    void Plugin_t::Access_Data()
    {
        SKYLIB_LOG("Running doticu_fix_blank_names. Every reference in the loaded grid will be checked every 5 seconds.");
    }

    void Plugin_t::New_Game()
    {
        Begin();
    }

    void Plugin_t::Before_Save()
    {
    }

    void Plugin_t::Before_Load()
    {
    }

    void Plugin_t::After_Load()
    {
        Begin();
    }

    void Plugin_t::Begin()
    {
        class Callback :
            public Virtual::Callback_t
        {
        public:
            virtual void operator ()(Virtual::Variable_t*) override
            {
                Vector_t<some<Reference_t*>> grid = Reference_t::Grid_References();
                for (size_t idx = 0, end = grid.size(); idx < end; idx += 1) {
                    grid[idx]->Remove_Blank_Name();
                }
                Virtual::Utility_t::Wait_Even_In_Menu(5.0f, new Callback());
            }
        };
        Virtual::Utility_t::Wait_Even_In_Menu(5.0f, new Callback());
    }

    extern Plugin_t plugin = Plugin_t();

}}

SKYLIB_EXPORT_SKSE_PLUGIN(doticu_skylib::fix_blank_names::plugin);
