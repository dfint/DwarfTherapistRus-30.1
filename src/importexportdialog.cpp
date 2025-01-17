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

#include "importexportdialog.h"
#include "ui_importexportdialog.h"
#include "dwarftherapist.h"
#include "customprofession.h"
#include "version.h"
#include "gamedatareader.h"
#include "labor.h"
#include "mainwindow.h"
#include "viewmanager.h"
#include "gridview.h"
#include "viewcolumnset.h"
#include "truncatingfilelogger.h"
#include "role.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#if QT_VERSION >= 0x050000
# include <QStandardPaths>
#else
# include <QDesktopServices>
# define QStandardPaths QDesktopServices
#endif

static QString _get_dir(QStandardPaths::StandardLocation locationId)
{
#if QT_VERSION >= 0x050000
    return QStandardPaths::writableLocation(locationId);
#else
    return QDesktopServices::storageLocation(locationId);
#endif
}

ImportExportDialog::ImportExportDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ImportExportDialog)
    , m_mode(MODE_UNSET)
{
    ui->setupUi(this);
    connect(ui->btn_select_none, SIGNAL(clicked()), SLOT(clear_selection()));
    connect(ui->btn_select_all, SIGNAL(clicked()), SLOT(select_all()));
}

bool ImportExportDialog::setup_for_profession_export() {
    m_mode = MODE_EXPORT_PROFESSIONS;
    QString default_path = QString("%1/%2")
        .arg(_get_dir(QStandardPaths::DesktopLocation))
        .arg("custom_professions.dtp");
    m_path = QFileDialog::getSaveFileName(this, trUtf8("Choose a file to export to"),	default_path,
        "Dwarf Therapist Profession Exports (*.dtp);;All Files (*.*)");
    if (m_path.isEmpty())
        return false; //cancelled
    LOGI << "exporting custom professions to:" << m_path;

    setWindowTitle(trUtf8("Export Custom Professions"));
    ui->buttonBox->addButton(trUtf8("Export Selected"), QDialogButtonBox::YesRole);
    Version v;
    QDateTime t = QDateTime::currentDateTime();
    ui->lbl_file_path->setText(m_path);
    ui->lbl_version->setText(v.to_string());
    ui->lbl_export_time->setText(t.toString());
    foreach(CustomProfession *cp, DT->get_custom_professions()) {
        QString title = trUtf8("%1 (%2 labors)").arg(cp->get_name()).arg(cp->get_enabled_labors().size());
        QListWidgetItem *i = new QListWidgetItem(title, ui->list_professions);
        i->setData(Qt::UserRole, cp->get_name());
        i->setData(Qt::UserRole+1, false); // not conflicting as far as we know
        i->setCheckState(Qt::Checked);
        m_profs << cp;
    }
    ui->lbl_professions_count->setText(QString::number(m_profs.size()));
    return true;
}

