#ifndef MAGIC_ITEM_BOX_H
#define MAGIC_ITEM_BOX_H

class ArmyCreator;

#include "army_list.h"
#include "enums.h"
#include "magic_item.h"
#include "option_selector.h"
#include "selection_tree.h"
#include "tools.h"

#include <algorithm>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QScrollArea>
#include <QSpinBox>
#include <QString>
#include <QTabWidget>

class MagicItemBox {
private:
    std::shared_ptr<ArmyCreator> creator;
    QTabWidget* box;
    QGroupBox* descr_box;
    QLabel* descr_label;
    std::shared_ptr<unit> current;
    ItemCategory ic_selected;

    // generic method to do some common checks and book-keeping before
    // calling specific function according to item_type value
    QScrollArea* make_items_tab(const std::unordered_map<std::string, item>& items,
                              ItemType item_type);
    QScrollArea* make_weapons_tab(const std::vector<std::pair<std::string, item>>& opt_weapons);
    QScrollArea* make_armour_tab(const std::vector<std::pair<std::string, item>>& opt_armour);
    QScrollArea* make_talismans_tab(const std::vector<std::pair<std::string, item>>& opt_talismans);
    QScrollArea* make_enchanted_tab(const std::vector<std::pair<std::string, item>>& opt_enchanted);
    QScrollArea* make_arcane_tab(const std::vector<std::pair<std::string, item>>& opt_arcane);
    QScrollArea* make_banners_tab(const std::vector<std::pair<std::string, item>>& opt_banners);
    QScrollArea* make_other_tab(const std::vector<std::pair<std::string, item>>& opt_other);

    bool check_itemtype_allowance(ItemType it) const;
public:
    explicit MagicItemBox(std::shared_ptr<ArmyCreator> creator_, QTabWidget* box_,
                          QGroupBox* descr_box_, QLabel* descr_label_);
    ~MagicItemBox() = default;

    void reset(const std::shared_ptr<unit>& current_);
    void reset_category(ItemCategory ic_selected_);

    bool reinitialise(ItemType focus);
    void clear();
};

#endif // !MAGIC_ITEM_BOX_H
