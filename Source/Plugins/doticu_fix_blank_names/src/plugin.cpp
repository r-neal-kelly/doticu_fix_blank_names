/*
    Copyright © 2020 r-neal-kelly, aka doticu
*/

#include <thread>
#include <unordered_set>

#include "doticu_skylib/alias_base.h"
#include "doticu_skylib/alias_reference.h"
#include "doticu_skylib/extra_text_display.h"
#include "doticu_skylib/interface.h"
#include "doticu_skylib/player.h"
#include "doticu_skylib/quest.h"
#include "doticu_skylib/ui.h"
#include "doticu_skylib/version.h"

#include "ini.h"
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

    void Plugin_t::On_After_Load_Data(Start_Updating_f start_updating_f)
    {
        SKYLIB_LOG("doticu_fix_blank_names:");
        SKYLIB_LOG("- Update interval has been set to %llu seconds.", ini.update_interval_in_seconds);
        if (ini.only_prevent_blank_names) {
            SKYLIB_LOG("- Blank names will only be prevented and never removed.");
        } else {
            if (ini.only_remove_blank_names_on_load_game) {
                SKYLIB_LOG("- Blank names will be removed only when a save-game is loaded.");
            } else {
                SKYLIB_LOG("- Blank names will be removed every update interval.");
            }
            if (ini.remove_blank_name_algorithm == 0) {
                SKYLIB_LOG("- Using the loose algorithm. Names with no characters or just whitespace are considered blank.");
            } else {
                SKYLIB_LOG("- Using the strict algorithm. Only names with no characters are considered blank.");
            }
        }
        SKYLIB_LOG("");

        std::thread(
            []()->void
            {
                Player_t& player = *Player_t::Self();
                UI_t& ui = UI_t::Self();
                u32 time = 0;
                while (true) {
                    {
                        std::lock_guard<std::mutex> locker(Plugin_t::lock);
                        if (time != ui.game_timer.total_time) {
                            time = ui.game_timer.total_time;
                            if (player.Is_Attached()) {
                                plugin.Prevent();
                                if (!ini.only_remove_blank_names_on_load_game) {
                                    plugin.Remove_Grid();
                                }
                            }
                        }
                    }
                    std::this_thread::sleep_for(std::chrono::seconds(ini.update_interval_in_seconds));
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
        if (ini.only_remove_blank_names_on_load_game) {
            Remove_All();
        }

        if (this->locker.owns_lock()) {
            this->locker.unlock();
        }
    }

    void Plugin_t::On_Before_Delete_Game(some<const char*> file_path, u32 file_path_length)
    {
    }

    void Plugin_t::On_Update()
    {
    }

    static void Test(some<Alias_Reference_t*> alias, some<Reference_t*> reference, some<Extra_Text_Display_t*> x_text_display)
    {
        if (reference->form_id == 0x00019DEA) {
            reference->Move_To_Orbit(Player_t::Self(), 0.0f, 0.0f);

            x_text_display->Name("Blah!", true);
            x_text_display->owning_quest = none<Quest_t*>();

            class Callback :
                public Callback_i<>
            {
            public:
                some<Alias_Reference_t*> alias;
                some<Reference_t*> reference;

            public:
                Callback(some<Alias_Reference_t*> alias, some<Reference_t*> reference) :
                    alias(alias), reference(reference)
                {
                }

            public:
                virtual void operator ()() override
                {
                    class Callback2 :
                        public Callback_i<>
                    {
                    public:
                        some<Alias_Reference_t*> alias;
                        some<Reference_t*> reference;

                    public:
                        Callback2(some<Alias_Reference_t*> alias, some<Reference_t*> reference) :
                            alias(alias), reference(reference)
                        {
                        }

                    public:
                        virtual void operator ()() override
                        {
                            if (String_t(reference->Name()) == String_t("")) {
                                reference->Log_Name_And_Type(SKYLIB_TAB);
                            }
                        }
                    };
                    alias->Fill(reference, new Callback2(alias, reference));
                }
            };
            alias->Unfill(new Callback(alias, reference));
        }
    }

    void Plugin_t::Prevent()
    {
        Vector_t<some<Quest_t*>> quests = Quest_t::Quests_Static();
        for (size_t idx = 0, end = quests.size(); idx < end; idx += 1) {
            some<Quest_t*> quest = quests[idx];
            Write_Locker_t locker(quest->aliases_lock);
            for (size_t idx = 0, end = quest->aliases.Count(); idx < end; idx += 1) {
                maybe<Alias_Reference_t*> alias = quest->aliases[idx];
                if (alias && quest->Has_Filled_Alias(alias->id)) {
                    class Callback :
                        public Callback_i<maybe<Reference_t*>>
                    {
                    public:
                        some<Quest_t*> quest;
                        some<Alias_Reference_t*> alias;

                    public:
                        Callback(some<Quest_t*> quest, some<Alias_Reference_t*> alias) :
                            quest(quest), alias(alias)
                        {
                        }

                    public:
                        virtual void operator ()(maybe<Reference_t*> reference) override
                        {
                            if (reference) {
                                Write_Locker_t locker(reference->x_list.lock);
                                maybe<Extra_Text_Display_t*> x_text_display = reference->x_list.Get<Extra_Text_Display_t>(locker);
                                if (x_text_display) {
                                    if (!x_text_display->owning_quest) {
                                        x_text_display->owning_quest = this->quest;
                                        SKYLIB_LOG("Potentially prevented a blank name:");
                                        reference->Log_Name_And_Type(SKYLIB_TAB);
                                    }
                                }
                            }
                        }
                    };
                    alias->To_Reference(new Callback(quest, alias()));
                }
            }
        }
    }

    void Plugin_t::Remove_All()
    {
        if (!ini.only_prevent_blank_names) {
            Bool_t whitespace_counts_as_blank = ini.remove_blank_name_algorithm == 0;
            Vector_t<some<Reference_t*>> all = Reference_t::All_References();
            for (size_t idx = 0, end = all.size(); idx < end; idx += 1) {
                some<Reference_t*> reference = all[idx];
                if (reference->Remove_Blank_Name(whitespace_counts_as_blank)) {
                    SKYLIB_LOG("Successfully removed blank name:");
                    reference->Log_Name_And_Type(SKYLIB_TAB);
                }
            }
        }
    }

    void Plugin_t::Remove_Grid()
    {
        if (!ini.only_prevent_blank_names) {
            Bool_t whitespace_counts_as_blank = ini.remove_blank_name_algorithm == 0;
            Vector_t<some<Reference_t*>> grid = Reference_t::Grid_References();
            for (size_t idx = 0, end = grid.size(); idx < end; idx += 1) {
                some<Reference_t*> reference = grid[idx];
                if (reference->Remove_Blank_Name(whitespace_counts_as_blank)) {
                    SKYLIB_LOG("Successfully removed blank name:");
                    reference->Log_Name_And_Type(SKYLIB_TAB);
                }
            }
        }
    }

    Plugin_t plugin;

}}

SKYLIB_EXPORT_SKSE_PLUGIN(doticu_skylib::fix_blank_names::plugin);
