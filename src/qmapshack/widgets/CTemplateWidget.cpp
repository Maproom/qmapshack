/**********************************************************************************************
    Copyright (C) 2017 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "widgets/CTemplateWidget.h"

#include <QtGui>
#include <QtUiTools>
#include <QtWidgets>

#include "helpers/CSettings.h"

CTemplateWidget::CTemplateWidget(QWidget* parent) : QDialog(parent) {
  setupUi(this);
  connect(comboTemplates, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
          &CTemplateWidget::slotTemplateActivated);
  connect(pushPreview, &QPushButton::pressed, this, &CTemplateWidget::slotPreview);
  connect(toolPathTemplates, &QToolButton::pressed, this, &CTemplateWidget::slotSetPath);
  listTemplates();
}

void CTemplateWidget::listTemplates() {
  comboTemplates->clear();
  comboTemplates->addItem(tr("choose one..."));
  comboTemplates->addItem(tr("Cycling Tour Summary (built-in)"), "://templates/Cycling_Tour_Summary.ui");
  comboTemplates->addItem(tr("Hiking Tour Summary (built-in)"), "://templates/Hiking_Tour_Summary.ui");
  comboTemplates->addItem(tr("Hiking Tour Summary - Advanced (built-in)"),
                          "://templates/Hiking_Tour_Summary_(Advanced).ui");
  comboTemplates->addItem(tr("Running Summary (built-in)"), "://templates/Running_Summary.ui");

  SETTINGS;
  const QString& path = cfg.value("TextEditWidget/templatePath", "").toString();

  if (!path.isEmpty()) {
    QDir dir(path);
    const QStringList& files = dir.entryList(QStringList("*.ui"), QDir::Files);
    for (const QString& file : files) {
      QString name = QFileInfo(file).completeBaseName().replace("_", " ");
      comboTemplates->addItem(name, dir.absoluteFilePath(file));
    }
  }

  const QString& data = cfg.value("TextEditWidget/template", "").toString();
  const int idx = comboTemplates->findData(data);
  if (idx != -1) {
    comboTemplates->setCurrentIndex(idx);
  }
}

QString CTemplateWidget::text() {
  if (widget.isNull()) {
    return "";
  }
  QString str;

  QList<QGroupBox*> groups =
      widget->findChildren<QGroupBox*>(QRegularExpression("group.*"), Qt::FindDirectChildrenOnly);
  std::sort(groups.begin(), groups.end(),
            [](const QGroupBox* g1, const QGroupBox* g2) { return g1->objectName() < g2->objectName(); });

  for (const QGroupBox* group : std::as_const(groups)) {
    str += QString("<p><b>%1</b>: ").arg(group->title());
    str += resolveGroup(group);
    str += "</p>";
  }

  return str;
}

QString CTemplateWidget::resolveGroup(const QGroupBox* group) {
  QString str;
  QList<QWidget*> widgets = group->findChildren<QWidget*>(QRegularExpression(".*"), Qt::FindDirectChildrenOnly);
  std::sort(widgets.begin(), widgets.end(), [](const QWidget* w1, const QWidget* w2) {
    return w1->property("order").toInt() < w2->property("order").toInt();
  });

  for (const QWidget* w : std::as_const(widgets)) {
    const QString pre(str.isEmpty() ? "" : ", ");

    {
      const QCheckBox* obj = dynamic_cast<const QCheckBox*>(w);
      if (obj != nullptr) {
        if (obj->isChecked()) {
          str += pre + obj->text().replace("&", "");
        }
        continue;
      }
    }

    {
      const QRadioButton* obj = dynamic_cast<const QRadioButton*>(w);
      if (obj != nullptr) {
        if (obj->isChecked()) {
          str += pre + obj->text().replace("&", "");
        }
        continue;
      }
    }

    {
      const QComboBox* obj = dynamic_cast<const QComboBox*>(w);
      if (obj != nullptr) {
        if (!obj->currentText().isEmpty()) {
          str += pre + obj->currentText();
        }
        continue;
      }
    }

    {
      const QLineEdit* obj = dynamic_cast<const QLineEdit*>(w);
      if (obj != nullptr) {
        if (!obj->text().simplified().isEmpty()) {
          str += pre + obj->text();
        }
        continue;
      }
    }

    {
      const QTextEdit* obj = dynamic_cast<const QTextEdit*>(w);
      if (obj != nullptr) {
        if (!obj->toPlainText().simplified().isEmpty()) {
          str += pre + obj->toHtml();
        }
        continue;
      }
    }
  }

  if (str.isEmpty()) {
    str += tr("-");
  }

  return str;
}

void CTemplateWidget::slotSetPath() {
  SETTINGS;
  QString path = cfg.value("TextEditWidget/templatePath", QDir::homePath()).toString();
  path = QFileDialog::getExistingDirectory(this, tr("Template path..."), path);
  if (path.isEmpty()) {
    return;
  }
  cfg.setValue("TextEditWidget/templatePath", path);
  listTemplates();
}

void CTemplateWidget::slotTemplateActivated(int idx) {
  delete widget;
  if (idx < 1) {
    pushPreview->setEnabled(false);
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    return;
  }

  bool success = false;
  const QString& filename = comboTemplates->itemData(idx).toString();
  QFile file(filename);
  if (!file.open(QFile::ReadOnly)) {
    widget = new QLabel(tr("Failed to read template file %1.").arg(filename));
  } else {
    QUiLoader loader;
    widget = loader.load(&file, this);
    file.close();

    if (widget.isNull()) {
      widget = new QLabel(loader.errorString());
    } else if (nextInFocusChain() != nullptr) {
      // convert focus chain into a sortable property.
      quint32 cnt = 0;
      QWidget* first = nextInFocusChain();
      QWidget* next = first;
      do {
        if ((dynamic_cast<QCheckBox*>(next) != nullptr) || (dynamic_cast<QRadioButton*>(next) != nullptr) ||
            (dynamic_cast<QComboBox*>(next) != nullptr) || (dynamic_cast<QLineEdit*>(next) != nullptr) ||
            (dynamic_cast<QTextEdit*>(next) != nullptr)) {
          next->setProperty("order", cnt++);
        }

        next = next->nextInFocusChain();
      } while (next && next != first);

      success = true;
    }
  }

  layoutWidget->insertWidget(0, widget);
  pushPreview->setEnabled(success);
  buttonBox->button(QDialogButtonBox::Ok)->setEnabled(success);

  SETTINGS;
  cfg.setValue("TextEditWidget/template", filename);

  adjustSize();
}

void CTemplateWidget::slotPreview() {
  QTextBrowser* preview = new QTextBrowser();

  preview->setAttribute(Qt::WA_DeleteOnClose, true);
  preview->setWindowModality(Qt::ApplicationModal);
  preview->setReadOnly(true);
  preview->setHtml(text());
  preview->setWindowTitle(tr("Preview..."));

  preview->setMinimumWidth(600);
  const QList<QScreen*>& screens = QGuiApplication::screens();
  preview->move(screens.first()->availableGeometry().center() - -preview->rect().center());

  preview->show();
  preview->raise();

  QAction* action = new QAction(preview);
  action->setShortcut(Qt::Key_Escape);

  preview->addAction(action);
  connect(action, &QAction::triggered, preview, &QTextBrowser::close);
}
