/*
    Copyright © 2020 r-neal-kelly, aka doticu
*/

#include "doticu_skylib/alias_reference.h"
#include "doticu_skylib/extra_text_display.h"
#include "doticu_skylib/game.h"
#include "doticu_skylib/interface.h"
#include "doticu_skylib/player.h"
#include "doticu_skylib/quest.h"
#include "doticu_skylib/ui.h"
#include "doticu_skylib/version.h"

#include "ini.h"
#include "plugin.h"

namespace doticu_skylib { namespace fix_blank_names {

    std::unordered_map<Reference_t*, String_t> Plugin_t::store;

    Plugin_t::Plugin_t() :
        SKSE_Plugin_t("doticu_fix_blank_names",
                      Version_t<u16>(1, 5, 97),
                      Operator_e::EQUAL_TO,
                      Version_t<u16>(2, 0, 17),
                      Operator_e::GREATER_THAN_OR_EQUAL_TO)
    {
        SKYLIB_LOG("doticu_fix_blank_names:");

        SKYLIB_LOG("- Update interval has been set to %llu seconds.", ini.update_interval_in_seconds);

        if (ini.restore_blank_names_algorithm == 0) {
            SKYLIB_LOG("- Using the loose restore algorithm. Restoring all references.");
        } else {
            SKYLIB_LOG("- Using the strict restore algorithm. Only restoring aliased references with the DO_STORE_NAME flag.");
        }

        if (ini.do_remove_blank_names) {
            if (ini.only_remove_blank_names_on_load_game) {
                SKYLIB_LOG("- Blank names will be removed only when a save-game is loaded.");
            } else {
                SKYLIB_LOG("- Blank names will be removed every update interval.");
            }
            if (ini.remove_blank_names_algorithm == 0) {
                SKYLIB_LOG("- Using the loose remove algorithm. Names with no characters or just whitespace are considered blank.");
            } else {
                SKYLIB_LOG("- Using the strict remove algorithm. Only names with no characters are considered blank.");
            }
        } else {
            SKYLIB_LOG("- Unrestorable blank names will not be touched.");
        }

        SKYLIB_LOG("");
    }

    Plugin_t::~Plugin_t()
    {
    }

    void Plugin_t::On_After_Load_Data()
    {
        Start_Updating(std::chrono::milliseconds(ini.update_interval_in_seconds * 1000));
    }

    void Plugin_t::On_After_New_Game()
    {
        store.clear();
    }

    void Plugin_t::On_Before_Save_Game(const std::string& file_name)
    {
        Restore_Names();
    }

    void Plugin_t::On_After_Save_Game(const std::string& file_name)
    {
    }

    void Plugin_t::On_Before_Load_Game(const std::string& file_name)
    {
        store.clear();
    }

    void Plugin_t::On_After_Load_Game(const std::string& file_name, Bool_t did_load_successfully)
    {
        if (did_load_successfully) {
            if (ini.do_remove_blank_names) {
                Remove_Blanks(std::move(Reference_t::All_References()));
            }
        }
    }

    void Plugin_t::On_Before_Delete_Game(const std::string& file_name)
    {
    }

    void Plugin_t::On_Update(u32 time_stamp)
    {
        Store_Names();
        Restore_Names();
        if (ini.do_remove_blank_names && !ini.only_remove_blank_names_on_load_game) {
            Remove_Blanks(std::move(Reference_t::Grid_References()));
        }
    }

    void Plugin_t::Store_Names()
    {
        class Iterator_t :
            public Iterator_i<some<Reference_t*>>
        {
        public:
            virtual Iterator_e operator ()(some<Reference_t*> reference) override
            {
                maybe<Extra_Text_Display_t*> x_text_display =
                    reference->x_list.Get<Extra_Text_Display_t>();
                if (x_text_display && x_text_display->Is_Custom() && x_text_display->name) {
                    store[reference()] = x_text_display->name;
                }
                return Iterator_e::CONTINUE;
            }
        } iterator;

        Reference_t::Iterate_All_References_Periodically(iterator, std::chrono::nanoseconds(0));
    }

    void Plugin_t::Restore_Names()
    {
        for (auto it = store.begin(); it != store.end();) {
            Read_Locker_t locker(Game_t::Form_IDs_To_Forms_Lock());

            maybe<Reference_t*> reference = it->first;
            String_t name = it->second;
            if (reference && name && Game_t::Has_Form(reference(), locker)) {
                maybe<Extra_Text_Display_t*> x_text_display =
                    reference->x_list.Get<Extra_Text_Display_t>();
                if (x_text_display && x_text_display->Is_Custom()) {
                    if (!x_text_display->name) {
                        auto Can_Apply_Strict = [](some<Reference_t*> reference)->Bool_t
                        {
                            Vector_t<some<Alias_Reference_t*>> aliases = reference->Alias_References();
                            for (size_t idx = 0, end = aliases.size(); idx < end; idx += 1) {
                                if (aliases[idx]->Do_Store_Name()) {
                                    return true;
                                }
                            }
                            return false;
                        };
                        if (ini.restore_blank_names_algorithm == 0 || Can_Apply_Strict(reference())) {
                            SKYLIB_LOG("Restoring a name that has become blank: %s", name);
                            reference->Log_Name_And_Type(SKYLIB_TAB);
                            reference->Name(name);
                        }
                    }
                    ++it;
                } else {
                    it = store.erase(it);
                }
            } else {
                it = store.erase(it);
            }
        }
    }

    void Plugin_t::Remove_Blanks(Vector_t<some<Reference_t*>> references)
    {
        Bool_t whitespace_counts_as_blank = ini.remove_blank_names_algorithm == 0;
        for (size_t idx = 0, end = references.size(); idx < end; idx += 1) {
            some<Reference_t*> reference = references[idx];
            if (reference->Remove_Blank_Name(whitespace_counts_as_blank)) {
                SKYLIB_LOG("Removed a blank name:");
                reference->Log_Name_And_Type(SKYLIB_TAB);
            }
        }
    }

    Plugin_t plugin;

}}

SKYLIB_EXPORT_SKSE_PLUGIN(doticu_skylib::fix_blank_names::plugin);
