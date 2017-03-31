#include <QSettings>
#include <QtDebug>
#include <QMessageBox>

#include "configuration.h"

Configuration::Configuration()
{
    QSettings settings;
    qDebug() <<  settings.applicationName() << "\n" <<
              settings.organizationName() << endl;
}

Configuration& Config() {
    static Configuration conf;
    return conf;
}

void Configuration::read()
{
    QSettings settings;
    ///TODO : Read your settings.
}

void Configuration::write() const
{
    QSettings settings;

    ///TODO : Write your settings to seetings object.

}

void Configuration::setFontSettings(QFont font, int fsize, int fzoom)
{
    m_editorFont = font;
    m_editorFontSize = fsize;
    m_editorFontZoom = fzoom;
}

QFont Configuration::editorFont()
{
     return m_editorFont;
}