bool ImportExportDialog::setup_for_profession_import() {
    m_mode = MODE_IMPORT_PROFESSIONS;
    QString default_path = QString("%1/%2").arg(_get_dir(QStandardPaths::DesktopLocation)).arg("custom_professions.dtp");
    m_path = QFileDialog::getOpenFileName(this, trUtf8("Choose a file to import"), default_path,
        "Dwarf Therapist Profession Exports (*.dtp);;All Files (*.*)");
    if (m_path.isEmpty())
        return false; //cancelled
    LOGI << "importing custom professions from:" << m_path;

    setWindowTitle(trUtf8("Import Custom Professions"));
    ui->buttonBox->addButton(trUtf8("Import Selected"), QDialogButtonBox::YesRole);

    QSettings s(m_path, QSettings::IniFormat);

    /* don't need to check versions yet, since everything will be compatible */
    Version file_version;
    file_version.major = s.value("info/DT_version/major", 0).toInt();
    file_version.minor = s.value("info/DT_version/minor", 0).toInt();
    file_version.patch = s.value("info/DT_version/patch", 0).toInt();
    QDateTime t = s.value("info/export_date").toDateTime();

    int cnt = s.beginReadArray("custom_professions");

    for(int i = 0; i < cnt; i++) {
        s.setArrayIndex(i);
        m_profs.append(new CustomProfession(s,DT));
    }
    s.endArray();

    ui->lbl_file_path->setText(m_path);
    ui->lbl_version->setText(file_version.to_string());
    ui->lbl_export_time->setText(t.toString());
    ui->lbl_professions_count->setText(QString::number(m_profs.size()));
    foreach(CustomProfession *cp, m_profs) {
        QString title = trUtf8("%1 (%2 labors)").arg(cp->get_name())
                        .arg(cp->get_enabled_labors().size());
        QListWidgetItem *i = new QListWidgetItem(title, ui->list_professions);
        i->setData(Qt::UserRole, cp->get_name());
        i->setData(Qt::UserRole+1, false); // not conflicting as far as we know
        i->setCheckState(Qt::Checked);
        QString tooltip = "<h3>Enabled Labors</h3><ul>";
        GameDataReader *gdr = GameDataReader::ptr();
        foreach(int labor_id, cp->get_enabled_labors()) {
            QString labor_name = "UNKNOWN";
            Labor *l = gdr->get_labor(labor_id);
            if (l) {
                labor_name = l->name;
            } else {
                LOGE << trUtf8("custom profession lists labor_id %1, which is "
                            "unrecognized!").arg(labor_id);
            }
            tooltip += QString("<li>%1</li>").arg(labor_name);
        }
        tooltip += "</ul>";
        i->setToolTip(tooltip);

        // watch out for conflicts!
        if (DT->get_custom_profession(cp->get_name())) {
            i->setTextColor(Qt::red);
            i->setData(Qt::UserRole+1, true); // conflicting flag
            i->setText(i->text() + " CONFLICT");
            i->setCheckState(Qt::Unchecked);
            i->setFlags(Qt::NoItemFlags);
            i->setToolTip(trUtf8("You already have a custom profession with this name!"));
        }
    }
    return true;
}

bool ImportExportDialog::setup_for_gridview_export() {
    m_mode = MODE_EXPORT_GRIDVIEWS;
    QString default_path = QString("%1/%2")
        .arg(_get_dir(QStandardPaths::DesktopLocation))
        .arg("gridviews.dtg");
    m_path = QFileDialog::getSaveFileName(this, trUtf8("Choose a file to export to"),
        default_path,  "Dwarf Therapist Grid View Exports (*.dtg);;All Files (*.*)");
    if (m_path.isEmpty())
        return false; //cancelled
    LOGI << "exporting grid views to:" << m_path;
    setWindowTitle(trUtf8("Export Grid Views"));
    ui->buttonBox->addButton(trUtf8("Export Selected"), QDialogButtonBox::YesRole);
    Version v;
    QDateTime t = QDateTime::currentDateTime();
    ui->lbl_file_path->setText(m_path);
    ui->lbl_version->setText(v.to_string());
    ui->lbl_export_time->setText(t.toString());
    ui->lbl_total_title->setText(trUtf8("Total Views"));

    foreach(GridView *gv, DT->get_main_window()->get_view_manager()->views()) {
        QListWidgetItem *i = new QListWidgetItem(gv->name(), ui->list_professions);
        i->setData(Qt::UserRole, gv->name());
        i->setData(Qt::UserRole+1, false); // not conflicting as far as we know
        i->setCheckState(Qt::Checked);
        m_views << gv;
    }
    ui->lbl_professions_count->setText(QString::number(m_views.size()));
    return true;
}

