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
#ifndef CELLCOLORS_H
#define CELLCOLORS_H

#include <QObject>
#include <QSettings>
#include <QColor>
#include <QVector>
#include <QSharedPointer>
#include "cellcolordef.h"

class CellColors : public QObject {
    Q_OBJECT
public:
    CellColors(QObject *parent=0);
    CellColors(const CellColors &cc);
    virtual ~CellColors();

    bool overrides_cell_colors() {return m_override_cell_colors;}
    void set_overrides_cell_colors(bool val) {m_override_cell_colors = val;}

    virtual void inherit_colors(const CellColors &cc);

    QColor get_color(int idx) const;
    void set_color(int idx, QColor c);

    QVector<QSharedPointer<CellColorDef> > get_color_defs();

public slots:
    virtual void read_settings(){}
    virtual void write_to_ini(QSettings &s);
    virtual void use_defaults();

protected:
    bool m_override_cell_colors;
    QVector<QSharedPointer<CellColorDef> > m_color_defs;

    virtual void load_settings(QSettings &s);
    virtual QColor get_default_color(int idx) const;
    virtual QSharedPointer<CellColorDef> get_default_color_def(int idx);

};
#endif // CELLCOLORS_H
