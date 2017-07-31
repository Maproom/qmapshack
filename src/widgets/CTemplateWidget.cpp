/**********************************************************************************************
    Copyright (C) 2017 Oliver Eichler oliver.eichler@gmx.de

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

**********************************************************************************************/

#include "helpers/CSettings.h"
#include "helpers/String.h"
#include "widgets/CTemplateWidget.h"

#include <QtGui>
#include <QtUiTools>

CTemplateWidget::CTemplateWidget(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);
    connect(comboTemplates, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &CTemplateWidget::slotTemplateActivated);
    connect(pushPreview, &QPushButton::pressed, this, &CTemplateWidget::slotPreview);
    connect(toolPathTemplates, &QToolButton::pressed, this, &CTemplateWidget::slotSetPath);
    listTemplates();
}


void CTemplateWidget::listTemplates()
{
    comboTemplates->clear();
    comboTemplates->addItem(tr("choose one..."));
    comboTemplates->addItem(tr("Hiking Tour Summary (built-in)"), s_("://templates/Hiking_Tour_Summary.ui"));

    SETTINGS;
    const QString& path = cfg.value(s_("TextEditWidget/templatePath"), s_("")).toString();

    if(!path.isEmpty())
    {
        QDir dir(path);
        QStringList files = dir.entryList(QStringList(s_("*.ui")), QDir::Files);
        for(const QString& file : files)
        {
            QString name = QFileInfo(file).completeBaseName().replace(s_("_"), s_(" "));
            comboTemplates->addItem(name, dir.absoluteFilePath(file));
        }
    }

    const QString& data = cfg.value(s_("TextEditWidget/template"), "").toString();
    const int idx = comboTemplates->findData(data);
    if(idx != -1)
    {
        comboTemplates->setCurrentIndex(idx);
    }
}

QString CTemplateWidget::text()
{
    if(widget.isNull())
    {
        return s_("");
    }
    QString str;

    QList<QGroupBox*> groups = widget->findChildren<QGroupBox*>(QRegExp("group.*"), Qt::FindDirectChildrenOnly);
    qSort(groups.begin(), groups.end(), [](const QGroupBox * g1, const QGroupBox * g2){return g1->objectName() < g2->objectName(); });

    for(const QGroupBox * group : groups)
    {
        str += QString(s_("<p><b>%1</b>: ")).arg(group->title());
        str += resolveGroup(group);
        str += s_("</p>");
    }

    return str;
}

QString CTemplateWidget::resolveGroup(const QGroupBox * group)
{
    QString str;
    QList<QWidget *> widgets = group->findChildren<QWidget*>(QRegExp(".*"), Qt::FindDirectChildrenOnly);
    qSort(widgets.begin(), widgets.end(), [](const QWidget * w1, const QWidget * w2){return w1->property("order") < w2->property("order"); });

    for(const QWidget * w : widgets)
    {
        const QString pre(str.isEmpty() ? s_("") : s_(", "));


        if(w->objectName().startsWith(s_("check")))
        {
            const QCheckBox * obj = dynamic_cast<const QCheckBox*>(w);
            if((obj != nullptr) && obj->isChecked())
            {
                str += pre + obj->text().replace(s_("&"),s_(""));
            }
        }
        else if(w->objectName().startsWith(s_("radio")))
        {
            const QRadioButton * obj = dynamic_cast<const QRadioButton*>(w);
            if((obj != nullptr) && obj->isChecked())
            {
                str += pre + obj->text().replace(s_("&"),s_(""));
            }
        }
        else if(w->objectName().startsWith(s_("combo")))
        {
            const QComboBox * obj = dynamic_cast<const QComboBox*>(w);
            if(obj != nullptr && !obj->currentText().isEmpty())
            {
                str += pre + obj->currentText();
            }
        }
        else if(w->objectName().startsWith(s_("line")))
        {
            const QLineEdit * obj = dynamic_cast<const QLineEdit*>(w);
            if((obj != nullptr) && !obj->text().simplified().isEmpty())
            {
                str += pre + obj->text();
            }
        }
        else if(w->objectName().startsWith(s_("text")))
        {
            const QTextEdit * obj = dynamic_cast<const QTextEdit*>(w);
            if((obj != nullptr) && !obj->toPlainText().simplified().isEmpty())
            {
                str += pre + obj->toHtml();
            }
        }
    }

    if(str.isEmpty())
    {
        str += tr("-");
    }

    return str;
}

void CTemplateWidget::slotSetPath()
{
    SETTINGS;
    QString path = cfg.value(s_("TextEditWidget/templatePath"), QDir::homePath()).toString();
    path = QFileDialog::getExistingDirectory(this, tr("Template path..."), path);
    if(path.isEmpty())
    {
        return;
    }
    cfg.setValue(s_("TextEditWidget/templatePath"), path);
}

void CTemplateWidget::slotTemplateActivated(int idx)
{
    SETTINGS;

    delete widget;
    if(idx == 0)
    {
        return;
    }

    const QString& filename = comboTemplates->itemData(idx).toString();
    QFile file(filename);
    if(!file.open(QFile::ReadOnly))
    {
        QMessageBox::critical(this, tr("Failed..."), tr("Failed to read template file %1.").arg(filename));
        comboTemplates->setCurrentIndex(0);
        cfg.setValue(s_("TextEditWidget/template"), s_(""));
    }
    else
    {
        widget = QUiLoader().load(&file, this);
        layoutWidget->insertWidget(0,widget);
        file.close();
        cfg.setValue(s_("TextEditWidget/template"), filename);

        // convert focus chain into a sortable property.
        quint32 cnt     = 0;
        QWidget * first = nextInFocusChain();
        QWidget * next  = first;
        do
        {
            const QString& name = next->objectName();
            if(name.startsWith(s_("check"))
               ||name.startsWith(s_("radio"))
               ||name.startsWith(s_("combo"))
               ||name.startsWith(s_("line"))
               ||name.startsWith(s_("text")))
            {
                next->setProperty("order", cnt++);
            }

            next = next->nextInFocusChain();
        }
        while(next != first);
    }
}

void CTemplateWidget::slotPreview()
{
    QTextBrowser * preview = new QTextBrowser();

    preview->setAttribute(Qt::WA_DeleteOnClose, true);
    preview->setWindowModality(Qt::ApplicationModal);
    preview->setReadOnly(true);
    preview->setHtml(text());
    preview->setWindowTitle(tr("Preview..."));

    preview->setMinimumWidth(600);
    preview->move(QApplication::desktop()->screen()->rect().center() - preview->rect().center());
    preview->show();
    preview->raise();

    QAction * action = new QAction(preview);
    action->setShortcut(Qt::Key_Escape);

    preview->addAction(action);
    connect(action, &QAction::triggered, preview, &QTextBrowser::close);
}