bool ImportExportDialog::setup_for_gridview_import() {
    m_mode = MODE_IMPORT_GRIDVIEWS;
    QString default_path = QString("%1/%2")
        .arg(_get_dir(QStandardPaths::DesktopLocation))
        .arg("gridviews.dtg");
    m_path = QFileDialog::getOpenFileName(this, trUtf8("Choose a file to import"),
        default_path,  "Dwarf Therapist Grid View Exports (*.dtg);;All Files (*.*)");
    LOGI << "importing grid views from:" << m_path;

    setWindowTitle(trUtf8("Import Grid Views"));
    ui->buttonBox->addButton(trUtf8("Import Selected"), QDialogButtonBox::YesRole);

    QSettings s(m_path, QSettings::IniFormat);

    /* don't need to check versions yet, since everything will be compatible */
    Version file_version;
    file_version.major = s.value("info/DT_version/major", 0).toInt();
    file_version.minor = s.value("info/DT_version/minor", 0).toInt();
    file_version.patch = s.value("info/DT_version/patch", 0).toInt();
    QDateTime t = s.value("info/export_date").toDateTime();

    ViewManager *view_mgr = DT->get_main_window()->get_view_manager();
    int cnt = s.beginReadArray("gridviews");
    for(int i = 0; i < cnt; i++) {
        s.setArrayIndex(i);
        m_views << GridView::read_from_ini(s, this);
    }
    s.endArray();

    ui->lbl_file_path->setText(m_path);
    ui->lbl_version->setText(file_version.to_string());
    ui->lbl_export_time->setText(t.toString());
    ui->lbl_professions_count->setText(QString::number(m_views.size()));
    ui->lbl_total_title->setText(trUtf8("Total Views"));

    foreach(GridView *gv, m_views) {
        QListWidgetItem *i = new QListWidgetItem(gv->name(), ui->list_professions);
        i->setData(Qt::UserRole, gv->name());
        i->setData(Qt::UserRole+1, false); // not conflicting as far as we know
        i->setCheckState(Qt::Checked);

        QString tooltip = "<h3>BreakDown</h3><ul>";
        foreach(ViewColumnSet *set, gv->sets()) {
            tooltip += QString("<li>SET: %1 (%2 cols)</li>")
                .arg(set->name())
                .arg(set->columns().size());
        }
        tooltip += "</ul>";
        i->setToolTip(tooltip);

        // watch out for conflicts!
        if (view_mgr->get_view(gv->name())) {
            i->setTextColor(Qt::red);
            i->setData(Qt::UserRole+1, true); // conflicting flag
            i->setText(i->text() + " CONFLICT");
            i->setCheckState(Qt::Unchecked);
            i->setFlags(Qt::NoItemFlags);
            i->setToolTip(trUtf8("You already have a grid view with this name!"));
        }
    }
    return true;
}



bool ImportExportDialog::setup_for_role_export() {
    m_mode = MODE_EXPORT_ROLES;
    QString default_path = QString("%1/%2")
        .arg(_get_dir(QStandardPaths::DesktopLocation))
        .arg("custom_roles.dtp");
    m_path = QFileDialog::getSaveFileName(this, trUtf8("Choose a file to export to"),	default_path,
        "Dwarf Therapist Roles Exports (*.dtp);;All Files (*.*)");
    if (m_path.isEmpty())
        return false; //cancelled
    LOGI << "exporting custom roles to:" << m_path;

    setWindowTitle(trUtf8("Export Custom Roles"));
    ui->buttonBox->addButton(trUtf8("Export Selected"), QDialogButtonBox::YesRole);
    Version v;
    QDateTime t = QDateTime::currentDateTime();
    ui->lbl_file_path->setText(m_path);
    ui->lbl_version->setText(v.to_string());
    ui->lbl_export_time->setText(t.toString());
    ui->lbl_total_title->setText(trUtf8("Total Roles"));

    foreach(Role *r, GameDataReader::ptr()->get_roles()){
        if(r->is_custom()){
            QString title = QString("%1").arg(r->name());
            QListWidgetItem *i = new QListWidgetItem(title,ui->list_professions);
            i->setData(Qt::UserRole, r->name());
            i->setData(Qt::UserRole+1, false); //not conflicting as far as we know
            i->setCheckState(Qt::Checked);
            m_roles << r;
        }
    }
    ui->lbl_professions_count->setText(QString::number(m_roles.size()));
    return true;
}

