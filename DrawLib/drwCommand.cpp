#include "drwCommand.h"
#include <QDataStream>
#include <QTextStream>

//===================================
// drwCommand Implementation
//===================================

drwCommand::s_ptr drwCommand::InstanciateSubclass( drwCommandId id, int user )
{
	s_ptr newCommand;
	switch (id) 
	{
		case drwIdSetFrameCommand:
			newCommand.reset( new drwSetFrameCommand(user) );
			break;
		case drwIdMouseCommand:
			newCommand.reset( new drwMouseCommand(user) );
			break;
		case drwIdSetPersistenceCommand:
			newCommand.reset( new drwSetPersistenceCommand(user) );
			break;
	}
	return newCommand;
}

int drwCommand::HeaderSize() 
{ 
	return sizeof( quint32 ) + sizeof( int ); 
}

drwCommand::s_ptr drwCommand::ReadHeader( QDataStream & in )
{
	quint32 id = 0;
	in >> id;
	int user = 0;
	in >> user;
	return drwCommand::InstanciateSubclass( (drwCommandId)id, user );
}

bool drwCommand::Write( QDataStream & stream )
{
	stream << (quint32)(GetCommandId());
	stream << GetUserId();
	return WriteImpl( stream );
}


//===================================
// drwSetFrameCommand Implementation
//===================================

int drwSetFrameCommand::BodySize()
{
	return sizeof(NewFrame);
}

void drwSetFrameCommand::Read( QDataStream & stream )
{
	stream >> NewFrame;
}


bool drwSetFrameCommand::WriteImpl( QDataStream & stream )
{
	stream << NewFrame;
	return true;
}

void drwSetFrameCommand::Write( QTextStream & stream )
{
	stream << "SetFrame: frame = " << NewFrame;
}

bool drwSetFrameCommand::Concatenate( drwCommand * other )
{
	if( other->GetCommandId() == drwIdSetFrameCommand )
	{
		NewFrame = ((drwSetFrameCommand*)other)->NewFrame;
		return true;
	}
	return false;
}

//===================================
// drwSetPersistenceCommand Implementation
//===================================

int drwSetPersistenceCommand::BodySize()
{
	return sizeof(Persistence);
}

void drwSetPersistenceCommand::Read( QDataStream & stream )
{
	stream >> Persistence;
}

bool drwSetPersistenceCommand::WriteImpl( QDataStream & stream )
{
	stream << Persistence;
	return true;
}

void drwSetPersistenceCommand::Write( QTextStream & stream )
{
	stream << "SetPersistence: persistence = " << Persistence;
}

bool drwSetPersistenceCommand::Concatenate( drwCommand * other )
{
	if( other->GetCommandId() == drwIdSetPersistenceCommand )
	{
		Persistence = ((drwSetPersistenceCommand*)other)->Persistence;
		return true;
	}
	return false;
}


//===================================
// drwMouseCommand Implementation
//===================================

drwMouseCommand::drwMouseCommand( int userId )
: drwCommand(userId)
, m_type( Press )
, m_x(0.0)
, m_y(0.0)
, m_z(0.0)
, m_xTilt(0)
, m_yTilt(0)
, m_pressure(1.0)
, m_rotation(0.0)
, m_tangentialPressure(0.0)
{
}

drwMouseCommand::drwMouseCommand( int userId, drwMouseCommand::MouseCommandType commandType, 
								 double x, double y, double z, int xTilt,
								 int yTilt, double pressure, double rotation,
								 double tangentialPressure ) 
: drwCommand(userId)
, m_type( commandType )
, m_x(x)
, m_y(y)
, m_z(z)
, m_xTilt(xTilt)
, m_yTilt(yTilt)
, m_pressure(pressure)
, m_rotation(rotation)
, m_tangentialPressure(tangentialPressure)
{}

drwMouseCommand::drwMouseCommand( drwMouseCommand & other )
: drwCommand( other )
, m_type( other.m_type )
, m_x( other.m_x )
, m_y( other.m_y )
, m_z( other.m_z )
, m_xTilt( other.m_xTilt )
, m_yTilt( other.m_yTilt )
, m_pressure( other.m_pressure )
, m_rotation( other.m_rotation )
, m_tangentialPressure( other.m_tangentialPressure )
{
}

int drwMouseCommand::BodySize()
{
	return sizeof(m_type) + sizeof(m_x) + sizeof(m_y) + sizeof(m_z) \
		+ sizeof(m_xTilt) + sizeof( m_yTilt ) + sizeof(m_pressure) \
		+ sizeof(m_rotation) + sizeof( m_tangentialPressure );
}

void drwMouseCommand::Read( QDataStream & stream )
{
	quint32 typeInt;
	stream >> typeInt;
	m_type = (MouseCommandType)typeInt;
	stream >> m_x;
	stream >> m_y;
	stream >> m_z;
	stream >> m_xTilt;
	stream >> m_yTilt;
	stream >> m_pressure;
	stream >> m_rotation;
	stream >> m_tangentialPressure;
}

bool drwMouseCommand::WriteImpl( QDataStream & stream )
{
	stream << (quint32)m_type;
	stream << m_x;
	stream << m_y;
	stream << m_z;
	stream << m_xTilt;
	stream << m_yTilt;
	stream << m_pressure;
	stream << m_rotation;
	stream << m_tangentialPressure;
	return true;
}

void drwMouseCommand::Write( QTextStream & stream )
{
	stream << "Pos = (" << m_x << ", " << m_y << ", " << m_z << ")   Tilt = (" << m_xTilt << ", " << m_yTilt 
	<< ")  Pressure = " << m_pressure << "   Rotation = " << m_rotation << "  Tang pressure = " << m_tangentialPressure; 
}
