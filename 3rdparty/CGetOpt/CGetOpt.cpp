#define QT_NO_CAST_ASCII
#define QT_NO_ASCII_CAST

#include "CGetOpt.h"

#include <QApplication>
#include <QFileInfo>
#include <QStack>
#include <stdlib.h>

#include <assert.h>

/**
   \class CGetOpt

   \brief A command line option parser.

   This class helps to overcome the repetitive, tedious and
   error-prone task of parsing the command line options passed to your
   application by the user. Specify the acceptable syntax with a
   minimum of statements in a readable way, check it against the
   actual arguments passed and find the retrieved values in variables
   of your program. The name \em CGetOpt is based on similar utilities
   build into the Unix shell and other languages.

   A command line that a user might have entered is:

   \code
   app -v --config=my.cnf -Wall input.dat
   \endcode

   The typical usage has three stages:

   -# Construct a parser specifying what arguments to parse
   -# Set up the list of allowed and required options
   -# Run the parser

   For the first step there are three different constructors that
   either take arguments directly from \c main(), \c QApplication or a
   user specified list. Setting up the accepted syntax is done by a
   set of \c add functions like addSwitch(). The final step of running
   the parser is simply done by calling parse().

   A short example implementing a \c --verbose switch:

   \code
   int main(int argc, char **argv)
   {
       CGetOpt opts(argc, argv);
       bool verbose;
       opts.addSwitch("verbose", &verbose);
       if (!opts.parse())
           return 1;
       if (verbose)
           cout << "VERBOSE mode on" << endl;
       ...
   \endcode

   For a better understanding of the function names we'll better
   define some terms used in the API and its documentation:

   - \em Argument An argument is a plain text token like e.g. a file
   name one typically passes to an editor when invoking it.
   - \em Switch A switch is an on/off kind of argument without the need
     of additional information. Example: \c --debug.
   - \em Option An option is a normally optional argument with a key-value
   syntax like \c --output=out.txt or \c -I/usr/include.
   - \em Short \em Option A short option is a one letter option with a
   preceding dash. Like \c -v.
   - \em Long \em Option A long option has a more verbose,
   multi-letter name like \c --debug.
   .

   \author froglogic GbR <contact@froglogic.com>
*/


/**
   Constructs a command line parser from the arguments stored in a
   previously created QApplication instance.

   Example usage:
   \code
   QApplication a(argc, argv);

   CGetOpt opt;
   \endcode

   This constructor is probably the most convenient one to use in a
   regular Qt application. Note that QApplication may already have
   removed Qt (or X11) specific arguments. Also see
   QApplication::argv() and QApplication::argc().
 */
CGetOpt::CGetOpt()
{
    if ( !qApp )
    qFatal( "CGetOpt: requires a QApplication instance to be constructed first" );
    args = QCoreApplication::arguments().mid(1);
    init(0, 0);
}

/**
   \internal
 */

/**
   Construct a command line parser from the array \a argv of string
   pointers with the size \a argc. Those parameters have the form
   typically found in the \c main() function. That means that you can
   simply pass on the arguments specified by the user of your
   application.

   Example usage:

   \code
   int main(int argc, char **argv) {
       CGetOpt opt(argc, argv);
       ...
   }
   \endcode
 */
CGetOpt::CGetOpt( int argc, char *argv[] )
{
    init( argc, argv );
}

/**
   Construct a command line parser from the arguments specified in the
   list of arguments \a a. This constructor is convenient in those
   cases where you want to parse a command line assembled on-the-fly
   instead of relying on the \c argc and \c arg parameters passed to
   the \c main() function.
 */
CGetOpt::CGetOpt( const QStringList &a )
    : args( a )
{
    init( 0, 0 );
}

/**
   \internal
*/
void CGetOpt::init( int argc, char *argv[], int offset )
{
    numReqArgs = numOptArgs = 0;
    currArg = 1; // appname is not part of the arguments
    if ( argc ) {
    // application name
    aname = QFileInfo( QString::fromUtf8( argv[0] ) ).fileName();
    // arguments
    for ( int i = offset; i < argc; ++i )
        args.append( QString::fromUtf8( argv[i] ) );
    }
}

/**
   \fn bool CGetOpt::parse()

   Parse the command line arguments specified in the constructor under
   the conditions set by the various \c add*() functions. On success,
   the given variable reference will be initialized with their
   respective values and true will be returned. Returns false
   otherwise.

   In the future there'll be a way to retrieve an error message. In
   the current version the message will be printed to \c stderr.
*/

