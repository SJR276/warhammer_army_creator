#ifndef MAGE_CHARACTER_UNIT_H
#define MAGE_CHARACTER_UNIT_H

#include "army_maps.h"
#include "base_unit.h"
#include "base_mage_character_unit.h"
#include "_base_unit.h"
#include "_unit.h"

#include <memory>
#include <stdexcept>
#include <tuple>
#include <unordered_map>
#include <utility>

class mage_character_unit : public _unit {
private:
    // equipment
    std::unordered_map<
        WeaponType,
        std::tuple<ItemClass, std::string, double>
    > weapons_;
    std::unordered_map<
        ArmourType,
        std::tuple<ItemClass, std::string, double>
    > armours_;
    std::unordered_map<
        std::string,
        double
    > extras_;

    // item points
    double magic_item_points_;
    double faction_item_points_;
    double total_item_points_;

    // mage
    std::pair<short, double> level_;
public:
    const std::shared_ptr<base_mage_character_unit> handle;

    explicit mage_character_unit(std::shared_ptr<_base_unit> base);
    ~mage_character_unit() = default;

    // current property accessors
    std::size_t size() const noexcept;
    std::tuple<ItemClass, std::string, double> melee_weapon() const;
    std::tuple<ItemClass, std::string, double> ranged_weapon() const;

    std::unordered_map<
        ArmourType,
        std::tuple<ItemClass, std::string, double>
    > armour() const noexcept;
    std::unordered_map<
        std::string,
        double
    > extras() const noexcept;
    
    short level() const noexcept;

    // curremt property modifiers
    void pick_weapon(ItemClass item_type, std::string name);
    void pick_armour(ItemClass item_type, std::string name);
    void pick_extra(std::string name);
    void remove_weapon(WeaponType wt);
    void remove_armour(ArmourType at);
    void remove_extra(std::string name);

    void change_level(short lvl);
};

#endif // !MAGE_CHARACTER_UNIT_H