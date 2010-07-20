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
	
signals:
	
	void CommandRead( drwCommand::s_ptr command );
	
public slots:
	
	void PushCommand( drwCommand::s_ptr command );
	
protected:
	
	QMutex m_commandMutex;
	typedef std::vector< drwCommand::s_ptr > container;
	container Commands;
	
	bool Modified;
	bool Modifiable;
	
	static const quint32 MagicNumber;
	
};

#endif