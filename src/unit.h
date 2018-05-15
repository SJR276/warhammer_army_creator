#ifndef UNIT_H
#define UNIT_H
#include "army_maps.h"
#include <array>
#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

/**
 * TODO: the name argument to a unit should initiate look-up
 * to a map to load the properties of that unit - do not need
 * points_per_model nor min_size args as they can be obtained
 * from this process. 
 */

class unit {
protected:
    armies::Faction race;
    std::string name;
    std::size_t points_per_model;
    std::size_t size;
    std::size_t min_size;
    std::array<short, 9U> stat_table;
    std::vector<std::pair<std::string, std::string>> equipment;
    std::vector<std::pair<std::string, std::string>> special_rules;
public:
    unit(armies::Faction faction,
         const std::string& name,
         std::size_t points_per_model,
         std::size_t size=0U,
         std::size_t min_size=0U);
    ~unit();
    const std::string& get_name() const noexcept;
    std::size_t points_value() const noexcept;
    std::size_t unit_size() const noexcept;
    std::size_t minimum_unit_size() const noexcept;
    // modifiers
    void set_stat_table(std::array<short, 9U>&& stats);
    void set_equipment(std::vector<std::pair<std::string, std::string>>&& eq);
    void set_special_rules(std::vector<std::pair<std::string, std::string>>&& sr);
};

// custom hash for unit to allow use in unordered containers
struct unit_hash {
    std::size_t operator()(const unit& _unit) const noexcept {
        std::size_t h1 = std::hash<std::string>{}(_unit.get_name());
        std::size_t h2 = std::hash<std::size_t>{}(_unit.unit_size());
        return h1 ^ (h2 << 1);
    }
};

#endif // !UNIT_H
