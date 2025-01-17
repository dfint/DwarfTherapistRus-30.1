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

#include "weaponcolumn.h"
#include "viewcolumnset.h"
#include "columntypes.h"
#include "dwarfmodel.h"
#include "dwarf.h"
#include "dwarftherapist.h"
#include "itemweaponsubtype.h"
#include "gamedatareader.h"
#include "dfinstance.h"
#include <QPalette>

WeaponColumn::WeaponColumn(QSettings &s, ViewColumnSet *set, QObject *parent)
    : ViewColumn(s, set, parent)
    , m_weapon(0x0)
    , m_sub_type_id(s.value("sub_type_id",-1).toInt())
    , m_weapon_name(s.value("weapon_name","").toString())
{
    init();
    connect(DT,SIGNAL(units_refreshed()),this,SLOT(check_weapon()));
}

WeaponColumn::WeaponColumn(const QString &title, const int sub_type, ViewColumnSet *set, QObject *parent)
    : ViewColumn(title, CT_WEAPON, set, parent)
    , m_weapon(0x0)
    , m_sub_type_id(sub_type)
    , m_weapon_name(title)
{
    init();
    connect(DT,SIGNAL(units_refreshed()),this,SLOT(check_weapon()));
}

WeaponColumn::~WeaponColumn(){
    m_weapon = 0;
}

void WeaponColumn::init(){
    if(m_weapon_name.trimmed().isEmpty())
        m_weapon_name = m_title;

    if(m_sub_type_id >= 0){
        m_weapon = qobject_cast<ItemWeaponSubtype*>(DT->get_DFInstance()->get_item_subtype(WEAPON,m_sub_type_id));
    }
    if(m_weapon.isNull() || QString::compare(m_weapon_name, m_weapon->name_plural(),Qt::CaseInsensitive) != 0){
        m_weapon = DT->get_DFInstance()->find_weapon_subtype(m_weapon_name);
        if(m_weapon)
            m_sub_type_id = m_weapon->subType();
    }
}

QStandardItem *WeaponColumn::build_cell(Dwarf *d) {
    QStandardItem *item = init_cell(d);

    check_weapon();

    item->setData(CT_WEAPON, DwarfModel::DR_COL_TYPE);
    item->setData(0, DwarfModel::DR_RATING);
    item->setData(0, DwarfModel::DR_DISPLAY_RATING);

    if(!m_weapon || m_weapon->name_plural()==""){
        item->setToolTip("Weapon not found.");
        return item;
    }
    if(d->body_size() < 0){
        item->setToolTip(trUtf8("Missing body_size offset!"));
        return item;
    }

    QString wep = m_weapon->name_plural().toLower();
    if(wep.indexOf(",")>0)
        wep = trUtf8("these weapons");
    else
        wep = capitalizeEach(wep);
    float rating = 40; //small red square by default
    QString numeric_rating = "/";
    short sort_val = 1;

    //caste size determines can/can't wield
    //if can wield, then the individual's size determines 1h/2h

    //use the default size, as DF doesn't take into account a creature's actual size when checking if they can use weapons
    int caste_size = d->body_size(true);
    bool onehand = false;
    bool twohand = false;
    QString desc = trUtf8("<b>Can only wield</b> %1 with <u>2 hands</u>.").arg(wep);
    if(caste_size > m_weapon->single_grasp())
        onehand = true;
    if(caste_size > m_weapon->multi_grasp())
        twohand = true;

    //setup drawing ratings
    if(!onehand && !twohand){
        desc = trUtf8("<b>Cannot wield</b> %1.").arg(wep);
        rating = 15; //this will give us a medium-large red square as the further from the median the larger the square gets
        numeric_rating = "X";
        sort_val = 0;
    }
    else if (twohand && onehand){
        desc = trUtf8("<b>Can wield</b> %1 with one or two hands.").arg(wep);
        rating = 50; //49-51 are not drawn, so any value in there to draw nothing
        numeric_rating = "";
        sort_val = 2;
    }

    QStringList weapon_skills;
    if(m_weapon->melee_skill() >= 0){
        weapon_skills.append(trUtf8("<b>Melee Skill:</b> %1").arg(GameDataReader::ptr()->get_skill_name(m_weapon->melee_skill())));
    }
    if(m_weapon->ranged_skill() >= 0){
        weapon_skills.append(trUtf8("<b>Ranged Skill:</b> %1").arg(GameDataReader::ptr()->get_skill_name(m_weapon->ranged_skill())));
    }

    QString tt_title = m_title;
    if(tt_title.indexOf(",")>0){
        //add a weapon list to the description
        QStringList l = m_title.split(",",QString::SkipEmptyParts);
        desc += "<br><br><b>Weapons:</b><ul>";
        for(int i = 0; i<l.length(); i++){
            desc.append(QString("<li>%1</li>").arg(l.at(i)));
        }
        desc.append("</ul>");
    }else
        desc.append("<br><br>");

    item->setData(CT_WEAPON, DwarfModel::DR_COL_TYPE);
    item->setData(rating, DwarfModel::DR_RATING);
    item->setData(numeric_rating, DwarfModel::DR_DISPLAY_RATING);
    item->setData((sort_val * 100) + d->body_size(), DwarfModel::DR_SORT_VALUE);
    set_export_role(DwarfModel::DR_DISPLAY_RATING);

    QColor norm_text = QApplication::palette().toolTipText().color();

    QString tooltip = QString("<center><h3>%1</h3></center>%2%3%4%5<center><h4>%6 - %7</h4></center>")
            .arg(tt_title)
            .arg(desc)
            .arg(weapon_skills.join("<br>").append("<br/><br/>"))
            .arg(trUtf8("1h: <font color=%1>%2</font> cm<sup>3</sup><br>")
                 .arg(onehand ? norm_text.name() : QColor(Qt::red).name())
                 .arg(m_weapon->single_grasp()*10))
            .arg(trUtf8("2h: <font color=%1>%2</font> cm<sup>3</sup>")
                 .arg(twohand ? norm_text.name() : QColor(Qt::red).name())
                 .arg(m_weapon->multi_grasp()*10))
            .arg(d->nice_name())
            .arg(trUtf8("%1 cm<sup>3</sup>").arg(d->body_size() * 10)); //however in the tooltip, show the actual size

    item->setToolTip(tooltip);

    return item;
}

QStandardItem *WeaponColumn::build_aggregate(const QString &group_name, const QVector<Dwarf*> &dwarves) {
    Q_UNUSED(dwarves);
    QStandardItem *item = init_aggregate(group_name);
    return item;
}

void WeaponColumn::check_weapon(){
    if(m_weapon.isNull()){
        init();
    }
}

void WeaponColumn::write_to_ini(QSettings &s){
    ViewColumn::write_to_ini(s);
    s.setValue("sub_type_id", m_sub_type_id);
    if(!m_weapon.isNull()){
        s.setValue("weapon_name", m_weapon->name_plural());
    }else if(!m_weapon_name.isEmpty()){
        s.setValue("weapon_name", m_weapon_name);
    }else{
        LOGW << "weapon name could not be found for weapon column:" << m_title;
    }
}
