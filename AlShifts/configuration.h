#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QString>
#include <QtGui/QFont>

class Configuration
{
public:
    //Read configuration
    void read();
    //Write configuration
    void write() const;

    //font configuration
    void setFontSettings(QFont font, int fsize, int fzoom);
    void writeFontSettings();
    void readFontSettings();

    QFont editorFont();
    int editorFontSize() { return m_editorFontSize; }
    int editorFontZoom() { return m_editorFontZoom; }

private:
    Configuration();
    Configuration(const Configuration&);

    //private members variables
    QString m_fpcExecutable;
    //Editor Font Settings
    QFont m_editorFont;
    int m_editorFontSize;
    int m_editorFontZoom;

    // allow this function to create one instance
    friend Configuration& Config();

};

// use this function to access the settings
Configuration& Config();

#endif // CONFIGURATION_H
