#include "drwCommand.h"
#include <QDataStream>
#include <QTextStream>

//===================================
// drwCommand Implementation
//===================================

const int drwCommand::m_defaultUserId = 0;

drwCommand::s_ptr drwCommand::InstanciateSubclass( drwCommandId id )
{
	s_ptr newCommand;
	switch (id) 
	{
	case drwIdSetFrameCommand:
		newCommand.reset( new drwSetFrameCommand );
		break;
	case drwIdMouseCommand:
		newCommand.reset( new drwMouseCommand );
		break;
	case drwIdLineToolParamsCommand:
		newCommand.reset( new drwLineToolParamsCommand );
		break;
	case drwIdServerInitialCommand:
		newCommand.reset( new drwServerInitialCommand );
		break;
    case drwIdNewSceneCommand:
        newCommand.reset( new drwNewSceneCommand );
        break;
    case drwIdSceneParamsCommand:
        newCommand.reset( new drwSceneParamsCommand );
        break;
	default:
		Q_ASSERT(0); // If we get here, we try to read a command type that is not yet defined.
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
	drwCommand::s_ptr ret = drwCommand::InstanciateSubclass( (drwCommandId)id );
	ret->SetUserId( user );
	return ret;
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
// drwLineToolParamsCommand Implementation
//===================================

drwLineToolParamsCommand::drwLineToolParamsCommand()
	: Color( Vec4(1.0,1.0,1.0,1.0) )
	, BaseWidth( 1.0 )
	, PressureWidth( false )
	, PressureOpacity( false )
	, Fill( false )
    , Erase( false )
	, Persistence(0)
{
}

drwLineToolParamsCommand::drwLineToolParamsCommand( drwLineToolParamsCommand & other )
	: drwCommand( other )
	, Color( other.Color )
	, BaseWidth( other.BaseWidth )
	, PressureWidth( other.PressureWidth )
	, PressureOpacity( other.PressureOpacity )
	, Fill( other.Fill )
    , Erase( other.Erase )
	, Persistence( other.Persistence )
{
}


int drwLineToolParamsCommand::BodySize()
{
    return (4 * sizeof(double) + sizeof(BaseWidth) + sizeof(PressureWidth) + sizeof(PressureOpacity) + sizeof(Fill) + sizeof(Erase) + sizeof(Persistence) );
}

void drwLineToolParamsCommand::Read( QDataStream & stream )
{
	stream >> Color[0];
	stream >> Color[1];
	stream >> Color[2];
	stream >> Color[3];
	stream >> BaseWidth;
	stream >> PressureWidth;
	stream >> PressureOpacity;
	stream >> Fill;
    stream >> Erase;
	stream >> Persistence;
}

bool drwLineToolParamsCommand::WriteImpl( QDataStream & stream )
{
	stream << Color[0];
	stream << Color[1];
	stream << Color[2];
	stream << Color[3];
	stream << BaseWidth;
	stream << PressureWidth;
	stream << PressureOpacity;
	stream << Fill;
    stream << Erase;
	stream << Persistence;
	return true;
}

void drwLineToolParamsCommand::Write( QTextStream & stream )
{
	stream << "Line tool params: ";
	stream << "Color = ( " << Color[0] << ", " << Color[1] << ", " << Color[2] << ", " << Color[3] << "  ";
	stream << "BaseWidth = " << BaseWidth << "  ";
	stream << "PressureWidth = " << PressureWidth << "  ";
	stream << "PressureOpactity = " << PressureOpacity << "  ";
	stream << "Fill = " << Fill << "  ";
    stream << "Erase = " << Erase << " ";
	stream << "Persistence = " << Persistence << "  ";
}

bool drwLineToolParamsCommand::Concatenate( drwCommand * other )
{
	if( other->GetCommandId() == drwIdLineToolParamsCommand )
	{
		drwLineToolParamsCommand * o = dynamic_cast<drwLineToolParamsCommand*>( other );
		Q_ASSERT(o);
		Color = o->Color;
		BaseWidth = o->BaseWidth;
		PressureWidth = o->PressureWidth;
		PressureOpacity = o->PressureOpacity;
		Fill = o->Fill;
        Erase = o->Erase;
		Persistence = o->Persistence;
		return true;
	}
	return false;
}


//===================================
// drwServerInitialCommand Implementation
//===================================

int drwServerInitialCommand::BodySize()
{
    return sizeof(ProtocolVersion) + sizeof(NumberOfCommands);
}

void drwServerInitialCommand::Read( QDataStream & stream )
{
    stream >> ProtocolVersion;
	stream >> NumberOfCommands;
}

bool drwServerInitialCommand::WriteImpl( QDataStream & stream )
{
    stream << ProtocolVersion;
	stream << NumberOfCommands;
	return true;
}

void drwServerInitialCommand::Write( QTextStream & stream )
{
    stream << "ServerInitialCommand: ProtocolVersion = " << ProtocolVersion << " NumberOfCommands = " << NumberOfCommands;
}

//===================================
// drwMouseCommand Implementation
//===================================

drwMouseCommand::drwMouseCommand()
: drwCommand()
, m_type( Press )
, m_x(0.0)
, m_y(0.0)
, m_z(0.0)
, m_xPixel( 0 )
, m_yPixel( 0 )
, m_xTilt(0)
, m_yTilt(0)
, m_pressure(1.0)
, m_rotation(0.0)
, m_tangentialPressure(0.0)
{
}

drwMouseCommand::drwMouseCommand( drwMouseCommand::MouseCommandType commandType,
                                 double x, double y, double z, int xPix, int yPix, int xTilt,
								 int yTilt, double pressure, double rotation,
								 double tangentialPressure ) 
: drwCommand()
, m_type( commandType )
, m_x(x)
, m_y(y)
, m_z(z)
, m_xPixel( xPix )
, m_yPixel( yPix )
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
, m_xPixel( other.m_xPixel )
, m_yPixel( other.m_yPixel )
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
            + sizeof( m_xPixel ) + sizeof( m_yPixel ) \
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
    stream >> m_xPixel;
    stream >> m_yPixel;
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
    stream << m_xPixel;
    stream << m_yPixel;
	stream << m_xTilt;
	stream << m_yTilt;
	stream << m_pressure;
	stream << m_rotation;
	stream << m_tangentialPressure;
	return true;
}

void drwMouseCommand::Write( QTextStream & stream )
{
    stream << "Pos = (" << m_x << ", " << m_y << ", " << m_z << ") Screen Pos = ( " << m_xPixel << ", " <<  m_yPixel << ") Tilt = (" << m_xTilt << ", " << m_yTilt
	<< ")  Pressure = " << m_pressure << "   Rotation = " << m_rotation << "  Tang pressure = " << m_tangentialPressure; 
}

//===================================
// drwNewSceneCommand Implementation
//===================================

int drwNewSceneCommand::BodySize()
{
    return 0;
}

void drwNewSceneCommand::Read( QDataStream & stream )
{
}

bool drwNewSceneCommand::WriteImpl( QDataStream & stream )
{
    return true;
}

void drwNewSceneCommand::Write( QTextStream & stream )
{
    stream << "New Scene";
}

//===================================
// drwSceneParamsCommand Implementation
//===================================

int drwSceneParamsCommand::BodySize()
{
    return sizeof( m_numberOfFrames );
}

void drwSceneParamsCommand::Read( QDataStream & stream )
{
    stream >> m_numberOfFrames;
}

bool drwSceneParamsCommand::WriteImpl( QDataStream & stream )
{
    stream << m_numberOfFrames;
    return true;
}

void drwSceneParamsCommand::Write( QTextStream & stream )
{
    stream << "Number of frames = " << m_numberOfFrames;
}
