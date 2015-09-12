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
#ifndef TRAINEDCOLUMN_H
#define TRAINEDCOLUMN_H

#include "viewcolumn.h"
#include "global_enums.h"

class ViewColumn;
class Dwarf;

class TrainedColumn : public ViewColumn {
    Q_OBJECT
public:
    TrainedColumn(QSettings &s, ViewColumnSet *set, QObject *parent = 0);
    TrainedColumn(const QString &title, ViewColumnSet *set = 0, QObject *parent = 0);
    TrainedColumn(const TrainedColumn &to_copy); // copy ctor
    TrainedColumn* clone() {return new TrainedColumn(*this);}
    QStandardItem *build_cell(Dwarf *d);
    QStandardItem *build_aggregate(const QString &, const QVector<Dwarf*> &);

};

#endif // TRAINEDCOLUMN_H
