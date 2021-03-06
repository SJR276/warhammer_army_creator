#include "optionbox.h"
#include "armycreator.h"

OptionBox::OptionBox(std::shared_ptr<ArmyCreator> creator_, QGroupBox* box_) :
    creator(creator_), box(box_), current(), in_tree(InTree::NEITHER) {}

void OptionBox::command_box_helper(QLayout* layout,
    const std::unordered_map<
        CommandGroup, std::pair<std::string, double>
    >& opt_command,
    const std::unordered_map<
        CommandGroup, std::pair<std::string, double>
    >& command,
    bool master) {
    if (opt_command.count(CommandGroup::MUSICIAN)) {
        auto musician = opt_command.at(CommandGroup::MUSICIAN);
        std::string name = musician.first + " (" + tools::points_str(musician.second) + " pts)";
        QCheckBox* cb = new QCheckBox(creator->tr(name.data()));
        if (command.count(CommandGroup::MUSICIAN)) cb->setChecked(true);
        creator->connect(cb, &QCheckBox::clicked, creator.get(), [this, master](bool checked) {
            creator->optional_command_selected(CommandGroup::MUSICIAN, checked, master);
        });
        layout->addWidget(cb);
    }
    if (opt_command.count(CommandGroup::STANDARD_BEARER)) {
        auto sb = opt_command.at(CommandGroup::STANDARD_BEARER);
        std::string name = sb.first + " (" + tools::points_str(sb.second) + " pts)";
        QCheckBox* cb = new QCheckBox(creator->tr(name.data()));
        if (command.count(CommandGroup::STANDARD_BEARER)) cb->setChecked(true);
        creator->connect(cb, &QCheckBox::clicked, creator.get(), [this, master](bool checked) {
            creator->optional_command_selected(CommandGroup::STANDARD_BEARER, checked, master);
        });
        layout->addWidget(cb);
    }
    if (opt_command.count(CommandGroup::CHAMPION)) {
        auto champ = opt_command.at(CommandGroup::CHAMPION);
        std::string name = champ.first + " (" + tools::points_str(champ.second) + " pts)";
        QCheckBox* cb = new QCheckBox(creator->tr(name.data()));
        if (command.count(CommandGroup::CHAMPION)) cb->setChecked(true);
        creator->connect(cb, &QCheckBox::clicked, creator.get(), [this, master](bool checked) {
            creator->optional_command_selected(CommandGroup::CHAMPION, checked, master);
        });
        layout->addWidget(cb);
    }
}

void OptionBox::clear() {
    auto c = box->children();
    for (auto& x : c) delete x;
    //current.reset();
}

void OptionBox::reset(const std::shared_ptr<unit>& current_, InTree in_tree_) {
    current = current_;
    in_tree = in_tree_;
}

bool OptionBox::reinitialise() {
    if (current == nullptr) return false;
    std::string box_title = current->name() + " Options";
    box->setTitle(creator->tr(box_title.data()));
    QVBoxLayout* vbox = new QVBoxLayout;
    QGroupBox* sc_box = make_size_command_box();
    if (sc_box != nullptr) vbox->addWidget(sc_box);
    auto weapons_boxes = make_weapons_boxes();
    if (weapons_boxes.first != nullptr) vbox->addWidget(weapons_boxes.first);
    if (weapons_boxes.second != nullptr) vbox->addWidget(weapons_boxes.second);
    auto armour_boxes = make_armour_boxes();
    if (armour_boxes.first != nullptr) vbox->addWidget(armour_boxes.first);
    if (armour_boxes.second != nullptr) vbox->addWidget(armour_boxes.second);
    if (current->is_mage()) {
        auto mage_opt_box = make_mage_options_boxes();
        if (mage_opt_box.first != nullptr) vbox->addWidget(mage_opt_box.first);
        if (mage_opt_box.second != nullptr) vbox->addWidget(mage_opt_box.second);
    }
    auto mount_box = make_mounts_boxes();
    if (mount_box != nullptr) vbox->addWidget(mount_box);
    auto extras_boxes = make_extras_boxes();
    if (extras_boxes.first != nullptr) vbox->addWidget(extras_boxes.first);
    if (extras_boxes.second != nullptr) vbox->addWidget(extras_boxes.second);
    box->setLayout(vbox);
    return true;
}

