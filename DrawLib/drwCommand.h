#ifndef __drwCommand_h_
#define __drwCommand_h_

#include "macros.h"
#include "SharedPtrMacro.h"

class QDataStream;
class QTextStream;

enum drwCommandId { drwIdSetFrameCommand, drwIdMouseCommand, drwIdSetPersistenceCommand, drwIdServerInitialCommand };

class drwCommand
{
	
public:
	
	SharedPtrMacro(drwCommand);
	
	drwCommand() : m_userId(m_defaultUserId) {}
	drwCommand( drwCommand & other ) : m_userId( other.m_userId ) {}
	virtual ~drwCommand() {}
	virtual s_ptr Clone() = 0; 
	
	virtual drwCommandId GetCommandId() = 0;
	int GetUserId() { return m_userId; }
	void SetUserId( int id ) { m_userId = id; }
	
	// Reading ( the XXXSize() functions are used by clients to determine if there is enough data in a stream to read commands)
	static int HeaderSize();
	static s_ptr ReadHeader( QDataStream & in );
	virtual int BodySize() = 0;
	virtual void Read( QDataStream & in ) = 0;
	
	// Writing
	bool Write( QDataStream & in );
	virtual void Write( QTextStream & in ) = 0;
	
	virtual bool Concatenate( drwCommand * other ) { return false; }
	
protected:

	static s_ptr InstanciateSubclass( drwCommandId id );
	virtual bool WriteImpl( QDataStream & stream ) = 0;
	
	int m_userId;
	
private:

	static const int m_defaultUserId;
};

class drwSetFrameCommand : public drwCommand
{
	
public:
	
	drwSetFrameCommand() { NewFrame = 0; }
	drwSetFrameCommand( drwSetFrameCommand & other ) : drwCommand( other ), NewFrame(other.NewFrame) {}
	virtual ~drwSetFrameCommand() {}
	virtual s_ptr Clone() { s_ptr newCom( new drwSetFrameCommand( *this ) ); return newCom; }

	drwCommandId GetCommandId() { return drwIdSetFrameCommand; }
	int BodySize();
	void Read( QDataStream & stream );
	bool WriteImpl( QDataStream & stream );
	void Write( QTextStream & stream );
	
	SetMacro(NewFrame,int);
	GetMacro(NewFrame,int);
	
	virtual bool Concatenate( drwCommand * other );
	
protected:
	
	int NewFrame;
	
};

class drwSetPersistenceCommand : public drwCommand
{
	
public:
	
	drwSetPersistenceCommand() : Persistence(0) { }
	drwSetPersistenceCommand( drwSetPersistenceCommand & other ) : drwCommand( other ), Persistence( other.Persistence ) {}
	virtual ~drwSetPersistenceCommand() {}
	virtual s_ptr Clone() { s_ptr newCom( new drwSetPersistenceCommand( *this ) ); return newCom; }
	
	drwCommandId GetCommandId() { return drwIdSetPersistenceCommand; }
	int BodySize();
	void Read( QDataStream & stream );
	bool WriteImpl( QDataStream & stream );
	void Write( QTextStream & stream );
	
	SetMacro(Persistence,int);
	GetMacro(Persistence,int);
	
	virtual bool Concatenate( drwCommand * other );
	
protected:
	
	int Persistence;
	
};

class drwServerInitialCommand : public drwCommand
{

public:

	drwServerInitialCommand()
		: NumberOfCommands( 0 ){}
	drwServerInitialCommand( int nbCommands )
		: NumberOfCommands( nbCommands ){}
	drwServerInitialCommand( drwServerInitialCommand & other )
		: drwCommand( other )
		, NumberOfCommands( other.NumberOfCommands ){}
	virtual ~drwServerInitialCommand() {}
	virtual s_ptr Clone() { s_ptr newCom( new drwServerInitialCommand( *this ) ); return newCom; }

	drwCommandId GetCommandId() { return drwIdServerInitialCommand; }
	int BodySize();
	void Read( QDataStream & stream );
	bool WriteImpl( QDataStream & stream );
	void Write( QTextStream & stream );

	SetMacro(NumberOfCommands,int);
	GetMacro(NumberOfCommands,int);

protected:

	int NumberOfCommands;
};

class drwMouseCommand : public drwCommand
{
	
public:
	
	enum MouseCommandType { Press, Release, Move };
	
	drwMouseCommand();
	drwMouseCommand( MouseCommandType commandType, double x, double y, double z, int xTilt, int yTilt, double pressure, double rotation, double tangentialPressure );
	drwMouseCommand( drwMouseCommand & );
	virtual ~drwMouseCommand() {}
	virtual s_ptr Clone() { s_ptr newCom( new drwMouseCommand( *this ) ); return newCom; }
	
	virtual drwCommandId GetCommandId() { return drwIdMouseCommand; }
	int BodySize();
	void Read( QDataStream & stream );
	bool WriteImpl( QDataStream & stream );
	
	MouseCommandType GetType() { return m_type; }
	double X() { return m_x; }
	double Y() { return m_y; }
	double Z() { return m_z; }
	int XTilt() { return m_xTilt; }
	int YTilt() { return m_yTilt; }
	double Pressure() { return m_pressure; }
	double Rotation() { return m_rotation; }
	double TangentialPressure() { return m_tangentialPressure; }
	
protected:
	
	void Write( QTextStream & stream );
	
	MouseCommandType m_type;
	double m_x;
	double m_y;
	double m_z;
	int m_xTilt;
	int m_yTilt;
	double m_pressure;
	double m_rotation;
	double m_tangentialPressure;
};


#endif