/**
   \internal
*/
bool CGetOpt::parse( bool untilFirstSwitchOnly )
{
    //    qDebug( "parse(%s)", args.join( QString( "," ) ).ascii() );
    // push all arguments as we got them on a stack
    // more pushes might following when parsing condensed arguments
    // like --key=value.
    QStack<QString> stack;
    {
    if (!args.isEmpty()) {
        QStringList::const_iterator it = args.end();
        --it;
        const QStringList::const_iterator begin = args.begin();
        do {
        stack.push( *it );
        } while ( it-- != begin );
    }
    }

    const OptionConstIterator obegin = options.begin();
    const OptionConstIterator oend = options.end();
    enum { StartState, ExpectingState, OptionalState } state = StartState;
    Option currOpt;
    enum TokenType { LongOpt, ShortOpt, Arg, End } t, currType = End;
    bool extraLoop = true; // we'll do an extra qRound. fake an End argument
    while ( !stack.isEmpty() || extraLoop ) {
    QString a;
    QString origA;
    // identify argument type
    if ( !stack.isEmpty() ) {
        a = stack.pop();
        currArg++;
        origA = a;
        //	    qDebug( "popped %s", a.ascii() );
        if ( a.startsWith( QString::fromLatin1( "--" ) ) ) {
        // recognized long option
        a = a.mid( 2 );
        if ( a.isEmpty() ) {
            qWarning( "'--' feature not supported, yet" );
            exit( 2 );
        }
        t = LongOpt;
        // split key=value style arguments
        int equal = a.indexOf( '=' );
        if ( equal >= 0 ) {
            stack.push( a.mid( equal + 1 ) );
            currArg--;
            a = a.left( equal );
        }
        } else if ( a.length() == 1 ) {
        t = Arg;
        } else if ( a[0] == '-' ) {
#if 0 // compat mode for -long style options
        if ( a.length() == 2 ) {
            t = ShortOpt;
            a = a[1];
        } else {
            a = a.mid( 1 );
            t = LongOpt;
            // split key=value style arguments
            int equal = a.find( '=' );
            if ( equal >= 0 ) {
            stack.push( a.mid( equal + 1 ) );
            currArg--;
            a = a.left( equal );
            }
        }
#else
        // short option
        t = ShortOpt;
        // followed by an argument ? push it for later processing.
        if ( a.length() > 2 ) {
            stack.push( a.mid( 2 ) );
            currArg--;
        }
        a = a[1];
#endif
        } else {
        t = Arg;
        }
    } else {
        // faked closing argument
        t = End;
    }
    // look up among known list of options
    Option opt;
    if ( t != End ) {
        OptionConstIterator oit = obegin;
        while ( oit != oend ) {
        const Option &o = *oit;
        if ( ( t == LongOpt && a == o.lname ) || // ### check state
             ( t == ShortOpt && a[0].unicode() == o.sname ) ) {
            opt = o;
            break;
        }
        ++oit;
        }
        if ( t == LongOpt && opt.type == OUnknown ) {
        if ( currOpt.type != OVarLen ) {
            qWarning( "Unknown option --%s", (const char *)a.toLatin1() );
            return false;
        } else {
            // VarLength options support arguments starting with '-'
            t = Arg;
        }
        } else if ( t == ShortOpt && opt.type == OUnknown ) {
        if ( currOpt.type != OVarLen ) {
            qWarning( "Unknown option -%c", a[0].unicode() );
            return false;
        } else {
            // VarLength options support arguments starting with '-'
            t = Arg;
        }
        }

    } else {
        opt = Option( OEnd );
    }

    // interpret result
    switch ( state ) {
    case StartState:
        if ( opt.type == OSwitch ) {
        setSwitch( opt );
        setOptions.insert( opt.lname, 1 );
        setOptions.insert( QString( QChar( opt.sname ) ), 1 );
        } else if ( opt.type == OArg1 || opt.type == ORepeat ) {
        state = ExpectingState;
        currOpt = opt;
        currType = t;
        setOptions.insert( opt.lname, 1 );
        setOptions.insert( QString( QChar( opt.sname ) ), 1 );
        } else if ( opt.type == OOpt || opt.type == OVarLen ) {
        state = OptionalState;
        currOpt = opt;
        currType = t;
        setOptions.insert( opt.lname, 1 );
        setOptions.insert( QString( QChar( opt.sname ) ), 1 );
        } else if ( opt.type == OEnd ) {
        // we're done
        } else if ( opt.type == OUnknown && t == Arg ) {
        if ( numReqArgs > 0 ) {
            if ( reqArg.stringValue->isNull() ) { // ###
            *reqArg.stringValue = a;
            } else {
            qWarning( "Too many arguments" );
            return false;
            }
        } else if ( numOptArgs = 9999 ) {
            optArg.listValue->append( a );
        } else if ( numOptArgs > 0 ) {
            if ( optArg.stringValue->isNull() ) { // ###
            *optArg.stringValue = a;
            } else {
            qWarning( "Too many arguments" );
            return false;
            }
        }
        } else {
        qFatal( "unhandled StartState case %d",  opt.type );
        }
        break;
    case ExpectingState:
        if ( t == Arg ) {
        if ( currOpt.type == OArg1 ) {
            *currOpt.stringValue = a;
            state = StartState;
        } else if ( currOpt.type == ORepeat ) {
            currOpt.listValue->append( a );
            state = StartState;
        } else {
            abort();
        }
        } else {
        QString n = currType == LongOpt ?
                currOpt.lname : QString( QChar( currOpt.sname ) );
        qWarning( "Expected an argument after '%s' option", (const char *)n.toLatin1() );
        return false;
        }
        break;
    case OptionalState:
        if ( t == Arg ) {
        if ( currOpt.type == OOpt ) {
            *currOpt.stringValue = a;
            state = StartState;
        } else if ( currOpt.type == OVarLen ) {
            currOpt.listValue->append( origA );
            // remain in this state
        } else {
            abort();
        }
        } else {
        // optional argument not specified
        if ( currOpt.type == OOpt )
            *currOpt.stringValue = currOpt.def;
        if ( t != End ) {
            // re-evaluate current argument
            stack.push( origA );
            currArg--;
        }
        state = StartState;
        }
        break;
    }

    if ( untilFirstSwitchOnly && opt.type == OSwitch )
        return true;

    // are we in the extra loop ? if so, flag the final end
    if ( t == End )
        extraLoop = false;
    }

    if ( numReqArgs > 0 && reqArg.stringValue->isNull() ) {
    qWarning( "Lacking required argument" );
    return false;
    }

    return true;
}

