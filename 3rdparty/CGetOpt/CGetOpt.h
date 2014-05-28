/**********************************************************************
 * Copyright (c) 2003, 2004, froglogic Porten & Stadlbauer GbR
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *    * Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials
 *      provided with the distribution.
 *
 *    * Neither the name of the froglogic nor the names of its
 *      contributors may be used to endorse or promote products
 *      derived from this software without specific prior written
 *      permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 **********************************************************************/

#ifndef GETOPT_H
#define GETOPT_H

#include <QStringList>
#include <QString>
#include <QMap>

class CGetOpt {
public:
    CGetOpt();
#ifndef QK_QT5_PORT
    CGetOpt( int offset );
#endif
    CGetOpt( int argc, char *argv[] );
    CGetOpt( const QStringList &a );

    QString appName() const { return aname; }

    // switch (no arguments)
    void addSwitch( const QString &lname, bool *b );
    void addSwitch( char sname, const QString &lname, bool *b );

    // options (with arguments, sometimes optional)
    void addOption( char s, const QString &l, QString *v );
    void addVarLengthOption( const QString &l, QStringList *v );
    void addRepeatableOption( char s, QStringList *v );
    void addRepeatableOption( const QString &l, QStringList *v );
    void addOptionalOption( const QString &l, QString *v,
                                const QString &def );
    void addOptionalOption( char s, const QString &l,
				QString *v, const QString &def );

    // bare arguments
    void addArgument( const QString &name, QString *v );
    void addOptionalArgument( const QString &name, QString *v );
    void addOptionalArguments( const QString &name, QStringList *v );

    bool parse( bool untilFirstSwitchOnly );
    bool parse() { return parse( false ); }

    bool isSet( const QString &name ) const;

    int currentArgument() const { return currArg; }

private:
    enum OptionType { OUnknown, OEnd, OSwitch, OArg1, OOpt, ORepeat, OVarLen };

    struct Option;
    friend struct Option;

    struct Option {
        Option( OptionType t = OUnknown,
                char s = 0, const QString &l = QString::null )
            : type( t ),
              sname( s ),
              lname( l ),
              boolValue( 0 ) { }

        OptionType type;
        char sname;		// short option name (0 if none)
        QString lname;	// long option name  (null if none)
        union {
            bool *boolValue;
            QString *stringValue;
            QStringList *listValue;
        };
        QString def;
    };

    QList<Option> options;
    typedef QList<Option>::const_iterator OptionConstIterator;
    QMap<QString, int> setOptions;

    void init( int argc, char *argv[], int offset = 1 );
    void addOption( Option o );
    void setSwitch( const Option &o );

    QStringList args;
    QString aname;

    int numReqArgs;
    int numOptArgs;
    Option reqArg;
    Option optArg;

    int currArg;
};

#endif