QGroupBox* OptionBox::make_size_command_box() {
    switch (current->base_unit_type()) {
    case BaseUnitType::NORMAL:
        if (current->unit_class() == UnitCategory::MONSTER) return nullptr;
        break;
    case BaseUnitType::MIXED:
        break;
    default: return nullptr;
    }
    QGroupBox* sc_groupbox = new QGroupBox(creator->tr("Size and Command Group"));
    QVBoxLayout* sc_box_layout = new QVBoxLayout;
    QHBoxLayout* size_box_layout = new QHBoxLayout;
    // command group
    QGroupBox* command_box = nullptr;
    if (current->base_unit_type() == BaseUnitType::NORMAL) {
        // unit size
        QLabel* label = new QLabel(creator->tr("Unit size"));
        QSpinBox* size_sb = new QSpinBox();
        auto p = std::dynamic_pointer_cast<normal_unit>(current);
        size_sb->setMinimum(static_cast<int>(p->min_size()));
        size_sb->setMaximum(
            (p->max_size() == std::numeric_limits<std::size_t>::max()) ?
                1000 : static_cast<int>(p->max_size())
        );
        // set value of spinbox to current unit size
        if (enum_convert::in_army_trees(in_tree)) size_sb->setValue(static_cast<int>(p->size()));
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
        creator->connect(size_sb, QOverload<int>::of(&QSpinBox::valueChanged), creator.get(), [this](int value) {
            creator->change_unit_size(value, true);
        });
#else
        creator->connect(size_sb, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), creator, [this](int value) {
            creator->change_unit_size(value, true);
        });
#endif
        size_box_layout->addWidget(label);
        size_box_layout->addWidget(size_sb);
        size_box_layout->addStretch(1);
        sc_box_layout->addLayout(size_box_layout);
    }
    else { // BaseUnitType::MIXED
        auto p = std::dynamic_pointer_cast<mixed_unit>(current);
        // master
        std::string master_label_name = p->handle->master_name() + " models";
        QLabel* master_label = new QLabel(creator->tr(master_label_name.data()));
        QSpinBox* master_size_sb = new QSpinBox();
        master_size_sb->setMinimum(static_cast<int>(p->handle->master_min_size()));
        master_size_sb->setMaximum(
            (p->handle->master_max_size() == std::numeric_limits<std::size_t>::max()) ?
                1000 : static_cast<int>(p->handle->master_max_size())
        );
        // slave
        std::string slave_label_name = p->handle->slave_name() + " models";
        QLabel* slave_label = new QLabel(creator->tr(slave_label_name.data()));
        QSpinBox* slave_size_sb = new QSpinBox();
        slave_size_sb->setMinimum(static_cast<int>(p->handle->slave_min_size()));
        slave_size_sb->setMaximum(
            (p->handle->slave_max_size() == std::numeric_limits<std::size_t>::max()) ?
                1000 : static_cast<int>(p->handle->slave_max_size())
        );
        if (enum_convert::in_army_trees(in_tree)) {
            master_size_sb->setValue(static_cast<int>(p->master_size()));
            slave_size_sb->setValue(static_cast<int>(p->slave_size()));
        }
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
        creator->connect(master_size_sb, QOverload<int>::of(&QSpinBox::valueChanged), creator.get(), [this](int value) {
            creator->change_unit_size(value, true);
        });
        creator->connect(slave_size_sb, QOverload<int>::of(&QSpinBox::valueChanged), creator.get(), [this](int value) {
            creator->change_unit_size(value, false);
        });
#else
        creator->connect(master_size_sb, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), creator, [this](int value) {
            creator->change_unit_size(value, true);
        });
        creator->connect(slave_size_sb, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), creator, [this](int value) {
            creator->change_unit_size(value, false);
        });
#endif
        size_box_layout->addWidget(master_label);
        size_box_layout->addWidget(master_size_sb);
        size_box_layout->addWidget(slave_label);
        size_box_layout->addWidget(slave_size_sb);
        size_box_layout->addStretch(1);
        sc_box_layout->addLayout(size_box_layout);
    }
    command_box = make_command_box();
    if (command_box != nullptr) sc_box_layout->addWidget(command_box);
    sc_box_layout->addStretch(1);
    sc_groupbox->setLayout(sc_box_layout);
    return sc_groupbox;
}

QGroupBox* OptionBox::make_command_box() {
    switch (current->base_unit_type()) {
    case BaseUnitType::NORMAL:
    {
        auto p = std::dynamic_pointer_cast<normal_unit>(current);
        auto opt_command = p->handle->optional_command();
        std::unordered_map<CommandGroup, std::pair<std::string, double>> command;
        if (enum_convert::in_army_trees(in_tree)) command = p->command();
        if (opt_command.empty()) return nullptr;
        QGroupBox* gb = new QGroupBox(creator->tr("Command Group"));
        QVBoxLayout* vbl = new QVBoxLayout;
        command_box_helper(vbl, opt_command, command);
        vbl->addStretch(1);
        gb->setLayout(vbl);
        return gb;
    }
    case BaseUnitType::MIXED:
    {
        auto p = std::dynamic_pointer_cast<mixed_unit>(current);
        auto opt_master_command = p->handle->master_optional_command();
        auto opt_slave_command = p->handle->slave_optional_command();
        if (opt_master_command.empty() && opt_slave_command.empty()) return nullptr;
        std::unordered_map<CommandGroup, std::pair<std::string, double>> master_command;
        std::unordered_map<CommandGroup, std::pair<std::string, double>> slave_command;
        if (enum_convert::in_army_trees(in_tree)) {
            master_command = p->master_command();
            slave_command = p->slave_command();
        }
        QGroupBox* gb = new QGroupBox(creator->tr("Command Groups"));
        QHBoxLayout* hlayout = new QHBoxLayout;
        // master command group
        if (!opt_master_command.empty()) {
            QGroupBox* master_gb = new QGroupBox(
                creator->tr((p->handle->master_name() + " Command").data())
            );
            QVBoxLayout* master_vbl = new QVBoxLayout;
            command_box_helper(master_vbl, opt_master_command, master_command, true);
            master_vbl->addStretch(1);
            master_gb->setLayout(master_vbl);
            hlayout->addWidget(master_gb);
        }
        // slave command group
        if (!opt_slave_command.empty()) {
            QGroupBox* slave_gb = new QGroupBox(
                creator->tr((p->handle->slave_name() + " Command").data())
            );
            QVBoxLayout* slave_vbl = new QVBoxLayout;
            command_box_helper(slave_vbl, opt_slave_command, slave_command);
            slave_vbl->addStretch(1);
            slave_gb->setLayout(slave_vbl);
            hlayout->addWidget(slave_gb);
        }
        // now finalise both
        gb->setLayout(hlayout);
        return gb;
    }
    default: return nullptr;
    }
}