bool ImportExportDialog::setup_for_role_import() {
    m_mode = MODE_IMPORT_ROLES;
    QString default_path = QString("%1/%2").arg(_get_dir(QStandardPaths::DesktopLocation)).arg("custom_roles.dtp");
    m_path = QFileDialog::getOpenFileName(this, trUtf8("Choose a file to import"), default_path,
        "Dwarf Therapist Roles Imports (*.dtp);;All Files (*.*)");
    if (m_path.isEmpty())
        return false; //cancelled
    LOGI << "importing custom roles from:" << m_path;

    setWindowTitle(trUtf8("Import Custom Roles"));
    ui->buttonBox->addButton(trUtf8("Import Selected"), QDialogButtonBox::YesRole);

    QSettings s(m_path, QSettings::IniFormat);

    /* don't need to check versions yet, since everything will be compatible */
    Version file_version;
    file_version.major = s.value("info/DT_version/major", 0).toInt();
    file_version.minor = s.value("info/DT_version/minor", 0).toInt();
    file_version.patch = s.value("info/DT_version/patch", 0).toInt();
    QDateTime t = s.value("info/export_date").toDateTime();

    int cnt = s.beginReadArray("custom_roles");
    for(int i = 0; i < cnt; i++) {
        s.setArrayIndex(i);
        Role *r = new Role(s, DT);
        m_roles << r;
    }
    s.endArray();

    ui->lbl_file_path->setText(m_path);
    ui->lbl_version->setText(file_version.to_string());
    ui->lbl_export_time->setText(t.toString());
    ui->lbl_professions_count->setText(QString::number(m_roles.size()));
    ui->lbl_total_title->setText(trUtf8("Total Roles"));
    foreach(Role *r, m_roles) {
        QString title = QString("%1").arg(r->name());
        QListWidgetItem *i = new QListWidgetItem(title, ui->list_professions);
        i->setData(Qt::UserRole, r->name());
        i->setData(Qt::UserRole+1, false); // not conflicting as far as we know
        i->setCheckState(Qt::Checked);
        i->setToolTip(r->get_role_details());

        // watch out for conflicts!
        Role *rCheck = GameDataReader::ptr()->get_role(r->name());
        if(rCheck && rCheck->is_custom()){
            i->setTextColor(Qt::red);
            i->setData(Qt::UserRole+1, true); // conflicting flag
            i->setText(trUtf8("%1 (%2!)").arg(i->text()).arg(trUtf8("CONFLICT")));
            i->setCheckState(Qt::Unchecked);
            i->setFlags(Qt::NoItemFlags);
            i->setToolTip(trUtf8("You already have a custom role with this name!"));
        }
    }
    return true;
}





void ImportExportDialog::select_all() {
    for(int i = 0; i < ui->list_professions->count(); ++i) {
        QListWidgetItem *item = static_cast<QListWidgetItem*>(ui->list_professions->item(i));
        if (item->data(Qt::UserRole + 1).toBool())
            continue;
        ui->list_professions->item(i)->setCheckState(Qt::Checked);
    }
}

void ImportExportDialog::clear_selection() {
    for(int i = 0; i < ui->list_professions->count(); ++i) {
        ui->list_professions->item(i)->setCheckState(Qt::Unchecked);
    }
}

QList<CustomProfession *> ImportExportDialog::get_profs() {
    QList<CustomProfession*> out;
    for(int i = 0; i < ui->list_professions->count(); ++i) {
        QListWidgetItem *item = static_cast<QListWidgetItem*>(ui->list_professions->item(i));
        if (item->checkState() != Qt::Checked)
            continue;
        QString name = item->data(Qt::UserRole).toString();
        foreach(CustomProfession *cp, m_profs) {
            if (name == cp->get_name())
                out << cp;
        }
    }
    return out;
}

QList<GridView *> ImportExportDialog::get_views() {
    QList<GridView*> out;
    for(int i = 0; i < ui->list_professions->count(); ++i) {
        QListWidgetItem *item = static_cast<QListWidgetItem*>(ui->list_professions->item(i));
        if (item->checkState() != Qt::Checked)
            continue;
        QString name = item->data(Qt::UserRole).toString();
        foreach(GridView *gv, m_views) {
            if (name == gv->name())
                out << gv;
        }
    }
    return out;
}

QList<Role *> ImportExportDialog::get_roles(){
    QList<Role*> out;
    for(int i = 0; i < ui->list_professions->count(); i++){
        QListWidgetItem *item = static_cast<QListWidgetItem*>(ui->list_professions->item(i));
        if(item->checkState() != Qt::Checked)
            continue;
        QString name = item->data(Qt::UserRole).toString();
        foreach(Role *r, m_roles){
            if(name == r->name())
                out << r;
        }
    }
    return out;
}

void ImportExportDialog::accept() {
    switch(m_mode) {
    case MODE_EXPORT_PROFESSIONS:
        export_selected_professions();
        break;
    case MODE_IMPORT_PROFESSIONS:
        import_selected_professions();
        break;
    case MODE_EXPORT_GRIDVIEWS:
        export_selected_gridviews();
        break;
    case MODE_IMPORT_GRIDVIEWS:
        import_selected_gridviews();
        break;
    case MODE_EXPORT_ROLES:
        export_selected_roles();
        break;
    case MODE_IMPORT_ROLES:
        import_selected_roles();
        break;
    default:
        QMessageBox::warning(this, trUtf8("Oh no!"), trUtf8("Unknown import/export type!"));
        return;
    }
    return QDialog::accept();
}