/**
   \internal
*/
void CGetOpt::addOption( Option o )
{
    // ### check for conflicts
    options.append( o );
}

/**
   Adds a switch with the long name \a lname. If the switch is found
   during parsing the bool \a *b will bet set to true. Otherwise the
   bool will be initialized to false.

   Example:

   \code
   CGetOpt opt;
   bool verbose;
   opt.addSwitch("verbose", &verbose);
   \endcode

   The boolean flag \c verbose will be set to true if \c --verbose has
   been specified in the command line; false otherwise.
*/
void CGetOpt::addSwitch( const QString &lname, bool *b )
{
    Option opt( OSwitch, 0, lname );
    opt.boolValue = b;
    addOption( opt );
    // ### could do all inits at the beginning of parse()
    *b = false;
}

/**
   Adds a switch with the long name \a lname and the short name \a sname.
   If the switch is found
   during parsing the bool \a *b will bet set to true. Otherwise the
   bool will be initialized to false.

   Example:

   \code
   CGetOpt opt;
   bool verbose;
   opt.addSwitch('v', "verbose", &verbose);
   \endcode

   The boolean flag \c verbose will be set to true if either \c -v or \c --verbose has
   been specified in the command line; false otherwise.
*/
void CGetOpt::addSwitch( char sname, const QString &lname, bool *b )
{
    Option opt( OSwitch, sname, lname );
    opt.boolValue = b;
    addOption( opt );
    // ### could do all inits at the beginning of parse()
    *b = false;
}

/**
   \internal
*/
void CGetOpt::setSwitch( const Option &o )
{
    assert( o.type == OSwitch );
    *o.boolValue = true;
}

/**
   Registers an option with the short name \a s and long name \a l to
   the parser. If this option is found during parsing the value will
   be stored in the string pointed to by \a v. By default \a *v will
   be initialized to \c QString::null.
*/
void CGetOpt::addOption( char s, const QString &l, QString *v )
{
    Option opt( OArg1, s, l );
    opt.stringValue = v;
    addOption( opt );
    *v = QString::null;
}

/**
   Registers a long option \a l that can have a variable number of
   corresponding value parameters. As there currently is no way to
   tell the end of the value list the only sensible use of this option
   is at the end of the command line.

   Example:

   \code
   QStringList args;
   opt.addVarLengthOption("exec", &args);
   \endcode

   Above code will lead to "-f" and "test.txt" being stored in \a args
   upon

   \code
   myapp --exec otherapp -f test.txt
   \endcode
 */