std::pair<QGroupBox*, QGroupBox*> OptionBox::make_weapons_boxes() {
    bool has_opt_weapons = false;
    bool has_champ_opt_weapons = false;
    std::pair<QGroupBox*, QGroupBox*> boxes = {nullptr, nullptr};
    switch (current->base_unit_type()) {
    case BaseUnitType::MAGE_CHARACTER:
    case BaseUnitType::MELEE_CHARACTER:
    {
        auto p = std::dynamic_pointer_cast<character_unit>(current);
        if (p->handle_->opt().weapons().empty()) return boxes;
        has_opt_weapons = true;
        break;
    }
    case BaseUnitType::NORMAL:
    {
        auto p = std::dynamic_pointer_cast<normal_unit>(current);
        if (p->handle->opt().weapons().empty() && p->handle->champion_opt().weapons().empty())
            return boxes;
        if (!p->handle->opt().weapons().empty()) has_opt_weapons = true;
        if (!p->handle->champion_opt().weapons().empty()) has_champ_opt_weapons = true;
        break;
    }
    case BaseUnitType::MIXED:
    {
        auto p = std::dynamic_pointer_cast<mixed_unit>(current);
        if (p->handle->master_opt().weapons().empty() && p->handle->master_champion_opt().weapons().empty() &&
                p->handle->slave_opt().weapons().empty() && p->handle->slave_champion_opt().weapons().empty())
            return boxes;
        if (!p->handle->master_opt().weapons().empty() || !p->handle->slave_opt().weapons().empty())
            has_opt_weapons = true;
        if (!p->handle->master_champion_opt().weapons().empty() ||
                !p->handle->slave_champion_opt().weapons().empty())
            has_champ_opt_weapons = true;
        break;
    }
    default: return boxes;
    }
    if (has_opt_weapons) {
        QGroupBox* weapons_box = new QGroupBox(creator->tr("Weapons"));
        QVBoxLayout* weapons_box_layout = new QVBoxLayout;
        auto melee_subbox = make_weapons_subbox(WeaponType::MELEE, false);
        if (melee_subbox != nullptr) weapons_box_layout->addWidget(melee_subbox);
        auto ranged_subbox = make_weapons_subbox(WeaponType::BALLISTIC, false);
        if (ranged_subbox != nullptr) weapons_box_layout->addWidget(ranged_subbox);
        weapons_box_layout->addStretch(1);
        weapons_box->setLayout(weapons_box_layout);
        boxes.first = weapons_box;
    }
    if (has_champ_opt_weapons) {
        QGroupBox* weapons_box = new QGroupBox(creator->tr("Champion Weapons"));
        QVBoxLayout* weapons_box_layout = new QVBoxLayout;
        auto melee_subbox = make_weapons_subbox(WeaponType::MELEE, true);
        if (melee_subbox != nullptr) weapons_box_layout->addWidget(melee_subbox);
        auto ranged_subbox = make_weapons_subbox(WeaponType::BALLISTIC, true);
        if (ranged_subbox != nullptr) weapons_box_layout->addWidget(ranged_subbox);
        weapons_box_layout->addStretch(1);
        weapons_box->setLayout(weapons_box_layout);
        boxes.second = weapons_box;
    }
    return boxes;
}

