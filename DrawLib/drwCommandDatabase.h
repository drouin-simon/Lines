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
	void ExecuteAll();
	
	bool IsModified() { return Modified; }
	void Clear();
	int GetNumberOfCommands() { return Commands.size(); }
	void PushCommand( drwCommand::s_ptr command );
	
signals:
	
	void CommandRead( drwCommand::s_ptr command );
	
protected:
	
	QMutex m_commandMutex;
	typedef std::vector< drwCommand::s_ptr > container;
	container Commands;
	
	bool Modified;
	bool Modifiable;
	
	static const quint32 MagicNumber;
	
};

#endif
