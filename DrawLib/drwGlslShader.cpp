#include "drwGlslShader.h"
#include "IncludeGLee.h"
#include "IncludeGl.h"
#include <iostream>
#include <stdio.h>
#include <stdarg.h>

using namespace std;

drwGlslShader::drwGlslShader() 
	: m_glslShader(0)
	, m_glslProg(0)
	, m_init( false )
{
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

bool drwGlslShader::Init()
{
	GLint success = 0;

	// Fresh start
	Clear();

	// Add sources from files to m_memSources vector
	for( unsigned i = 0; i < m_shaderFilenames.size(); ++i )
	{
		std::string newSource;
		if( !LoadOneShaderSource( m_shaderFilenames[i].c_str(), newSource ) )
			return false;
		m_memSources.push_back( newSource );
	}
	
	// put all the sources in an array of const GLchar*
	const GLchar ** shaderStringPtr = new const GLchar*[ m_memSources.size() ];
	for( unsigned i = 0; i < m_memSources.size(); ++i )
	{
		shaderStringPtr[i] = m_memSources[i].c_str();
	}
	
	// Create the shader and set its source
	m_glslShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(m_glslShader, m_memSources.size(), shaderStringPtr, NULL);

	// Compile the shader
	glCompileShader(m_glslShader);
    glGetShaderiv(m_glslShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
		GLint logLength = 0;
		glGetShaderiv(	m_glslShader, GL_INFO_LOG_LENGTH, &logLength );
        GLchar * infoLog = new GLchar[logLength+1];
        glGetShaderInfoLog( m_glslShader, logLength, NULL, infoLog);
		ReportError( "Error in fragment shader complilation: \n %s\n", infoLog );
		delete [] infoLog;
        return false;
    }

	// Create program object and attach shader
	m_glslProg = glCreateProgram();
	glAttachShader( m_glslProg, m_glslShader );

	// Link program
    glLinkProgram( m_glslProg );
    glGetProgramiv( m_glslProg, GL_LINK_STATUS, &success);
    if (!success)
    {
		GLint logLength = 0;
		glGetProgramiv(	m_glslProg, GL_INFO_LOG_LENGTH, &logLength );
		GLchar * infoLog = new GLchar[logLength+1];
        glGetProgramInfoLog(m_glslProg, logLength, NULL, infoLog);
		ReportError( "Error in glsl program linking: \n %s\n", infoLog );
		delete [] infoLog;
		return false;
    }

	m_init = true;
	return true;
}

bool drwGlslShader::UseProgram( bool use )
{
	bool res = true;
	if( use && m_init )
	{
		glUseProgram( m_glslProg );
	}
	else
	{
		res = true;
		glUseProgram( 0 );
	}
	return res;
}

bool drwGlslShader::SetVariable( const char * name, int value )
{
	int location = glGetUniformLocation( m_glslProg, name );
	if( location != -1 )
	{
		glUniform1iARB( location, value );
		return true;
	}
	ReportError( "Couldn't set shader variable %s.\n", name );
	return false;
}

bool drwGlslShader::SetVariable( const char * name, float value )
{
	int location = glGetUniformLocation( m_glslProg, name );
	if( location != -1 )
	{
		glUniform1fARB( location, value );
		return true;
	}
	ReportError( "Couldn't set shader variable %s.\n", name );
	return false;
}

bool drwGlslShader::LoadOneShaderSource( const char * filename, std::string & shaderSource )
{
	// Open shader file for reading
	FILE * f = fopen( filename, "rb" );
	if( !f )
	{
		ReportError( "Couldn't open shader file %s\n", filename );
		return false;
	}

	// Get file size
	fseek( f, 0, SEEK_END );
    long length = ftell( f );
	fseek( f, 0, SEEK_SET );

	// Read file into shaderSource string
	char * charShaderSource = new char[ length + 1 ];
	fread( charShaderSource, 1, length, f );
	charShaderSource[length] = '\0';
	shaderSource = charShaderSource;
	delete [] charShaderSource;

	// close the file
    fclose( f );

	return true;
}

void drwGlslShader::Clear()
{
	if( m_glslShader != 0 )
	{
		glDeleteShader( m_glslShader );
		m_glslShader = 0;
	}
	if( m_glslProg != 0 )
	{
		glDeleteProgram( m_glslProg );
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