QGroupBox* OptionBox::make_weapons_subbox(WeaponType wt, bool champion) {
    if (current->is_mixed()) {
        auto p = std::dynamic_pointer_cast<mixed_unit>(current);
        std::unordered_map<
            std::string,
            weapon_option
        > master_opt_weapons;
        std::unordered_map<
            WeaponType,
            std::tuple<ItemCategory, std::string, double>
        > master_curr_weapons;
        std::unordered_map<
            std::string,
            weapon_option
        > slave_opt_weapons;
        std::unordered_map<
            WeaponType,
            std::tuple<ItemCategory, std::string, double>
        > slave_curr_weapons;
        if (champion) {
            master_opt_weapons = p->handle->master_champion_opt().weapons();
            master_curr_weapons = p->master_champion_weapons();
            slave_opt_weapons = p->handle->slave_champion_opt().weapons();
            slave_curr_weapons = p->slave_champion_weapons();
        } else {
            master_opt_weapons = p->handle->master_opt().weapons();
            master_curr_weapons = p->master_weapons();
            slave_opt_weapons = p->handle->slave_opt().weapons();
            slave_curr_weapons = p->slave_weapons();
        }
        return nullptr;
        //QGroupBox* gb = new QGroupBox()
    }
    std::unordered_map<
        std::string,
        weapon_option
    > opt_weapons;
    std::unordered_map<
        WeaponType,
        std::tuple<ItemCategory, std::string, double>
    > curr_weapons;
    switch (current->base_unit_type()) {
    case BaseUnitType::MAGE_CHARACTER:
    case BaseUnitType::MELEE_CHARACTER:
    {
        auto p = std::dynamic_pointer_cast<character_unit>(current);
        opt_weapons = p->handle_->opt().weapons();
        curr_weapons = p->weapons();
        break;
    }
    case BaseUnitType::NORMAL:
    {
        auto p = std::dynamic_pointer_cast<normal_unit>(current);
        if (champion) {
            opt_weapons = p->handle->champion_opt().weapons();
            p->switch_model_select(ModelSelect::CHAMPION);
            curr_weapons = p->weapons();
            p->switch_model_select(ModelSelect::DEFAULT);
        }
        else {
            opt_weapons = p->handle->opt().weapons();
            p->switch_model_select(ModelSelect::DEFAULT);
            curr_weapons = p->weapons();
        }
        break;
    }
    default: return nullptr;
    }
    auto opt_weapon_vec = tools::find_all_if(
        std::begin(opt_weapons),
        std::end(opt_weapons),
        [wt] (const auto& x) { return x.second.type == wt; }
    );
    if (opt_weapon_vec.empty()) return nullptr;
    std::string subbox_label = "";
    switch (wt) {
    case WeaponType::MELEE:
        subbox_label = "Melee";
        break;
    case WeaponType::BALLISTIC:
        subbox_label = "Ranged";
        break;
    default: throw std::runtime_error("Unrecognised weapon type!");
    }
    QGroupBox* weapons_subbox = new QGroupBox(creator->tr(subbox_label.data()));
    QVBoxLayout* weapons_subbox_layout = new QVBoxLayout;
    bool has_weapon = false;
    for (const auto& w : opt_weapon_vec) {
        std::string pts_str = tools::points_str(w->second.points, current->base_unit_type());
        std::string button_label = w->first + " (" + pts_str + ")";
        QRadioButton* rb = new QRadioButton(creator->tr(button_label.data()));
        if (curr_weapons.count(w->second.type)) { // check if current unit weapon map contains this weapon
            if (std::get<1>(curr_weapons.at(w->second.type)) == w->first) {
                rb->setChecked(true);
                has_weapon = true;
            }
        }
        std::string weapon_name = w->first;
        ItemCategory ic = w->second.category;
        creator->connect(rb, &QRadioButton::clicked, creator.get(), [weapon_name, ic, wt, champion, this](auto) {
            creator->optional_weapon_selected(weapon_name, wt, ic, champion, true);
        });
        weapons_subbox_layout->addWidget(rb);
    }
    QRadioButton* none_rb = new QRadioButton(creator->tr("None"));
    if (!has_weapon) none_rb->setChecked(true);
    creator->connect(none_rb, &QRadioButton::clicked, creator.get(), [champion, wt, this](auto) {
        creator->optional_weapon_selected("", wt, ItemCategory::MUNDANE, champion, true);
    });
    weapons_subbox_layout->addWidget(none_rb);
    weapons_subbox_layout->addStretch(1);
    weapons_subbox->setLayout(weapons_subbox_layout);
    return weapons_subbox;
}

std::pair<QGroupBox*, QGroupBox*> OptionBox::make_armour_boxes() {
    bool has_opt_armour = false;
    bool has_champ_opt_armour = false;
    std::pair<QGroupBox*, QGroupBox*> boxes = {nullptr, nullptr};
    switch (current->base_unit_type()) {
    case BaseUnitType::MAGE_CHARACTER:
    case BaseUnitType::MELEE_CHARACTER:
    {
        auto p = std::dynamic_pointer_cast<character_unit>(current);
        if (p->handle_->opt().armour().empty()) return boxes;
        has_opt_armour = true;
        break;
    }
    case BaseUnitType::NORMAL:
    {
        auto p = std::dynamic_pointer_cast<normal_unit>(current);
        if (p->handle->opt().armour().empty() && p->handle->champion_opt().armour().empty())
            return boxes;
        if (!p->handle->opt().armour().empty()) has_opt_armour = true;
        if (!p->handle->champion_opt().armour().empty()) has_champ_opt_armour = true;
        break;
    }
    default: return boxes;
    }
    if (has_opt_armour) {
        QGroupBox* armour_box = new QGroupBox(creator->tr("Armour"));
        QVBoxLayout* armour_box_layout = new QVBoxLayout;
        auto body_subbox = make_armour_subbox(ArmourType::ARMOUR, false);
        if (body_subbox != nullptr) armour_box_layout->addWidget(body_subbox);
        auto shield_subbox = make_armour_subbox(ArmourType::SHIELD, false);
        if (shield_subbox != nullptr) armour_box_layout->addWidget(shield_subbox);
        auto helmet_subbox = make_armour_subbox(ArmourType::HELMET, false);
        if (helmet_subbox != nullptr) armour_box_layout->addWidget(helmet_subbox);
        armour_box_layout->addStretch(1);
        armour_box->setLayout(armour_box_layout);
        boxes.first = armour_box;
    }
    if (has_champ_opt_armour) {
        QGroupBox* armour_box = new QGroupBox(creator->tr("Champion Armour"));
        QVBoxLayout* armour_box_layout = new QVBoxLayout;
        auto body_subbox = make_armour_subbox(ArmourType::ARMOUR, true);
        if (body_subbox != nullptr) armour_box_layout->addWidget(body_subbox);
        auto shield_subbox = make_armour_subbox(ArmourType::SHIELD, true);
        if (shield_subbox != nullptr) armour_box_layout->addWidget(shield_subbox);
        auto helmet_subbox = make_armour_subbox(ArmourType::HELMET, true);
        if (helmet_subbox != nullptr) armour_box_layout->addWidget(helmet_subbox);
        armour_box_layout->addStretch(1);
        armour_box->setLayout(armour_box_layout);
        boxes.second = armour_box;
    }
    return boxes;
}

