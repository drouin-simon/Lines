#ifndef __drwGlslShader_h_
#define __drwGlslShader_h_

#include "../GraphicsEngine/include/GraphicsEngineManager.h"

#include <string>
#include <vector>

class IGraphicsEngine;

class drwGlslShader
{
private:
	IGraphicsEngine* m_engine;

public:

	drwGlslShader();
	~drwGlslShader();

	void AddShaderFilename( const char * filename );
	void AddShaderMemSource( const char * src );
	void AddVertexShaderFilename( const char * filename );
	void AddVertexShaderMemSource( const char * src );
	
	bool Init();
	bool UseProgram( bool use );
	bool SetVariable( const char * name, int value );
	bool SetVariable( const char * name, float value );

protected:

	bool CreateAndCompileShader( unsigned shaderType, unsigned & shaderId, std::vector< std::string > & files, std::vector< std::string > & memSources );
	bool LoadOneShaderSource( const char * filename, std::string & shaderSource );
	void Clear();
	void ReportError( const char * msg, ... );

	std::vector< std::string > m_shaderFilenames;
	std::vector< std::string > m_memSources;
	std::vector< std::string > m_vertexShaderFilenames;
	std::vector< std::string > m_vertexMemSources;
	unsigned m_glslShader;
	unsigned m_glslVertexShader;
	unsigned m_glslProg;
	bool m_init;

};

#endif