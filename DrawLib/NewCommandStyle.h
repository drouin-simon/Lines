#ifndef __NewCommandStyle_h_
#define __NewCommandStyle_h_

enum drwCommandTypes { drwSetFrameCommandType, drwMouseCommandType, drwSetPersistenceCommandType };

class drwCommand
{
	
public:
	
	drwCommand( int user ) : m_user( user ) {}
	~drwCommand() {}
	
	virtual drwCommandTypes GetType() = 0;
	virtual int GetUser() { return m_user; }
	virtual void Read( QDataStream & stream ) = 0;
	virtual void Write( QDataStream & stream ) = 0;
	
protected:
	
	int m_user;

};

template < drwCommandTypes CommandType, class ContentType >
class drwCommandImpl : public drwCommand
{
	
public:
	
	drwCommandImpl( ContentType, int user ) : drwCommand( user )
	drwCommandTypes GetType() { return CommandType; }
	
protected:
	
	ContentType	m_content;
	drwCommandTypes m_type;
	int m_size;
	
};


// SetFrame command
struct drwSetFrameCommandContent
{
	int CurrentFrame;
};
typedef drwCommandImpl< drwSetFrameCommandType, SetFrameCommandContent > drwSetFrameCommand;

// Usage
/*
int main()
{
	drwSetFrameCommandContent content;
	content.CurrentFrame = 3;
	drwSetFrameCommand command( content, user );
}
*/
#endif