QGroupBox* OptionBox::make_armour_subbox(ArmourType at, bool champion) {
    std::unordered_map<
        std::string,
        armour_option
    > opt_armour;
    std::unordered_map<
        ArmourType,
        std::tuple<ItemCategory, std::string, double>
    > curr_armour;
    switch (current->base_unit_type()) {
    case BaseUnitType::MAGE_CHARACTER:
    case BaseUnitType::MELEE_CHARACTER:
    {
        auto p = std::dynamic_pointer_cast<character_unit>(current);
        opt_armour = p->handle_->opt().armour();
        curr_armour = p->armour();
        break;
    }
    case BaseUnitType::NORMAL:
    {
        auto p = std::dynamic_pointer_cast<normal_unit>(current);
        if (champion) {
            opt_armour = p->handle->champion_opt().armour();
            p->switch_model_select(ModelSelect::CHAMPION);
            curr_armour = p->armour();
            p->switch_model_select(ModelSelect::DEFAULT);
        }
        else {
            opt_armour = p->handle->opt().armour();
            p->switch_model_select(ModelSelect::DEFAULT);
            curr_armour = p->armour();
        }
        break;
    }
    default: return nullptr;
    }
    auto opt_armour_vec = tools::find_all_if(
        std::begin(opt_armour),
        std::end(opt_armour),
        [at] (const auto& x) { return x.second.type == at; }
    );
    if (opt_armour_vec.empty()) return nullptr;
    std::string subbox_label = "";
    switch (at) {
    case ArmourType::ARMOUR:
        subbox_label = "Body Armour";
        break;
    case ArmourType::SHIELD:
        subbox_label = "Shield";
        break;
    case ArmourType::HELMET:
        subbox_label = "Helmet";
        break;
    default: throw std::runtime_error("Unrecognised armour type!");
    }
    QGroupBox* armour_subbox = new QGroupBox(creator->tr(subbox_label.data()));
    QVBoxLayout* armour_subbox_layout = new QVBoxLayout;
    bool has_armour = false;
    for (const auto& a : opt_armour_vec) {
        BaseUnitType but = current->base_unit_type();
        std::string pts_str = tools::points_str(a->second.points, but);
        std::string button_label = a->first + " (" + pts_str + ")";
        QRadioButton* rb = new QRadioButton(creator->tr(button_label.data()));
        // check if current unit armour map contains this piece of armour
        if (curr_armour.count(a->second.type)) {
               if (std::get<1>(curr_armour.at(a->second.type)) == a->first) {
                   rb->setChecked(true);
                   has_armour = true;
               }
        }
        std::string armour_name = a->first;
        ItemCategory ic = a->second.category;
        creator->connect(rb, &QRadioButton::clicked, creator.get(), [armour_name, ic, at, champion, this](auto) {
            creator->optional_armour_selected(armour_name, at, ic, champion, true);
        });
        armour_subbox_layout->addWidget(rb);
    }
    QRadioButton* none_rb = new QRadioButton(creator->tr("None"));
    if (!has_armour) none_rb->setChecked(true);
    creator->connect(none_rb, &QRadioButton::clicked, creator.get(), [champion, at, this](auto) {
        creator->optional_armour_selected("", at, ItemCategory::MUNDANE, champion, true);
    });
    armour_subbox_layout->addWidget(none_rb);
    armour_subbox_layout->addStretch(1);
    armour_subbox->setLayout(armour_subbox_layout);
    return armour_subbox;
}

