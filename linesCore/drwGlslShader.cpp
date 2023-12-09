#include "drwGlslShader.h"
#include <iostream>
#include <stdio.h>
#include <stdarg.h>

using namespace std;

drwGlslShader::drwGlslShader() 
	: m_glslShader(0)
	, m_glslVertexShader(0)
	, m_glslProg(0)
	, m_init( false )
{
    m_engine = GraphicsEngineManager::getGraphicsEngine();
}

drwGlslShader::~drwGlslShader() 
{
	Clear();
}

void drwGlslShader::AddShaderFilename( const char * filename )
{
	m_shaderFilenames.push_back( std::string( filename ) );
}

void drwGlslShader::AddShaderMemSource( const char * src )
{
	m_memSources.push_back( std::string( src ) );
}

void drwGlslShader::AddVertexShaderFilename( const char * filename )
{
	m_vertexShaderFilenames.push_back( std::string( filename ) );
}

void drwGlslShader::AddVertexShaderMemSource( const char * src )
{
	m_vertexMemSources.push_back( std::string( src ) );
}

bool drwGlslShader::Init()
{
	// Fresh start
	Clear();
	
	// Load and try compiling vertex shader
	if( m_vertexMemSources.size() != 0 || m_vertexShaderFilenames.size() != 0 )
		if( !m_engine->CreateAndCompileVertexShader( m_glslVertexShader, m_vertexShaderFilenames, m_vertexMemSources ) )
			return false;
	
	// Load and try compiling pixel shader
	if( m_memSources.size() != 0 || m_shaderFilenames.size() != 0 )
		if( !m_engine->CreateAndCompileFragmentShader( m_glslShader, m_shaderFilenames, m_memSources ) )
			return false;
	
	// Check that at least one of the shaders have been compiled
	if( m_glslVertexShader == 0 && m_glslShader == 0 )
		return false;

	// Create program object and attach shader
	m_glslProg = m_engine->createProgram();
	if( m_glslVertexShader )
		m_engine->attachShader( m_glslProg, m_glslVertexShader );
	if( m_glslShader )
		m_engine->attachShader( m_glslProg, m_glslShader );

	// Create program and link shaders
	m_engine->linkProgram(m_glslProg);

	// Error message if program was not link correctly
	//GLint success = 0;

 //   m_engine->getProgramIv( m_glslProg, GL_LINK_STATUS, &success );
 //   if (!success)
 //   {
	//	GLint logLength = 0;
	//	m_engine->getProgramIv(	m_glslProg, GL_INFO_LOG_LENGTH, &logLength );
	//	GLchar * infoLog = new GLchar[ logLength + 1 ];
 //       m_engine->getProgramInfoLog( m_glslProg, logLength, NULL, infoLog );
	//	ReportError( "Error in glsl program linking: \n %s\n", infoLog );
	//	delete [] infoLog;
	//	return false;
 //   }

	m_init = true;
	return true;
}

//bool drwGlslShader::CreateAndCompileShader( unsigned shaderType, unsigned & shaderId, std::vector< std::string > & files, std::vector< std::string > & memSources )
//{
//	// Add sources from files to m_memSources vector
//	for( unsigned i = 0; i < files.size(); ++i )
//	{
//		std::string newSource;
//		if( !LoadOneShaderSource( files[i].c_str(), newSource ) )
//			return false;
//		memSources.push_back( newSource );
//	}
//	
//	// put all the sources in an array of const GLchar*
//	const GLchar ** shaderStringPtr = new const GLchar*[ memSources.size() ];
//	for( unsigned i = 0; i < memSources.size(); ++i )
//	{
//		shaderStringPtr[i] = memSources[i].c_str();
//	}
//	
//	// Create the shader and set its source
//	shaderId = glCreateShader( shaderType );
//    glShaderSource( shaderId, memSources.size(), shaderStringPtr, NULL);
//	
//	delete [] shaderStringPtr;
//	
//	// Compile the shader
//	GLint success = 0;
//	glCompileShader( shaderId );
//    glGetShaderiv( shaderId, GL_COMPILE_STATUS, &success );
//    if (!success)
//    {
//		GLint logLength = 0;
//		glGetShaderiv( shaderId, GL_INFO_LOG_LENGTH, &logLength );
//        GLchar * infoLog = new GLchar[logLength+1];
//        glGetShaderInfoLog( shaderId, logLength, NULL, infoLog);
//		ReportError( "Error in shader complilation: \n %s\n", infoLog );
//		delete [] infoLog;
//        return false;
//    }
//	return true;
//}

bool drwGlslShader::UseProgram( bool use )
{
	bool res = true;
	if( use && m_init )
	{
		m_engine->UseProgram(m_glslProg);
	}
	else
	{
		res = true;
		m_engine->UseProgram(0);
	}
	return res;
}

bool drwGlslShader::SetVariable( const char * name, int value )
{
	bool result = m_engine->SetVariable(m_glslProg, name, value);
	if (!result) ReportError( "Couldn't set shader variable %s.\n", name );
	return result;
}

bool drwGlslShader::SetVariable( const char * name, float value )
{
	bool result = m_engine->SetVariable(m_glslProg, name, value);
	if (!result) ReportError( "Couldn't set shader variable %s.\n", name );
	return result;
}

//bool drwGlslShader::LoadOneShaderSource( const char * filename, std::string & shaderSource )
//{
//	// Open shader file for reading
//	FILE * f = fopen( filename, "rb" );
//	if( !f )
//	{
//		ReportError( "Couldn't open shader file %s\n", filename );
//		return false;
//	}
//
//	// Get file size
//	fseek( f, 0, SEEK_END );
//    long length = ftell( f );
//	fseek( f, 0, SEEK_SET );
//
//	// Read file into shaderSource string
//	char * charShaderSource = new char[ length + 1 ];
//	fread( charShaderSource, 1, length, f );
//	charShaderSource[length] = '\0';
//	shaderSource = charShaderSource;
//	delete [] charShaderSource;
//
//	// close the file
//    fclose( f );
//
//	return true;
//}

void drwGlslShader::Clear()
{
	if( m_glslVertexShader != 0 )
	{
		m_engine->DeleteShader(m_glslVertexShader);
		m_glslVertexShader = 0;
	}
	if( m_glslShader != 0 )
	{
		m_engine->DeleteShader(m_glslShader);
		m_glslShader = 0;
	}
	if( m_glslProg != 0 )
	{
		m_engine->DeleteProgram(m_glslProg);
		m_glslProg = 0;
	}
	m_init = false;
}

void drwGlslShader::ReportError( const char * msg, ... )
{
	va_list args;
	va_start(args, msg);
	vprintf( msg, args );
	va_end(args);
}
