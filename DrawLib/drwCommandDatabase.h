#ifndef __drwCommandDatabase_h_
#define __drwCommandDatabase_h_

#include <QObject>
#include <QMutex>
#include "drwCommand.h"
#include <vector>

class drwCommandDatabase : public QObject
{
	
	Q_OBJECT
	
public:
	
	drwCommandDatabase( QObject * parent );
	~drwCommandDatabase() {}
	
	bool Read( const char * filename );
	bool Write( const char * filename );
	
	bool IsModified() { return Modified; }
	void Clear();
	void PushCommand( drwCommand::s_ptr command );

    // Client access to the commands in the db
    void LockDb( bool l );
    int GetNumberOfCommands() { return Commands.size(); }
    drwCommand::s_ptr GetCommand( int index );
	
protected:
	
	QMutex m_commandMutex;
	typedef std::vector< drwCommand::s_ptr > container;
	container Commands;
	
	bool Modified;
	
	static const quint32 MagicNumber;
	
};

#endif