std::pair<QGroupBox*, QGroupBox*> OptionBox::make_mage_options_boxes() {
    auto p = std::dynamic_pointer_cast<mage_character_unit>(current);
    std::pair<QGroupBox*, QGroupBox*> boxes = {nullptr, nullptr};
    // mage levels box
    auto opt_levels = p->handle->level_upgrades();
    if (!opt_levels.empty()) {
        short level = p->level();
        auto v = tools::umap_to_vector(opt_levels);
        std::sort(std::begin(v), std::end(v), [](const auto& lhs, const auto& rhs) {
            return lhs.first < rhs.first;
        });
        QGroupBox* levels_box = new QGroupBox(creator->tr("Mage Levels"));
        QVBoxLayout* levels_box_layout = new QVBoxLayout;
        bool has_level = false;
        for (const auto& l : v) {
            std::string name = "Level " + std::to_string(l.first) + " (" + tools::points_str(l.second) + " pts" + ")";
            QRadioButton* b = new QRadioButton(creator->tr(name.data()));
            if (level == l.first) {
                b->setChecked(true);
                has_level = true;
            }
            creator->connect(b, &QRadioButton::clicked, creator.get(), [this, l](auto) {
                creator->optional_level_selected(l.first);
            });
            levels_box_layout->addWidget(b);
        }
        std::string none_rb_name = "Level " + std::to_string(p->handle->mage_level()) + " (0 pts)";
        QRadioButton* none_rb = new QRadioButton(creator->tr(none_rb_name.data()));
        creator->connect(none_rb, &QRadioButton::clicked, creator.get(), [this, p](auto) {
            creator->optional_level_selected(p->handle->mage_level());
        });
        levels_box_layout->addWidget(none_rb);
        if (!has_level) none_rb->setChecked(true);
        levels_box_layout->addStretch(1);
        levels_box->setLayout(levels_box_layout);
        boxes.first = levels_box;
    }
    // mage lores box
    auto opt_lores = p->handle->lores();
    if (!opt_lores.empty()) {
        auto lores = p->lores();
        QGroupBox* lores_box = new QGroupBox(creator->tr("Lores of Magic"));
        QVBoxLayout* lores_box_layout = new QVBoxLayout;
        bool has_lore = false;
        for (const auto& l : opt_lores) {
            std::string name = "Lore of " + l.name;
            QAbstractButton* b;
            if (p->handle->lore_count() > 1) b = new QCheckBox(creator->tr(name.data()));
            else b = new QRadioButton(creator->tr(name.data()));
            // lores can only be empty if unit is selected in roster_tree
            // so in this case select the lore if it is a default
            if (lores.empty() && l.is_default) {
                b->setChecked(true);
                has_lore = true;
            }
            else if (std::count_if(std::begin(lores), std::end(lores), [&l](const auto& x) { return x == l.name; } )) {
                b->setChecked(true);
                has_lore = true;
            }
            if (p->handle->lore_count() > 1)
                creator->connect(b, &QCheckBox::clicked, creator.get(), [this, l](bool checked) {
                   creator->optional_lore_selected(l.name, !checked ? l.name : "");
                });
            else
                creator->connect(b, &QRadioButton::clicked, creator.get(), [this, l, p](auto) {
                    std::string to_remove = p->lores().empty() ? "" :p->lores()[0];
                    if (to_remove == l.name) return;
                    creator->optional_lore_selected(l.name, to_remove);
                });
            lores_box_layout->addWidget(b);
        }
        lores_box_layout->addStretch(1);
        lores_box->setLayout(lores_box_layout);
        boxes.second = lores_box;
    }
    return boxes;
}

QGroupBox* OptionBox::make_mounts_boxes() {
    std::unordered_map<std::string, mount_option> opt_mounts;
    std::tuple<
        std::string,
        double,
        std::pair<std::string, double>,
        std::unordered_map<std::string, double>
    > mount_;
    switch (current->base_unit_type()) {
    case BaseUnitType::MAGE_CHARACTER:
    case BaseUnitType::MELEE_CHARACTER:
    {
        auto p = std::dynamic_pointer_cast<character_unit>(current);
        opt_mounts = p->handle_->opt().mounts();
        mount_ = p->mnt();
        break;
    }
    case BaseUnitType::NORMAL:
    {
        auto p = std::dynamic_pointer_cast<normal_unit>(current);
        opt_mounts = p->handle->opt().mounts();
        mount_ = p->mnt();
        break;
    }
    default: return nullptr;
    }
    if (opt_mounts.empty()) return nullptr;
    QGroupBox* mounts_box = new QGroupBox(creator->tr("Mounts"));
    QVBoxLayout* vbox_mounts = new QVBoxLayout;
    bool has_mount = false;
    for (const auto& m : opt_mounts) {
        auto tmp = tools::split(std::to_string(m.second.points), '.');
        for (auto& s : tmp) tools::remove_leading_whitespaces(s);
        std::string pts_str = (tools::starts_with(tmp[1], '0')) ? tmp[0] : tmp[0] + "." + tmp[1].substr(0, 1);
        std::string permodel = (current->base_unit_type() == BaseUnitType::NORMAL) ? "/model" : "";
        std::string name = m.first + " (" + pts_str + " pts" + permodel + ")";
        QRadioButton* b = new QRadioButton(creator->tr(name.data()));
        if (std::get<0>(mount_) == m.first) {
            b->setChecked(true);
            has_mount = true;
        }
        creator->connect(b, &QRadioButton::clicked, creator.get(), [this, m](auto) {
            creator->optional_mount_selected(m.first);
        });
        vbox_mounts->addWidget(b);
        // spawn another group box containing mount options
        if (current->base()->mounts_handle() == nullptr) continue;
        if (!(m.second.oco_extras.empty() && m.second.mc_extras.empty()) &&
                std::get<0>(mount_) == m.first) { // only show the options if the associated mount is selected
            QFrame* f = new QFrame();
            QHBoxLayout* frame_layout = new QHBoxLayout;
            QGroupBox* mount_oco_options_box = (m.second.oco_extras.empty()) ?
                        nullptr : new QGroupBox(creator->tr("Options (single)"));
            QGroupBox* mount_mc_options_box = (m.second.mc_extras.empty()) ?
                        nullptr : new QGroupBox(creator->tr("Options (multiple)"));
            if (mount_oco_options_box != nullptr) {
                QVBoxLayout* mntopt_layout = new QVBoxLayout;
                bool has_option = false;
                for (const auto& mo : m.second.oco_extras) {
                    std::string mo_name = mo.first + " (" + tools::points_str(mo.second.points) + " pts)";
                    QRadioButton* rb = new QRadioButton(creator->tr(mo_name.data()));
                    if (std::get<2>(mount_).first == mo.first) {
                        rb->setChecked(true);
                        has_option = true;
                    }
                    creator->connect(rb, &QRadioButton::clicked, creator.get(), [this, mo](auto) {
                        creator->optional_mount_oco_extra_selected(mo.first);
                    });
                    mntopt_layout->addWidget(rb);
                }
                QRadioButton* none_rb = new QRadioButton(creator->tr("None"));
                if (!has_option) none_rb->setChecked(true);
                creator->connect(none_rb, &QRadioButton::clicked, creator.get(), [this](auto) {
                    creator->optional_mount_oco_extra_selected("");
                });
                mntopt_layout->addWidget(none_rb);
                mntopt_layout->addStretch(1);
                mount_oco_options_box->setLayout(mntopt_layout);
                frame_layout->addWidget(mount_oco_options_box);
            }
            if (mount_mc_options_box != nullptr) {
                QVBoxLayout* mntopt_layout = new QVBoxLayout;
                for (const auto& mo : m.second.mc_extras) {
                    std::string mo_name = mo.first + " (" + tools::points_str(mo.second.points) + " pts)";
                    QCheckBox* cb = new QCheckBox(creator->tr(mo_name.data()));
                    if (std::get<3>(mount_).count(mo.first)) cb->setChecked(true);
                    creator->connect(cb, &QCheckBox::clicked, creator.get(), [this, mo](bool checked) {
                        creator->optional_mount_mc_extra_selected(mo.first, checked);
                    });
                    mntopt_layout->addWidget(cb);
                }
                mntopt_layout->addStretch(1);
                mount_mc_options_box->setLayout(mntopt_layout);
                frame_layout->addWidget(mount_mc_options_box);
            }
            f->setLayout(frame_layout);
            vbox_mounts->addWidget(f);
        }
    }
    QRadioButton* none_rb = new QRadioButton(creator->tr("None"));
    if (!has_mount) none_rb->setChecked(true);
    creator->connect(none_rb, &QRadioButton::clicked, creator.get(), [this](auto) {
        creator->optional_mount_selected("");
    });
    vbox_mounts->addWidget(none_rb);
    vbox_mounts->addStretch(1);
    mounts_box->setLayout(vbox_mounts);
    return mounts_box;
}

