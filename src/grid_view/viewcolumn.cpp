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
#include "viewcolumnset.h"
#include "viewcolumn.h"
#include "dwarf.h"
#include "dwarftherapist.h"
#include "dtstandarditem.h"
#include "viewcolumncolors.h"

ViewColumn::ViewColumn(QString title, COLUMN_TYPE type, ViewColumnSet *set,QObject *parent, int col_idx)
    : QObject(parent)
    , m_title(title)
    , m_bg_color(Qt::red) //! should stand out if it doesn't get set
    , m_override_bg_color(false)
    , m_set(set)
    , m_type(type)
    , m_count(-1)
    , m_export_data_role(DwarfModel::DR_SORT_VALUE)
    , m_current_sort(CST_DEFAULT)
{
    if(set) {
        set->add_column(this,col_idx);
        m_bg_color = set->bg_color();
        m_cell_colors = new ViewColumnColors(set,this);
    }else{
        m_cell_colors = new ViewColumnColors(this);
    }
    connect(DT, SIGNAL(settings_changed()), this, SLOT(read_settings()));
}

ViewColumn::ViewColumn(QSettings &s, ViewColumnSet *set, QObject *parent)
    : QObject(parent)
    , m_title(s.value("name", "UNKNOWN").toString())
    , m_bg_color(Qt::red) //! should stand out if it doesn't get set
    , m_override_bg_color(s.value("override_color", false).toBool())
    , m_set(set)
    , m_type(get_column_type(s.value("type", "DEFAULT").toString()))
    , m_count(-1)
    , m_export_data_role(DwarfModel::DR_SORT_VALUE)
    , m_current_sort(CST_DEFAULT)
{
    if(set){
        set->add_column(this);
        m_cell_colors = new ViewColumnColors(s,set,this);
    }else{
        m_cell_colors = new ViewColumnColors(this);
    }
    if(m_override_bg_color){
        m_bg_color = read_color(s.value("bg_color").toString());
    }else if(set){
        m_bg_color = set->bg_color();
    }
    connect(DT, SIGNAL(settings_changed()), this, SLOT(read_settings()));
}

ViewColumn::ViewColumn(const ViewColumn &to_copy)
    : QObject(to_copy.parent())
    , m_title(to_copy.m_title)
    , m_bg_color(to_copy.m_bg_color)
    , m_override_bg_color(to_copy.m_override_bg_color)
    , m_set(to_copy.m_set)
    , m_type(to_copy.m_type)
    , m_count(to_copy.m_count)
    , m_export_data_role(to_copy.m_export_data_role)
    , m_sortable_types(to_copy.m_sortable_types)
    , m_current_sort(to_copy.m_current_sort)
    , m_cell_colors(to_copy.m_cell_colors)
    , m_available_states(to_copy.m_available_states)
    , m_cell_color_map(to_copy.m_cell_color_map)
{
    // cloning should not add it to the copy's set! You must add it manually!
    if (m_set && !m_override_bg_color){
        m_bg_color = m_set->bg_color();
    }
}

ViewColumn::~ViewColumn(){
    m_cell_colors = 0;
    clear_cells();
    m_set = 0;
}

QStandardItem *ViewColumn::init_cell(Dwarf *d) {
    DTStandardItem *item = new DTStandardItem;
    item->setStatusTip(QString("%1 :: %2").arg(m_title).arg(d->nice_name()));
    QColor bg;
    if (m_override_bg_color) {
        bg = m_bg_color;
    } else {
        bg = set()->bg_color();
    }
    item->setData(complement(bg),Qt::TextColorRole);
    item->setData(bg, Qt::BackgroundColorRole);

    item->setData(bg, DwarfModel::DR_DEFAULT_BG_COLOR);
    item->setData(false, DwarfModel::DR_IS_AGGREGATE);
    item->setData(d->id(), DwarfModel::DR_ID);
    item->setData(0,DwarfModel::DR_BASE_SORT);
    m_cells[d] = item;

    return item;
}

QStandardItem *ViewColumn::init_aggregate(QString group_name){
    DTStandardItem *item = new DTStandardItem;

    item->setStatusTip(m_title + " :: " + group_name);

    QColor bg;
    if (m_override_bg_color)
        bg = m_bg_color;
    else
        bg = m_set->bg_color();
    item->setData(bg, Qt::BackgroundColorRole);
    item->setData(bg, DwarfModel::DR_DEFAULT_BG_COLOR);

    item->setData(group_name, DwarfModel::DR_GROUP_NAME);
    item->setData(-1, DwarfModel::DR_RATING);
    item->setData(-1, DwarfModel::DR_DISPLAY_RATING);
    item->setData(m_set->name(), DwarfModel::DR_SET_NAME);

    item->setData(true, DwarfModel::DR_IS_AGGREGATE);
    return item;
}

void ViewColumn::clear_cells(){
    m_cells.clear();
}

void ViewColumn::write_to_ini(QSettings &s) {
    if (!m_title.isEmpty()){
        s.setValue("name", m_title);
    }else{
        s.setValue("name", "UNKNOWN");
    }
    s.setValue("type", get_column_type(m_type));
    if (m_override_bg_color) {
        s.setValue("override_color", true);
        s.setValue("bg_color", m_bg_color);
    }
    m_cell_colors->write_to_ini(s);
}

QString ViewColumn::get_cell_value(Dwarf *d)
{
    return QString("%1").arg(m_cells.value(d)->data(m_export_data_role).toString());
}

QString ViewColumn::tooltip_name_footer(Dwarf *d){
    return QString("<center><h4>%1</h4></center>").arg(d->nice_name());
}

void ViewColumn::read_settings(){
    m_cell_colors->read_settings();
    refresh_color_map();
}

void ViewColumn::init_states(){
    m_available_states << STATE_TOGGLE << STATE_ACTIVE << STATE_DISABLED;
}

void ViewColumn::refresh_color_map(){
    m_cell_color_map.insert(STATE_ACTIVE,m_cell_colors->get_color(STATE_ACTIVE));
    m_cell_color_map.insert(STATE_PENDING,m_cell_colors->get_color(STATE_PENDING));
    m_cell_color_map.insert(STATE_DISABLED,m_cell_colors->get_color(STATE_DISABLED));
    //in the case that a column has a pending state, use that when toggling the cell
    if(m_available_states.contains(STATE_PENDING)){
        m_cell_color_map.insert(STATE_TOGGLE,m_cell_colors->get_color(STATE_PENDING));
    }else{
        m_cell_color_map.insert(STATE_TOGGLE,m_cell_colors->get_color(STATE_ACTIVE));
    }
}

QColor ViewColumn::get_state_color(int state) const{
    return m_cell_color_map.value(static_cast<CELL_STATE>(state),m_cell_colors->get_color(STATE_ACTIVE));
}