void CGetOpt::addVarLengthOption( const QString &l, QStringList *v )
{
    Option opt( OVarLen, 0, l );
    opt.listValue = v;
    addOption( opt );
    *v = QStringList();
}

/**
   Registers an option with the short name \a s that can be specified
   repeatedly in the command line. The option values will be stored in
   the list pointed to by \a v. If no \a s option is found \a *v will
   remain at its default value of an empty QStringList instance.

   Example:

   To parse the \c -I options in a command line like
   \code
   myapp -I/usr/include -I/usr/local/include
   \endcode

   you can use code like this:

   \code
   CGetOpt opt;
   QStringList includes;
   opt.addRepeatableOption('I', &includes);
   opt.parse();
   \endcode
 */
void CGetOpt::addRepeatableOption( char s, QStringList *v )
{
    Option opt( ORepeat, s, QString::null );
    opt.listValue = v;
    addOption( opt );
    *v = QStringList();
}

/**
   Registers an option with the long name \a l that can be specified
   repeatedly in the command line.

   \sa addRepeatableOption( char, QStringList* )
 */
void CGetOpt::addRepeatableOption( const QString &l, QStringList *v )
{
    Option opt( ORepeat, 0, l );
    opt.listValue = v;
    addOption( opt );
    *v = QStringList();
}

/**
   Adds a long option \a l that has an optional value parameter. If
   the value is not specified by the user it will be set to \a def.

   Example:

   \code
   CGetOpt opt;
   QString file;
   opt.addOptionalOption("dump", &file, "<stdout>");
   \endcode

   \sa addOption
 */
void CGetOpt::addOptionalOption( const QString &l, QString *v,
                                const QString &def )
{
    addOptionalOption( 0, l, v, def );
}

/**
   Adds a short option \a s that has an optional value parameter. If
   the value is not specified by the user it will be set to \a def.
 */
void CGetOpt::addOptionalOption( char s, const QString &l,
                QString *v, const QString &def )
{
    Option opt( OOpt, s, l );
    opt.stringValue = v;
    opt.def = def;
    addOption( opt );
    *v = QString::null;
}

/**
   Registers a required command line argument \a name. If the argument
   is missing parse() will return false to indicate an error and \a *v
   will remain with its default QString::null value. Otherwise \a *v
   will be set to the value of the argument.

   Example:

   To accept simple arguments like

   \code
   myeditor letter.txt
   \endcode

   use a call like:

   \code
   QString &file;
   opt.addArgument("file", &file);
   \endcode

   Note: the \a name parameter has a rather descriptive meaning for
   now. It might be used for generating a usage or error message in
   the future. Right now, the only current use is in relation with the
   isSet() function.
 */
void CGetOpt::addArgument( const QString &name, QString *v )
{
    Option opt( OUnknown, 0, name );
    opt.stringValue = v;
    reqArg = opt;
    ++numReqArgs;
    *v = QString::null;
}

/**
   Registers an optional command line argument \a name. For a more
   detailed description see the addArgument() documentation.

 */
void CGetOpt::addOptionalArgument( const QString &name, QString *v )
{
    if (numOptArgs == 9999) {
    qWarning( "Cannot use both, addOptionalArguments and addOptionalArgument" );
    }
    else {
    Option opt( OUnknown, 0, name );
    opt.stringValue = v;
    optArg = opt;
    ++numOptArgs;
    *v = QString::null;
    }
}

/**
   Registers optional command line arguments \a name.  All remaining
   positional arguments on the command line will be assigned to the
   list.

   For a more detailed description see the addArgument()
   documentation.

 */
void CGetOpt::addOptionalArguments( const QString &name, QStringList *v )
{
    if (numOptArgs != 0) {
    qWarning( "Cannot use both, addOptionalArguments and addOptionalArgument" );
    }
    else {
    Option opt( OUnknown, 0, name );
    opt.listValue = v;
    optArg = opt;
    numOptArgs = 9999;	// well, many of them
    *v = QStringList();
    }
}

/**
   Returns true if the (long) option or switch \a name has been found
   in the command line; returns false otherwise. Leading hyphens are
   not part of the name.

   As the set/not set decision can also be made depending on the value
   of the variable reference used in the respective \c add*() call
   there's generally little use for this function.
*/

bool CGetOpt::isSet( const QString &name ) const
{
    return setOptions.find( name ) != setOptions.end();
}

/**
   \fn int CGetOpt::currentArgument() const
   \internal
*/