std::pair<QGroupBox*, QGroupBox*> OptionBox::make_extras_boxes() {
    bool has_opt_oco_extras = false;
    bool has_champ_opt_oco_extras = false;
    bool has_opt_mc_extras = false;
    bool has_champ_opt_mc_extras = false;
    std::pair<QGroupBox*, QGroupBox*> boxes = {nullptr, nullptr};
    switch (current->base_unit_type()) {
    case BaseUnitType::MAGE_CHARACTER:
    case BaseUnitType::MELEE_CHARACTER:
    {
        auto p = std::dynamic_pointer_cast<character_unit>(current);
        if (p->handle_->opt().oco_extras().empty() && p->handle_->opt().mc_extras().empty())
            return boxes;
        if (!p->handle_->opt().oco_extras().empty()) has_opt_oco_extras = true;
        if (!p->handle_->opt().mc_extras().empty()) has_opt_mc_extras = true;
        break;
    }
    case BaseUnitType::NORMAL:
    {
        auto p = std::dynamic_pointer_cast<normal_unit>(current);
        if (p->handle->opt().oco_extras().empty() && p->handle->opt().mc_extras().empty() &&
                p->handle->champion_opt().oco_extras().empty() && p->handle->champion_opt().mc_extras().empty())
            return boxes;
        if (!p->handle->opt().oco_extras().empty()) has_opt_oco_extras = true;
        if (!p->handle->opt().mc_extras().empty()) has_opt_mc_extras = true;
        if (!p->handle->champion_opt().oco_extras().empty()) has_champ_opt_oco_extras = true;
        if (!p->handle->champion_opt().mc_extras().empty()) has_champ_opt_mc_extras = true;
        break;
    }
    default: return boxes;
    }
    if (has_opt_oco_extras || has_opt_mc_extras) {
        QGroupBox* extras_box = new QGroupBox(creator->tr("Other"));
        QVBoxLayout* extras_box_layout = new QVBoxLayout;
        if (has_opt_oco_extras) {
            auto oco_subbox = make_oco_extras_subbox(false);
            extras_box_layout->addWidget(oco_subbox);
        }
        if (has_opt_mc_extras) {
            auto mc_subbox = make_mc_extras_subbox(false);
            extras_box_layout->addWidget(mc_subbox);
        }
        extras_box_layout->addStretch(1);
        extras_box->setLayout(extras_box_layout);
        boxes.first = extras_box;
    }
    if (has_champ_opt_oco_extras || has_champ_opt_mc_extras) {
        QGroupBox* extras_box = new QGroupBox(creator->tr("Champion Other"));
        QVBoxLayout* extras_box_layout = new QVBoxLayout;
        if (has_champ_opt_oco_extras) {
            auto oco_subbox = make_oco_extras_subbox(true);
            extras_box_layout->addWidget(oco_subbox);
        }
        if (has_champ_opt_mc_extras) {
            auto mc_subbox = make_mc_extras_subbox(true);
            extras_box_layout->addWidget(mc_subbox);
        }
        extras_box_layout->addStretch(1);
        extras_box->setLayout(extras_box_layout);
        boxes.second = extras_box;
    }
    return boxes;
}

