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

#include "traitcolumn.h"
#include "columntypes.h"
#include "dwarfmodel.h"
#include "dwarf.h"
#include "trait.h"
#include "gamedatareader.h"

TraitColumn::TraitColumn(const QString &title, const short &trait_id, ViewColumnSet *set, QObject *parent)
    : ViewColumn(title, CT_TRAIT, set, parent)
    , m_trait_id(trait_id)
    , m_trait(0)
{
    m_trait = GameDataReader::ptr()->get_trait(trait_id);
}

TraitColumn::TraitColumn(QSettings &s, ViewColumnSet *set, QObject *parent)
    : ViewColumn(s, set, parent)
    , m_trait_id(s.value("trait_id", -1).toInt())
    , m_trait(0)
{
    m_trait = GameDataReader::ptr()->get_trait(m_trait_id);
}

TraitColumn::TraitColumn(const TraitColumn &to_copy)
    : ViewColumn(to_copy)
    , m_trait_id(to_copy.m_trait_id)
    , m_trait(to_copy.m_trait)
{
}

QStandardItem *TraitColumn::build_cell(Dwarf *d) {
    QStandardItem *item = init_cell(d);
    item->setData(CT_TRAIT, DwarfModel::DR_COL_TYPE);
    item->setData(0, DwarfModel::DR_SPECIAL_FLAG); //default, special flag stores the alpha for the border

    short raw_value = d->trait(m_trait_id);
    short rating = raw_value;
    QStringList infos;
    if (m_trait){
        infos << m_trait->level_message(raw_value).append(m_trait->belief_conficts_msgs(raw_value,d->trait_conflicts(m_trait_id)));
    }else{
        infos << trUtf8("Unknown trait");
    }

    if(raw_value < 0){
        infos << trUtf8("This unit doesn't have this trait!");
    }

    if (d->trait_is_active(m_trait_id)==false)
        infos << trUtf8("Not an active trait for this dwarf.");

    int conflicting_belief_count = 0;
    if (m_trait){
        infos << m_trait->skill_conflicts_msgs(raw_value);
        infos << m_trait->special_messages(raw_value);

        conflicting_belief_count = m_trait->get_conflicting_beliefs().count();
        if(conflicting_belief_count > 0){
            infos << trUtf8("<br/>This trait can conflict with %1").arg(m_trait->belief_conflicts_names());
        }
        if(m_trait->valued_inversely()){
            infos << Trait::inverted_message;
            if(raw_value != -1)
                rating = 100 - raw_value;
        }
    }

    infos.removeAll("");

    if(d->trait_is_conflicted(m_trait_id) && conflicting_belief_count > 0){
        int alpha = 255 * ((float)d->trait_conflicts(m_trait_id).count() / (float)conflicting_belief_count);
        item->setData(alpha, DwarfModel::DR_SPECIAL_FLAG);
    }

    item->setText(QString::number(raw_value));
    item->setData(rating, DwarfModel::DR_SORT_VALUE);
    item->setData(rating, DwarfModel::DR_RATING);
    item->setData(rating, DwarfModel::DR_DISPLAY_RATING);
    item->setData(raw_value, DwarfModel::DR_EXPORT);
    set_export_role(DwarfModel::DR_EXPORT);

    QString tooltip = QString("<center><h3>%1</h3> %2</center><br/>%3<br/>%4")
            .arg(m_title)
            .arg(trUtf8("<b>Raw Value: %1</b>").arg(raw_value))
            .arg(infos.join("<br/>"))
            .arg(tooltip_name_footer(d));
    item->setToolTip(tooltip);

    return item;
}

QStandardItem *TraitColumn::build_aggregate(const QString &group_name, const QVector<Dwarf*> &dwarves) {
    Q_UNUSED(dwarves);
    QStandardItem *item = init_aggregate(group_name);
    return item;
}
