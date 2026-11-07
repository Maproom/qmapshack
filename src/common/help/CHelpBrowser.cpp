/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "help/CHelpBrowser.h"

#include <QBuffer>
#include <QContextMenuEvent>
#include <QDesktopServices>
#include <QEvent>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMenu>
#include <QMimeDatabase>
#include <QWebEngineContextMenuRequest>
#include <QWebEngineHistory>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>
#include <QWebEngineSettings>
#include <QWebEngineUrlRequestJob>
#include <QWebEngineUrlScheme>
#include <QWebEngineUrlSchemeHandler>
#include <QtHelp>

#include "theme/CUiTheme.h"

static constexpr char kScheme[] = "qthelp";
static constexpr char kThemeScript[] = "qmsTheme";

static bool isWebUrl(const QUrl& url) { return url.scheme() == "http" || url.scheme() == "https"; }

namespace {
/** @brief Answers qthelp:// requests from the help engine's compressed help file. */
class CHelpSchemeHandler : public QWebEngineUrlSchemeHandler {
 public:
  CHelpSchemeHandler(QHelpEngine* engine, QObject* parent) : QWebEngineUrlSchemeHandler(parent), engine(engine) {}

  void requestStarted(QWebEngineUrlRequestJob* job) override {
    const QUrl& url = job->requestUrl();
    const QByteArray& data = engine->fileData(url);
    if (data.isEmpty()) {
      job->fail(QWebEngineUrlRequestJob::UrlNotFound);
      return;
    }

    QByteArray mime = QMimeDatabase().mimeTypeForFile(url.path(), QMimeDatabase::MatchExtension).name().toUtf8();
    // Help pages are UTF-8, and some carry no charset of their own; Chromium would assume Windows-1252.
    if (mime.startsWith("text/")) {
      mime += ";charset=utf-8";
    }
    QBuffer* buffer = new QBuffer(job);
    buffer->setData(data);
    job->reply(mime, buffer);
  }

 private:
  QHelpEngine* engine;
};

/** @brief Hands a clicked web link to the system browser and loads no web page itself. */
class CHelpPage : public QWebEnginePage {
 public:
  CHelpPage(QWebEngineProfile* profile, QObject* parent) : QWebEnginePage(profile, parent) {}

  bool acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame) override {
    if (isWebUrl(url)) {
      if (type == NavigationTypeLinkClicked && isMainFrame) {
        QDesktopServices::openUrl(url);
      }
      return false;
    }
    return QWebEnginePage::acceptNavigationRequest(url, type, isMainFrame);
  }
};
}  // namespace

CHelpBrowser::CHelpBrowser(QHelpEngine* helpEngine, QWidget* parent)
    : QWebEngineView(parent), engine(helpEngine), profile(new QWebEngineProfile(this)) {
  profile->installUrlSchemeHandler(kScheme, new CHelpSchemeHandler(helpEngine, this));
  // A font the help CSS names but the system lacks falls back to the application's, not to serif.
  profile->settings()->setFontFamily(QWebEngineSettings::StandardFont, font().family());
  setPage(new CHelpPage(profile, this));
  applyTheme();

  connect(helpEngine->contentWidget(), &QHelpContentWidget::linkActivated, this, &CHelpBrowser::setSource);
  connect(helpEngine->indexWidget(), &QHelpIndexWidget::documentActivated, this,
          [this](const QHelpLink& document, const QString& keyword) { setSource(document.url); });
  connect(helpEngine->searchEngine()->resultWidget(), &QHelpSearchResultWidget::requestShowLink, this,
          &CHelpBrowser::setSource);
}

CHelpBrowser::~CHelpBrowser() {
  // The profile must outlive its page, but as a child of this it would be deleted first.
  delete page();
}

void CHelpBrowser::registerUrlScheme() {
  QWebEngineUrlScheme scheme(kScheme);
  scheme.setSyntax(QWebEngineUrlScheme::Syntax::Host);
  // CORS and Fetch allow page scripts to fetch() files from the help file.
  scheme.setFlags(QWebEngineUrlScheme::SecureScheme | QWebEngineUrlScheme::LocalScheme |
                  QWebEngineUrlScheme::CorsEnabled | QWebEngineUrlScheme::FetchApiAllowed);
  QWebEngineUrlScheme::registerScheme(scheme);
}

