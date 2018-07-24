#ifndef SELECTION_TREE_H
#define SELECTION_TREE_H

#include "army_maps.h"
#include "army_list.h"
#include "base_unit.h"
#include "base_character_unit.h"
#include "base_mage_character_unit.h"
#include "base_melee_character_unit.h"
#include "base_mixed_unit.h"
#include "base_normal_unit.h"
#include "item_parser.h"
#include "mage_character_unit.h"
#include "magic_item.h"
#include "melee_character_unit.h"
#include "mixed_unit.h"
#include "normal_unit.h"
#include "roster_parser.h"
#include "tools.h"
#include "unit.h"

#include <array>
#include <cmath>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <QString>

class selection_tree {
private:
    armies::Faction race;
    std::unordered_map<
        std::string,
        std::shared_ptr<base_unit>
    > roster;
    std::unordered_map<
        std::string,
        magic_item
    > magic_items;
    std::shared_ptr<unit> current_selection;

    std::reference_wrapper<army_list> army;

    std::pair<std::string, std::string> filenames() const noexcept;
    void parse_roster_file(const QString& rfile_str);
    void parse_item_file(const QString& ifile_str);
    std::vector<std::shared_ptr<base_unit>> all_of(armies::UnitType ut) const noexcept;
public:
    selection_tree(armies::Faction faction, army_list& list);
    ~selection_tree() = default;
    void add_unit_to_army_list(int id);
    void change_selection(const std::string& name);
    void reset_army_list(army_list&_army);
    void reset(armies::Faction faction, army_list& list);

    std::shared_ptr<unit> selected();

    std::vector<std::shared_ptr<base_unit>> lords() const noexcept;
    std::vector<std::shared_ptr<base_unit>> heroes() const noexcept;
    std::vector<std::shared_ptr<base_unit>> core() const noexcept;
    std::vector<std::shared_ptr<base_unit>> special() const noexcept;
    std::vector<std::shared_ptr<base_unit>> rare() const noexcept;

};

#endif // !SELECTION_TREE_H
