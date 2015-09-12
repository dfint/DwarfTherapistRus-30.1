/*
Dwarf Therapist
Copyright (c) 2009 Trey Stout (chmod)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#ifndef ITEM_H
#define ITEM_H

#include "truncatingfilelogger.h"
#include "global_enums.h"
#include "material.h"
#include "itemdefuniform.h"
#include <QObject>
#include <QColor>

class QString;

class Item : public QObject {
    Q_OBJECT
public:
    Item(const Item &i);
    Item(DFInstance *df, VIRTADDR item_addr, QObject *parent = 0);
    //uniform item ctor
    Item(DFInstance *df, ItemDefUniform *u, QObject *parent = 0);
    //placeholder item ctor
    Item(ITEM_TYPE itype,QString name, QObject *parent = 0);
    virtual ~Item();

    static const int MAX_AFFECTION = 10000;

    static const QString get_item_name_plural(const ITEM_TYPE &type) {
        QMap<ITEM_TYPE, QString> m;
        m[NONE]=trUtf8("N/A");
        m[BAR]=trUtf8("Metals/Fuels/Soaps");
        m[SMALLGEM]=trUtf8("Cut gemstones");
        m[BLOCKS]=trUtf8("Blocks");
        m[ROUGH]=trUtf8("Rough Gemstones");
        m[BOULDER]=trUtf8("Boulders");
        m[WOOD]=trUtf8("Woods");
        m[DOOR]=trUtf8("Doors");
        m[FLOODGATE]=trUtf8("Floodgates");
        m[BED]=trUtf8("Beds");
        m[CHAIR]=trUtf8("Chairs/Thrones");
        m[CHAIN]=trUtf8("Chains");
        m[FLASK]=trUtf8("Flasks");
        m[GOBLET]=trUtf8("Goblets");
        m[INSTRUMENT]=trUtf8("Instruments");
        m[TOY]=trUtf8("Toys");
        m[WINDOW]=trUtf8("Windows");
        m[CAGE]=trUtf8("Cages");
        m[BARREL]=trUtf8("Barrels");
        m[BUCKET]=trUtf8("Buckets");
        m[ANIMALTRAP]=trUtf8("Animal Traps");
        m[TABLE]=trUtf8("Tables");
        m[COFFIN]=trUtf8("Coffins");
        m[STATUE]=trUtf8("Statues");
        m[CORPSE]=trUtf8("Corpses");
        m[WEAPON]=trUtf8("Weapons");
        m[ARMOR]=trUtf8("Armors (Chest)");
        m[SHOES]=trUtf8("Armors (Feet)");
        m[SHIELD]=trUtf8("Armors (Shields)");
        m[HELM]=trUtf8("Armors (Head)");
        m[GLOVES]=trUtf8("Armors (Hands)");
        m[BOX]=trUtf8("Boxes/Bags");
        m[BIN]=trUtf8("Bins");
        m[ARMORSTAND]=trUtf8("Armor Stands");
        m[WEAPONRACK]=trUtf8("Weapon Racks");
        m[CABINET]=trUtf8("Cabinets");
        m[FIGURINE]=trUtf8("Figurines");
        m[AMULET]=trUtf8("Amulets");
        m[SCEPTER]=trUtf8("Scepters");
        m[AMMO]=trUtf8("Ammunitions");
        m[CROWN]=trUtf8("Crowns");
        m[RING]=trUtf8("Rings");
        m[EARRING]=trUtf8("Earrings");
        m[BRACELET]=trUtf8("Bracelets");
        m[GEM]=trUtf8("Large Gems");
        m[ANVIL]=trUtf8("Anvils");
        m[CORPSEPIECE]=trUtf8("Corpse Bodyparts");
        m[REMAINS]=trUtf8("Remains");
        m[MEAT]=trUtf8("Meats");
        m[FISH]=trUtf8("Fishes");
        m[FISH_RAW]=trUtf8("Raw Fishes");
        m[VERMIN]=trUtf8("Vermins");
        m[IS_PET]=trUtf8("Pets");
        m[SEEDS]=trUtf8("Seeds");
        m[PLANT]=trUtf8("Plants");
        m[SKIN_TANNED]=trUtf8("Tanned Hides");
        m[LEAVES_FRUIT]=trUtf8("Leaves");
        m[THREAD]=trUtf8("Threads");
        m[CLOTH]=trUtf8("Cloths");
        m[TOTEM]=trUtf8("Totems");
        m[PANTS]=trUtf8("Armors (Legs)");
        m[BACKPACK]=trUtf8("Backpacks");
        m[QUIVER]=trUtf8("Quivers");
        m[CATAPULTPARTS]=trUtf8("Catapult Parts");
        m[BALLISTAPARTS]=trUtf8("Ballista Parts");
        m[SIEGEAMMO]=trUtf8("Siege Ammunitions");
        m[BALLISTAARROWHEAD]=trUtf8("Ballista Ammunitions");
        m[TRAPPARTS]=trUtf8("Mechanisms");
        m[TRAPCOMP]=trUtf8("Trap Components");
        m[DRINK]=trUtf8("Alcohols");
        m[POWDER_MISC]=trUtf8("Flours/Sugars/Powders");
        m[CHEESE]=trUtf8("Cheeses");
        m[FOOD]=trUtf8("Prepared Foods");
        m[LIQUID_MISC]=trUtf8("Honeys/Syrups/Milks/Oils");
        m[COIN]=trUtf8("Coins");
        m[GLOB]=trUtf8("Fats");
        m[ROCK]=trUtf8("Rocks");
        m[PIPE_SECTION]=trUtf8("Pipes");
        m[HATCH_COVER]=trUtf8("Hatch Covers");
        m[GRATE]=trUtf8("Grates");
        m[QUERN]=trUtf8("Querns");
        m[MILLSTONE]=trUtf8("Millstones");
        m[SPLINT]=trUtf8("Splints");
        m[CRUTCH]=trUtf8("Crutches");
        m[TRACTION_BENCH]=trUtf8("Traction Benches");
        m[ORTHOPEDIC_CAST]=trUtf8("Casts");
        m[TOOL]=trUtf8("Tools");
        m[SLAB]=trUtf8("Slabs");
        m[EGG]=trUtf8("Eggs");
        m[BOOK]=trUtf8("Books");
        m[SUPPLIES]=trUtf8("Supplies");
        m[MELEE_EQUIPMENT]=trUtf8("Weapon & Shield");
        m[RANGED_EQUIPMENT]=trUtf8("Quiver & Ammo");
        return m.value(type, "N/A");
    }

    static const QString get_item_clothing_names(const ITEM_TYPE &type){
        QMap<ITEM_TYPE,QString> m;
        m[ARMOR]=trUtf8("Clothing (Chest)");
        m[SHOES]=trUtf8("Clothing (Feet)");
        m[HELM]=trUtf8("Clothing (Head)");
        m[GLOVES]=trUtf8("Clothing (Hands)");
        m[PANTS]=trUtf8("Clothing (Legs)");
        return m.value(type,"N/A");
    }

    static const QString get_item_name(const ITEM_TYPE &type) {
        QMap<ITEM_TYPE, QString> m;
        m[NONE]=trUtf8("N/A");
        m[BAR]=trUtf8("Bar");
        m[SMALLGEM]=trUtf8("Cut Gemstone");
        m[BLOCKS]=trUtf8("Block");
        m[ROUGH]=trUtf8("Rough Gemstone");
        m[BOULDER]=trUtf8("Boulder");
        m[WOOD]=trUtf8("Wood");
        m[DOOR]=trUtf8("Door");
        m[FLOODGATE]=trUtf8("Floodgate");
        m[BED]=trUtf8("Bed");
        m[CHAIR]=trUtf8("Chair");
        m[CHAIN]=trUtf8("Chain");
        m[FLASK]=trUtf8("Flask");
        m[GOBLET]=trUtf8("Goblet");
        m[INSTRUMENT]=trUtf8("Instrument");
        m[TOY]=trUtf8("Toy");
        m[WINDOW]=trUtf8("Window");
        m[CAGE]=trUtf8("Cage");
        m[BARREL]=trUtf8("Barrel");
        m[BUCKET]=trUtf8("Bucket");
        m[ANIMALTRAP]=trUtf8("Animal Trap");
        m[TABLE]=trUtf8("Table");
        m[COFFIN]=trUtf8("Coffin");
        m[STATUE]=trUtf8("Statue");
        m[CORPSE]=trUtf8("Corpse");
        m[WEAPON]=trUtf8("Weapon");
        m[ARMOR]=trUtf8("Armor");
        m[SHOES]=trUtf8("Boot");
        m[SHIELD]=trUtf8("Shield");
        m[HELM]=trUtf8("Helm");
        m[GLOVES]=trUtf8("Gauntlet");
        m[BOX]=trUtf8("Box");
        m[BIN]=trUtf8("Bin");
        m[ARMORSTAND]=trUtf8("Armor Stand");
        m[WEAPONRACK]=trUtf8("Weapon Rack");
        m[CABINET]=trUtf8("Cabinet");
        m[FIGURINE]=trUtf8("Figurine");
        m[AMULET]=trUtf8("Amulet");
        m[SCEPTER]=trUtf8("Scepter");
        m[AMMO]=trUtf8("Ammunition");
        m[CROWN]=trUtf8("Crown");
        m[RING]=trUtf8("Ring");
        m[EARRING]=trUtf8("Earring");
        m[BRACELET]=trUtf8("Bracelet");
        m[GEM]=trUtf8("Large Gem");
        m[ANVIL]=trUtf8("Anvil");
        m[CORPSEPIECE]=trUtf8("Corpse Bodypart");
        m[REMAINS]=trUtf8("Remain");
        m[MEAT]=trUtf8("Meat");
        m[FISH]=trUtf8("Fish");
        m[FISH_RAW]=trUtf8("Raw Fish");
        m[VERMIN]=trUtf8("Vermin");
        m[IS_PET]=trUtf8("Pet");
        m[SEEDS]=trUtf8("Seed");
        m[PLANT]=trUtf8("Plant");
        m[SKIN_TANNED]=trUtf8("Tanned Hide");
        m[LEAVES_FRUIT]=trUtf8("Leaf");
        m[THREAD]=trUtf8("Thread");
        m[CLOTH]=trUtf8("Cloth");
        m[TOTEM]=trUtf8("Totem");
        m[PANTS]=trUtf8("Greave");
        m[BACKPACK]=trUtf8("Backpack");
        m[QUIVER]=trUtf8("Quiver");
        m[CATAPULTPARTS]=trUtf8("Catapult Part");
        m[BALLISTAPARTS]=trUtf8("Ballista Part");
        m[SIEGEAMMO]=trUtf8("Siege Ammunition");
        m[BALLISTAARROWHEAD]=trUtf8("Ballista Ammunition");
        m[TRAPPARTS]=trUtf8("Mechanism");
        m[TRAPCOMP]=trUtf8("Trap Component");
        m[DRINK]=trUtf8("Alcohol");
        m[POWDER_MISC]=trUtf8("Powder");
        m[CHEESE]=trUtf8("Cheese");
        m[FOOD]=trUtf8("Prepared Food");
        m[LIQUID_MISC]=trUtf8("Syrup/Milk/Oil");
        m[COIN]=trUtf8("Coin");
        m[GLOB]=trUtf8("Fat");
        m[ROCK]=trUtf8("Rock");
        m[PIPE_SECTION]=trUtf8("Pipe");
        m[HATCH_COVER]=trUtf8("Hatch Cover");
        m[GRATE]=trUtf8("Grate");
        m[QUERN]=trUtf8("Quern");
        m[MILLSTONE]=trUtf8("Millstone");
        m[SPLINT]=trUtf8("Splint");
        m[CRUTCH]=trUtf8("Crutch");
        m[TRACTION_BENCH]=trUtf8("Traction Bench");
        m[ORTHOPEDIC_CAST]=trUtf8("Cast");
        m[TOOL]=trUtf8("Tool");
        m[SLAB]=trUtf8("Slab");
        m[EGG]=trUtf8("Egg");
        m[BOOK]=trUtf8("Book");
        m[SUPPLIES]=trUtf8("Other Equipment");
        return m.value(type, "N/A");
    }

    static const QList<ITEM_TYPE> items_with_subtypes(){return m_items_subtypes;}

    static bool has_subtypes(const ITEM_TYPE &i_type){
        return m_items_subtypes.contains(i_type);
    }

    static bool is_trade_good(const ITEM_TYPE &i_type){
        if(i_type == BRACELET || i_type == RING || i_type == SCEPTER|| i_type == CROWN ||
                i_type == FIGURINE || i_type == AMULET || i_type == EARRING){
            return true;
        }else{
            return false;
        }
    }

    static bool is_armor_type(const ITEM_TYPE &i_type, const bool &include_shield = false){
        if(i_type == ARMOR || i_type == GLOVES || i_type == HELM || i_type == PANTS || i_type == SHOES ||
                (include_shield && i_type == SHIELD)){
            return true;
        }else{
            return false;
        }
    }
    static bool is_supplies(const ITEM_TYPE &i_type){
        if(i_type == BACKPACK || i_type == CRUTCH || i_type == FLASK)
            return true;
        else
            return false;
    }
    static bool is_melee_equipment(const ITEM_TYPE &i_type){
        if(i_type == SHIELD || i_type == WEAPON)
            return true;
        else
            return false;
    }
    static bool is_ranged_equipment(const ITEM_TYPE &i_type){
        if(i_type == QUIVER || i_type == AMMO)
            return true;
        else
            return false;
    }

    static bool type_in_group(ITEM_TYPE group_type, ITEM_TYPE item){
        if(group_type == MELEE_EQUIPMENT && is_melee_equipment(item))
            return true;
        else if(group_type == SUPPLIES && is_supplies(item))
            return true;
        else if(group_type == RANGED_EQUIPMENT && is_ranged_equipment(item))
            return true;
        else
            return false;
    }

    //the category name for body parts not covered by clothing
    static const QString uncovered_group_name(){return trUtf8("Uncovered");}
    //the category for missing uniform items
    static const QString missing_group_name() {return trUtf8("Missing");}

    static const QColor color_clothed() {return QColor(69,148,21);}
    static const QColor color_missing() {return QColor(57,130,227);}
    static const QColor color_wear() {return QColor(240,116,0,255);}
    static const QColor color_wear(int level){
            int alpha = 255;
            if(level == 2)
                alpha = 190;
            if(level == 1)
                alpha = 135;
            if(level <= 0)
                alpha = 0;
            QColor wear_color = color_wear();
            wear_color.setAlpha(alpha);
            return wear_color;
    }

    static const QColor color_uncovered() {return QColor(227,22,18);}

    VIRTADDR address() {return m_addr;}

    int id(){return m_id;}

    int wear(){return m_wear;}
    int quality(){return m_quality;}

    ITEM_TYPE item_type(){return m_iType;}
    void item_type(ITEM_TYPE newType){m_iType = newType;}

    int mat_index(){return m_mat_idx;}
    short mat_type(){return m_mat_type;}

    virtual short item_subtype() const {return -1;}
    virtual short melee_skill(){return -1;}
    virtual short ranged_skill(){return -1;}

    QString display_name(bool colored = false);
    bool equals(const Item &) const;

    void set_affection(int level);

    QList<Item*> contained_items() {return m_contained_items;}
    int get_stack_size(){return m_stack_size;}
    void add_to_stack(int num){
        m_stack_size+=num;
        stack_size_changed();
    }
    QString get_material_name(){return m_material_name;}
    QString get_material_name_base(){return m_material_name_base;}
    short get_quality(){return m_quality;}
    QString get_item_name(){return m_item_name;}

protected:
    DFInstance *m_df;
    VIRTADDR m_addr;
    ITEM_TYPE m_iType;
    short m_wear;
    short m_mat_type;
    int m_mat_idx;
    short m_quality;
    QString m_material_name;
    QString m_material_name_base;
    QString m_item_name;
    QString m_layer_name;
    QString m_display_name;
    QColor m_color_display;
    int m_id;
    int m_affection;
    int m_stack_size;
    QString m_artifact_name;
    QList<Item*> m_contained_items;

    void read_data();
    void set_default_name(Material *m);
    void build_display_name();
    QString get_quality_symbol();

    virtual void stack_size_changed(){}

private:
    static const QList<ITEM_TYPE> m_items_subtypes;
    static const QList<ITEM_TYPE> init_subtypes();

};
#endif // ITEM_H