void CHelpBrowser::setSource(const QUrl& url) {
  if (isWebUrl(url)) {
    QDesktopServices::openUrl(url);
    return;
  }
  // The table of contents links its chapter nodes to no file at all.
  if (url.scheme() == kScheme && engine->fileData(url).isEmpty()) {
    return;
  }
  if (homeUrl.isEmpty()) {
    homeUrl = url;
  }
  setUrl(url);
}

void CHelpBrowser::changeEvent(QEvent* e) {
  QWebEngineView::changeEvent(e);
  if (CUiTheme::isPaletteChange(e)) {
    applyTheme();
  }
}

void CHelpBrowser::applyTheme() {
  // The packaged help CSS leaves the page colours to the viewer. Add a stylesheet after it, so its
  // rules win, and swap its text on a palette change instead of reloading the page.
  const QPalette& pal = palette();
  const QString& css = QString(
                           ":root {color-scheme:%1}\n"
                           "body {color:%2; background-color:%3}\n"
                           "a:link {color:%4}\n"
                           "a:visited {color:%5}\n"
                           "code, pre {%6}\n")
                           .arg(CUiTheme::isDark() ? "dark" : "light", pal.color(QPalette::Text).name(),
                                pal.color(QPalette::Base).name(), pal.color(QPalette::Link).name(),
                                pal.color(QPalette::LinkVisited).name(), CUiTheme::css(CUiTheme::Role::eCode));

  const QString& literal = QString::fromUtf8(QJsonDocument(QJsonArray{css}).toJson(QJsonDocument::Compact));
  const QString& source = QString(
                              "(function() {"
                              "  let style = document.getElementById('qms-theme');"
                              "  if (!style) {"
                              "    style = document.createElement('style');"
                              "    style.id = 'qms-theme';"
                              "    (document.head || document.documentElement).appendChild(style);"
                              "  }"
                              "  style.textContent = %1[0];"
                              "})();")
                              .arg(literal);

  QWebEngineScriptCollection& scripts = page()->scripts();
  for (const QWebEngineScript& old : scripts.find(kThemeScript)) {
    scripts.remove(old);
  }
  QWebEngineScript script;
  script.setName(kThemeScript);
  script.setInjectionPoint(QWebEngineScript::DocumentReady);
  script.setWorldId(QWebEngineScript::ApplicationWorld);
  script.setSourceCode(source);
  scripts.insert(script);

  page()->setBackgroundColor(pal.color(QPalette::Base));
  page()->runJavaScript(source, QWebEngineScript::ApplicationWorld);
}

void CHelpBrowser::contextMenuEvent(QContextMenuEvent* event) {
  QMenu* menu = new QMenu(this);

  const QWebEngineContextMenuRequest* request = lastContextMenuRequest();
  if (request != nullptr && !request->selectedText().isEmpty()) {
    menu->addAction(pageAction(QWebEnginePage::Copy));
  }
  if (request != nullptr && request->linkUrl().isValid()) {
    menu->addAction(pageAction(QWebEnginePage::CopyLinkToClipboard));
  }
  menu->addAction(pageAction(QWebEnginePage::SelectAll));

  QWebEngineHistory* hist = history();
  menu->addSeparator();
  if (hist->canGoBack()) {
    menu->addAction(QIcon(":/icons/Left.svgt"), tr("Go back one page"), Qt::CTRL | Qt::Key_Left, this,
                    &CHelpBrowser::back)
        ->setObjectName("actionBackward");
  }
  if (hist->canGoForward()) {
    menu->addAction(QIcon(":/icons/Right.svgt"), tr("Go forward one page"), Qt::CTRL | Qt::Key_Right, this,
                    &CHelpBrowser::forward)
        ->setObjectName("actionForward");
  }
  if (hist->canGoBack()) {
    menu->addAction(QIcon(":/icons/ToTop.svgt"), tr("Go to initial page"), Qt::CTRL | Qt::Key_Up, this,
                    [this]() { setSource(homeUrl); })
        ->setObjectName("actionHome");
  }

  menu->exec(event->globalPos());

  delete menu;
}