QGroupBox* OptionBox::make_oco_extras_subbox(bool champion) {
    std::unordered_map<std::string, extra_option> opt_oco_extras;
    std::pair<std::string, std::pair<bool, double>> curr_oco_extra;
    switch (current->base_unit_type()) {
    case BaseUnitType::MAGE_CHARACTER:
    case BaseUnitType::MELEE_CHARACTER:
    {
        auto p = std::dynamic_pointer_cast<character_unit>(current);
        opt_oco_extras = p->handle_->opt().oco_extras();
        if (enum_convert::in_army_trees(in_tree)) curr_oco_extra = p->oco_extra();
        break;
    }
    case BaseUnitType::NORMAL:
    {
        auto p = std::dynamic_pointer_cast<normal_unit>(current);
        if (champion) {
            opt_oco_extras = p->handle->champion_opt().oco_extras();
            p->switch_model_select(ModelSelect::CHAMPION);
            if (enum_convert::in_army_trees(in_tree)) curr_oco_extra = p->oco_extra();
            p->switch_model_select(ModelSelect::DEFAULT);
        }
        else {
            opt_oco_extras = p->handle->opt().oco_extras();
            p->switch_model_select(ModelSelect::DEFAULT);
            if (enum_convert::in_army_trees(in_tree)) curr_oco_extra = p->oco_extra();
        }
        break;
    }
    default: return nullptr;
    }
    QGroupBox* oco_box = new QGroupBox();
    QVBoxLayout* oco_box_layout = new QVBoxLayout;
    bool has_extra = false;
    for (const auto& e : opt_oco_extras) {
        std::string pts_str = tools::points_str(e.second.points);
        std::string permodel = (current->base_unit_type() == BaseUnitType::NORMAL &&
                                    !(e.second.is_singular)) ? "/model" : "";
        std::string label = e.first + " (" + pts_str + " pts" + permodel + ")";
        QRadioButton* rb = new QRadioButton(creator->tr(label.data()));
        if (curr_oco_extra.first == e.first) {
            rb->setChecked(true);
            has_extra = true;
        }
        creator->connect(rb, &QRadioButton::clicked, creator.get(), [this, champion, e](auto) {
            creator->optional_oco_extra_selected(e.first, champion);
        });
        oco_box_layout->addWidget(rb);
    }
    QRadioButton* none_rb = new QRadioButton(creator->tr("None"));
    if (!has_extra) none_rb->setChecked(true);
    creator->connect(none_rb, &QRadioButton::clicked, creator.get(), [this, champion](auto) {
        creator->optional_oco_extra_selected("", champion);
    });
    oco_box_layout->addWidget(none_rb);
    oco_box_layout->addStretch(1);
    oco_box->setLayout(oco_box_layout);
    return oco_box;
}

QGroupBox* OptionBox::make_mc_extras_subbox(bool champion) {
    std::unordered_map<std::string, extra_option> opt_mc_extras;
    std::unordered_map<std::string, std::pair<bool, double>> curr_mc_extras;
    switch (current->base_unit_type()) {
    case BaseUnitType::MAGE_CHARACTER:
    case BaseUnitType::MELEE_CHARACTER:
    {
        auto p = std::dynamic_pointer_cast<character_unit>(current);
        opt_mc_extras = p->handle_->opt().mc_extras();
        if (enum_convert::in_army_trees(in_tree)) curr_mc_extras = p->mc_extras();
        break;
    }
    case BaseUnitType::NORMAL:
    {
        auto p = std::dynamic_pointer_cast<normal_unit>(current);
        if (champion) {
            opt_mc_extras = p->handle->champion_opt().mc_extras();
            p->switch_model_select(ModelSelect::CHAMPION);
            if (enum_convert::in_army_trees(in_tree)) curr_mc_extras = p->mc_extras();
            p->switch_model_select(ModelSelect::DEFAULT);
        }
        else {
            opt_mc_extras = p->handle->opt().mc_extras();
            p->switch_model_select(ModelSelect::DEFAULT);
            if (enum_convert::in_army_trees(in_tree)) curr_mc_extras = p->mc_extras();
        }
        break;
    }
    default: return nullptr;
    }
    QGroupBox* mc_box = new QGroupBox();
    QVBoxLayout* mc_box_layout = new QVBoxLayout;
    for (const auto& e : opt_mc_extras) {
        std::string pts_str = tools::points_str(e.second.points);
        std::string permodel = (current->base_unit_type() == BaseUnitType::NORMAL &&
                                    !(e.second.is_singular)) ? "/model" : "";
        std::string label = e.first + " (" + pts_str + " pts" + permodel + ")";
        QCheckBox* cb = new QCheckBox(creator->tr(label.data()));
        if (curr_mc_extras.count(e.first)) cb->setChecked(true);
        creator->connect(cb, &QCheckBox::clicked, creator.get(), [this, champion, e](bool checked) {
            creator->optional_mc_extra_selected(e.first, champion, checked);
        });
        mc_box_layout->addWidget(cb);
    }
    mc_box_layout->addStretch(1);
    mc_box->setLayout(mc_box_layout);
    return mc_box;
}
