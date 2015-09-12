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
#ifndef MATERIAL_H
#define MATERIAL_H

#include <QObject>
#include "utils.h"
#include "global_enums.h"
#include "flagarray.h"

class DFInstance;
class MemoryLayout;

class Material : public QObject {
    Q_OBJECT
public:
    Material(QObject *parent = 0);
    Material(DFInstance *df, VIRTADDR address, int index, bool inorganic = false, QObject *parent = 0);
    virtual ~Material();

    static Material* get_material(DFInstance *df, const VIRTADDR &address, int index, bool inorganic = false, QObject *parent = 0);

    //! Return the memory address (in hex) of this Material in the remote DF process
    VIRTADDR address() {return m_address;}

    QString get_material_name(MATERIAL_STATES state);
    int id() {return m_index;}

    void load_data();
    bool is_inorganic() {return m_inorganic;}
    bool is_generated() {return m_is_generated;}

    FlagArray flags() {return m_flags;}

    static const QString get_material_flag_desc(const MATERIAL_FLAGS &flag) {
        QMap<MATERIAL_FLAGS, QString> m;
        m[BONE]=trUtf8("Bone");
        m[TOOTH]=trUtf8("Ivory/Tooth");
        m[HORN]=trUtf8("Horn/Hoof");
        m[PEARL]=trUtf8("Pearl");
        m[SHELL]=trUtf8("Shell");
        m[LEATHER]=trUtf8("Leather");
        m[SILK]=trUtf8("Silk");
        m[IS_GEM]=trUtf8("Gems");
        m[IS_GLASS]=trUtf8("Glass");
        m[IS_WOOD]=trUtf8("Wood");
        m[IS_STONE]=trUtf8("Stone");
        m[IS_METAL]=trUtf8("Metal");
        m[THREAD_PLANT]=trUtf8("Cloth");
        m[YARN]=trUtf8("Yarn/Wool/Fur");
        return m.value(flag, "Missing Description");
    }

    static const QString get_mat_class_desc(const int mat_class){
        QMap<int, QString> m;
        m[MC_LEATHER]=trUtf8("Leather");
        m[MC_CLOTH]=trUtf8("Cloth");
        m[MC_WOOD]=trUtf8("Wooden");
        m[MC_STONE]=trUtf8("Stone");
        m[MC_METAL_AMMO]=trUtf8("Metal");
        m[MC_METAL_AMMO2]=trUtf8("Metal");
        m[MC_METAL_ARMOR]=trUtf8("Metal");
        m[MC_GEM]=trUtf8("Gem");
        m[MC_BONE]=trUtf8("Bone");
        m[MC_SHELL]=trUtf8("Shell");
        m[MC_PEARL]=trUtf8("Pearl");
        m[MC_TOOTH]=trUtf8("Ivory/Tooth");
        m[MC_HORN]=trUtf8("Horn/Hoof");
        m[MC_PLANT_FIBER]=trUtf8("Plant Fiber");
        m[MC_SILK]=trUtf8("Silk");
        m[MC_YARN]=trUtf8("Yarn/Wool/Fur");
        return m.value(mat_class, "???");
    }

private:
    int m_index;
    VIRTADDR m_address;
    VIRTADDR m_flag_address;
    DFInstance * m_df;
    MemoryLayout * m_mem;
    FlagArray m_flags;
    bool m_inorganic;
    bool m_is_generated;
    QHash<MATERIAL_STATES, QString> m_state_names;

    void read_material();
};

#endif // MATERIAL_H
