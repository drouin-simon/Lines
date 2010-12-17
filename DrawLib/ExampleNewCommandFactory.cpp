#include <iostream>
#include <map>

class drwCommand
{

public:

    virtual int Id() = 0;
    virtual drwCommand * Clone() = 0;

protected:

};

class drwCommandFactory
{
public:
    static int RegisterClass( drwCommand * t )
    {
        //assert( m_register[t->Id()] == 0 );  // shouldn't be registered already
        m_register[t->Id()] = t;
        return t->Id();
    }
    static drwCommand * Create( int id )
    {
        //assert( m_register[id] != 0 );  // shouldn be registered
        return m_register[id]->Clone();
    }

protected:
    static std::map< int, drwCommand * > m_register;
};

std::map< int, drwCommand * > drwCommandFactory::m_register;


// specific implementation
class drwSetFrameCommand : public drwCommand
{
public:
    virtual drwCommand * Clone()
    {
        return new drwSetFrameCommand(*this);
    }
    drwSetFrameCommand() : m_frame(23) {}
    virtual int Id() { return 1; }  // each class has its own Id
    int m_frame;
};

int id = drwCommandFactory::RegisterClass( new drwSetFrameCommand );



// Example
int main( int argc, char ** argv )
{
    drwCommand * com = drwCommandFactory::Create( 1 );
    std::cout << com->Id() << std::endl;
}
