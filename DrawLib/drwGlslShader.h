#ifndef __drwGlslShader_h_
#define __drwGlslShader_h_

#include <string>
#include <vector>

class drwGlslShader
{

public:

	drwGlslShader();
	~drwGlslShader();

	void AddShaderFilename( const char * filename );
	void AddShaderMemSource( const char * src );
	bool Init();
	bool UseProgram( bool use );
	bool SetVariable( const char * name, int value );
	bool SetVariable( const char * name, float value );

protected:

	bool LoadOneShaderSource( const char * filename, std::string & shaderSource );
	void Clear();
	void ReportError( const char * msg, ... );

	std::vector< std::string > m_shaderFilenames;
	std::vector< std::string > m_memSources;
	unsigned m_glslShader;
	unsigned m_glslProg;
	bool m_init;

};

#endif