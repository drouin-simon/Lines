#include "drwCommandDatabase.h"
#include <QFile>
#include <QDataStream>

const quint32 drwCommandDatabase::MagicNumber = (quint32)0x44524157;  // "DRAW"

drwCommandDatabase::drwCommandDatabase( QObject * parent )
: QObject( parent )
{
	Modified = false;
	Modifiable = true;
}

bool drwCommandDatabase::Read( const char * filename )
{
	// Open file and stream
	QFile file( filename );
	if( ! file.open(QIODevice::ReadOnly) )
		return false;
	QDataStream in(&file);
	
	// clean-up previous stack of commands
	m_commandMutex.lock();
	Commands.clear();
	
	// Try to read the magic number
	quint32 magic = 0;
	in >> magic;
	if( magic != MagicNumber )
	{
		file.close();
		return false;
	}
	
	// read all commands
	Modifiable = false;
	while( !in.atEnd() ) 
	{
		drwCommand::s_ptr command = drwCommand::ReadHeader( in );
		if( command )
		{
			command->Read( in );
		}
		else
		{
			file.close();
			return false;
		}
		Commands.push_back( command );
	}
	Modifiable = true;
	m_commandMutex.unlock();
	
	file.close();
	return true;
}


bool drwCommandDatabase::Write( const char * filename )
{
	// open file and stream
	QFile file(filename);
	if( !file.open(QIODevice::WriteOnly) )
		return false;
	QDataStream out(&file);
	
	// write a magic number
	out << MagicNumber;
	
	// write all commands (and their class id)
	m_commandMutex.lock();
	container::iterator it = Commands.begin();
	for( ; it != Commands.end(); ++it )
	{
		if( !((*it)->Write( out ) ) )
			return false;
	}
	m_commandMutex.unlock();
	
	// close file
	file.close();
	
	// file is written, not modified anymore
	Modified = false;
	
	return true;
}

void drwCommandDatabase::Clear()
{
	m_commandMutex.lock();
	Commands.clear();
	Modified = false;
	m_commandMutex.unlock();
}

void drwCommandDatabase::PushCommand( drwCommand::s_ptr command )
{
	m_commandMutex.lock();
	if( Modifiable )
	{
		// Copy the command for thread safety
		drwCommand::s_ptr commandCopy = command->Clone();
		Modified = true;
        Commands.push_back( commandCopy );
	}
	m_commandMutex.unlock();
}

void drwCommandDatabase::LockDb( bool l )
{
    if( l )
        m_commandMutex.lock();
    else
        m_commandMutex.unlock();
}

drwCommand::s_ptr drwCommandDatabase::GetCommand( int index )
{
    drwCommand::s_ptr com = Commands[index]->Clone();
    return com;
}