void ImportExportDialog::export_selected_roles() {
    QSettings *u = DT->user_settings();
    float default_attributes_weight = u->value("options/default_attributes_weight",1.0).toFloat();
    float default_skills_weight = u->value("options/default_skills_weight",1.0).toFloat();
    float default_traits_weight = u->value("options/default_traits_weight",1.0).toFloat();
    float default_prefs_weight = u->value("options/default_prefs_weight",1.0).toFloat();

    QSettings s(m_path, QSettings::IniFormat);
    s.remove(""); // clear out the file if there was anything there.
    Version v;
    s.setValue("info/DT_version/major", v.major);
    s.setValue("info/DT_version/minor", v.minor);
    s.setValue("info/DT_version/patch", v.patch);
    s.setValue("info/export_date", QDateTime::currentDateTime());

    int exported = 0;
    int i = 0;
    s.beginWriteArray("custom_roles");
    foreach(Role *r, get_roles()) {
        s.setArrayIndex(i++);
        r->write_to_ini(s, default_attributes_weight, default_traits_weight, default_skills_weight, default_prefs_weight);
        exported++;
    }
    s.endArray();
    s.sync();
    if (exported)
        QMessageBox::information(this, trUtf8("Export Successful"),
            trUtf8("Exported %n custom role(s)", "", exported));
    m_roles.clear();
}

void ImportExportDialog::export_selected_professions() {
    QSettings s(m_path, QSettings::IniFormat);
    s.remove(""); // clear out the file if there was anything there.
    Version v;
    s.setValue("info/DT_version/major", v.major);
    s.setValue("info/DT_version/minor", v.minor);
    s.setValue("info/DT_version/patch", v.patch);
    s.setValue("info/export_date", QDateTime::currentDateTime());

    int exported = 0;
    int i = 0;
    s.beginWriteArray("custom_professions");
    foreach(CustomProfession *cp, get_profs()) {
        s.setArrayIndex(i++);
        cp->export_to_file(s);
        exported++;
    }
    s.endArray();
    s.sync();
    if (exported)
        QMessageBox::information(this, trUtf8("Export Successful"),trUtf8("Exported %n custom profession(s)", "", exported));
    m_profs.clear();
}


void ImportExportDialog::import_selected_roles(){
    int imported = 0;
    foreach(Role *r, get_roles()){
        r->is_custom(true);
        r->create_role_details(*DT->user_settings());
        GameDataReader::ptr()->get_roles().insert(r->name(), r);
        imported++;
    }
    DT->get_main_window()->write_custom_roles();
    DT->get_main_window()->refresh_roles_data();
    if(imported)
        QMessageBox::information(this, trUtf8("Import Successful"),
            trUtf8("Imported %n custom role(s)", "", imported));

    m_roles.clear();
}

void ImportExportDialog::import_selected_professions() {
    int imported = m_profs.count();
    DT->add_custom_professions(m_profs);
    if (imported)
        QMessageBox::information(this, trUtf8("Import Successful"),
            trUtf8("Imported %n custom profession(s)", "", imported));

    m_profs.clear();
}

void ImportExportDialog::export_selected_gridviews() {
    QSettings s(m_path, QSettings::IniFormat);
    s.remove(""); // clear out the file if there was anything there.
    Version v;
    s.setValue("info/DT_version/major", v.major);
    s.setValue("info/DT_version/minor", v.minor);
    s.setValue("info/DT_version/patch", v.patch);
    s.setValue("info/export_date", QDateTime::currentDateTime());

    int exported = 0;
    int i = 0;
    s.beginWriteArray("gridviews");
    foreach(GridView *gv, get_views()) {
        s.setArrayIndex(i++);
        gv->write_to_ini(s);
        exported++;
    }
    s.endArray();
    s.sync();
    if (exported)
        QMessageBox::information(this, trUtf8("Export Successful"),
            trUtf8("Exported %n grid view(s)", "", exported));

    m_views.clear();
}

void ImportExportDialog::import_selected_gridviews() {
    int imported = 0;
    ViewManager *view_mgr = DT->get_main_window()->get_view_manager();
    foreach(GridView *gv, get_views()) {
        view_mgr->add_view(gv);
        imported++;
    }
    if (imported) {
        QMessageBox::information(this, trUtf8("Import Successful"),
            trUtf8("Imported %n grid view(s)", "", imported));
        view_mgr->write_views();
        view_mgr->reload_views();
    }
    m_views.clear();
}
