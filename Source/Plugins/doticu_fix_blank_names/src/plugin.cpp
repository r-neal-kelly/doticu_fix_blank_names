/*
    Copyright © 2020 r-neal-kelly, aka doticu
*/

#include <thread>

#include "doticu_skylib/player.h"
#include "doticu_skylib/ui.h"
#include "doticu_skylib/version.h"

#include "plugin.h"

namespace doticu_skylib { namespace fix_blank_names {

    std::mutex Plugin_t::lock;

    Plugin_t::Plugin_t() :
        SKSE_Plugin_t("doticu_fix_blank_names",
                      Version_t<u16>(1, 5, 97),
                      Operator_e::EQUAL_TO,
                      Version_t<u16>(2, 0, 17),
                      Operator_e::GREATER_THAN_OR_EQUAL_TO),
        locker(Plugin_t::lock)
    {
    }

    Plugin_t::~Plugin_t()
    {
    }

    Bool_t Plugin_t::On_Register(some<Virtual::Machine_t*> machine)
    {
        return true;
    }

    void Plugin_t::On_After_Load_Data(some<Game_t*> game)
    {
        std::thread(
            []()->void
            {
                SKYLIB_LOG("Running. Every reference in the loaded grid will be checked every five seconds.");

                Player_t& player = *Player_t::Self();
                UI_t& ui = UI_t::Self();
                u32 time = 0;
                while (true) {
                    {
                        std::lock_guard<std::mutex> locker(Plugin_t::lock);
                        if (time != ui.game_timer.total_time) {
                            time = ui.game_timer.total_time;
                            if (player.Is_Attached()) {
                                Vector_t<some<Reference_t*>> grid = Reference_t::Grid_References();
                                for (size_t idx = 0, end = grid.size(); idx < end; idx += 1) {
                                    grid[idx]->Remove_Blank_Name();
                                }
                            }
                        }
                    }
                    std::this_thread::sleep_for(std::chrono::seconds(5));
                }
            }
        ).detach();
    }

    void Plugin_t::On_After_New_Game()
    {
    }

    void Plugin_t::On_Before_Save_Game()
    {
        if (!this->locker.owns_lock()) {
            this->locker.lock();
        }
    }

    void Plugin_t::On_After_Save_Game()
    {
        if (this->locker.owns_lock()) {
            this->locker.unlock();
        }
    }

    void Plugin_t::On_Before_Load_Game(some<const char*> file_path, u32 file_path_length)
    {
        if (!this->locker.owns_lock()) {
            this->locker.lock();
        }
    }

    void Plugin_t::On_After_Load_Game(Bool_t did_load_successfully)
    {
        if (this->locker.owns_lock()) {
            this->locker.unlock();
        }
    }

    void Plugin_t::On_Before_Delete_Game(some<const char*> file_path, u32 file_path_length)
    {
    }

    Plugin_t plugin;

}}

SKYLIB_EXPORT_SKSE_PLUGIN(doticu_skylib::fix_blank_names::plugin);
