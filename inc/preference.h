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
#ifndef PREFERENCE_H
#define PREFERENCE_H

#include <QObject>
#include "utils.h"
#include "global_enums.h"
#include "flagarray.h"

class RoleAspect;
class ItemSubtype;
class Plant;
class Race;

class Preference : public QObject {
    Q_OBJECT
public:

    static const QString get_pref_desc(const PREF_TYPES &type) {
        QMap<PREF_TYPES, QString> desc;
        desc[LIKE_MATERIAL] = trUtf8("Materials");
        desc[LIKE_CREATURE] = trUtf8("Creatures");
        desc[LIKE_FOOD] = trUtf8("Food & Drink");
        desc[HATE_CREATURE] = trUtf8("Dislikes");
        desc[LIKE_ITEM] = trUtf8("Items");
        desc[LIKE_PLANT] = trUtf8("Plants");
        desc[LIKE_TREE] = trUtf8("Trees");
        desc[LIKE_COLOR] = trUtf8("Colors");
        desc[LIKE_SHAPE] = trUtf8("Shapes");
        desc[LIKE_OUTDOORS] = trUtf8("Outdoors");
        return desc.value(type, trUtf8("N/A"));
    }

    Preference(QObject *parent);
    Preference(PREF_TYPES category, QString name, QObject *parent = 0);
    Preference(PREF_TYPES category, ITEM_TYPE iType, QObject *parent = 0);
    Preference(const Preference &p);

    int matches(Preference *role_pref, Dwarf *d = 0);

    void add_flag(int);
    void set_name(QString value) {m_name = value;}
    void set_category(PREF_TYPES cat) {m_pType = cat;}
    void set_item_type(ITEM_TYPE iType) {m_iType = iType;}
    void set_exact(bool m) {m_exact_match = m;}

    QString get_name() {return m_name;}
    PREF_TYPES get_pref_category() {return m_pType;}
    ITEM_TYPE get_item_type() {return m_iType;}
    FlagArray &flags() {return m_flags;}
    bool exact_match() {return m_exact_match;}

    RoleAspect *pref_aspect;

    //this is a general setter for a unit's preference's flags
    void set_pref_flags(const FlagArray &flags);

    //these setters to load specific flags for the role
    void set_pref_flags(Race *r);
    void set_pref_flags(Plant *p);
    void set_pref_flags(ItemSubtype *i);

protected:
    QString m_name; //actual value to search for when doing string comparisons
    PREF_TYPES m_pType; //preference category
    ITEM_TYPE m_iType; //type of item for an dwarf's item preference

    //these flags are used when writing to the ini. they're specifically chosen to find matches of particular materials
    FlagArray m_flags;

    //if it's not a general category preference (ie. 'wood') an exact match of the string is required
    bool m_exact_match;

    bool set_flag(FlagArray origin, const int flag);
    void set_flags(FlagArray origin, const QList<int> flags);
};

#endif // PREFERENCE_H
