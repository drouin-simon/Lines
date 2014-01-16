#include "SoundGenerator.h"
#include "IncludeGLee.h"
#include "IncludeGl.h"
#include "RtAudio.h"
#include <math.h>
#include "drwDrawableTexture.h"
#include "drwGlslShader.h"
#include <cstring>

using namespace std;

struct AudioParams
{
    void * outputBuffer;
    void * inputBuffer;
    unsigned int nbFrames;
    double streamTime;
    RtAudioStreamStatus status;
};

// Sample format
#define FORMAT RTAUDIO_FLOAT32

SoundGenerator::SoundGenerator()
{
    m_audio = 0;
    m_soundGenerationTarget = 0;
    m_soundGenerationShader = 0;
    m_minFrequency = 20.0;
    m_maxFrequency = 5000.0;
    m_nbFrequencies = 300;
    m_nextBufferIndex = 0;
    m_soundBufferSize = 0;
    m_soundBuffer = 0;
    m_deviceIndex = 1;
    m_nbChannels = 2;
    m_sampleFreq = 44100;
    m_masterVol = 0.5;
    m_lastTime = 0.0;
}

SoundGenerator::~SoundGenerator()
{
    Stop();
    delete m_soundGenerationTarget;
    delete m_soundGenerationShader;
    delete m_soundBuffer;
}

void SoundGenerator::SetNumberOfImagesPerSecond( int nb )
{
    m_nbFramesPerImage = m_sampleFreq / nb;
}

int SendFramesToSoundCardCB( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *data )
{
    if ( status )
      std::cout << "Stream underflow detected!" << std::endl;

    SoundGenerator * gen = (SoundGenerator*)data;
    AudioParams params;
    params.outputBuffer = outputBuffer;
    params.inputBuffer = inputBuffer;
    params.nbFrames = nBufferFrames;
    params.streamTime = streamTime;
    params.status = status;
    return gen->SendFramesToSoundCard( params );
}

bool SoundGenerator::Play()
{
    m_audio = new RtAudio;

    if( m_audio->getDeviceCount() < 1 )
    {
        cerr << "No audio device!" << endl;
        return false;
    }

    m_audio->showWarnings( true );

    unsigned nbFrames = 512;

    RtAudio::StreamParameters params;
    params.deviceId = m_deviceIndex;
    params.nChannels = m_nbChannels;
    params.firstChannel = 0;

    RtAudio::StreamOptions options;
    options.flags = RTAUDIO_HOG_DEVICE;
    options.flags |= RTAUDIO_SCHEDULE_REALTIME;
    options.flags |= RTAUDIO_NONINTERLEAVED;

    try
    {
        m_audio->openStream( &params, NULL, FORMAT, m_sampleFreq, &nbFrames, &SendFramesToSoundCardCB, (void *)this, &options );
        m_audio->startStream();
    }
    catch ( RtError& e )
    {
        e.printMessage();
        delete m_audio;
        m_audio = 0;
    }

    return true;
}

void SoundGenerator::Stop()
{
    if( m_audio && m_audio->isStreamOpen() )
        m_audio->closeStream();
    delete m_audio;
    m_audio = 0;
}

static const char * pixelShaderCode = " \
uniform sampler2DRect tex_id; \
uniform float samplePixInterval; \
uniform float freqPixInterval; \
uniform float freqInterval; \
uniform float minFreq; \
uniform float maxFreq; \
uniform float sampleTimeInterval; \
uniform float startTime; \
uniform float masterVolume; \
\
void main() \
{ \
    float PI = 3.14159265358979323846264; \
    int sampleIndex = (int)floor( gl_FragCoord.x - 0.5 + 1000 * ( gl_FragCoord.y - 0.5 ) ); \
    float time = startTime + sampleIndex * sampleTimeInterval; \
    float texRow = 0.5 * samplePixInterval + sampleIndex * samplePixInterval; \
    float fPix = 0.5 * freqPixInterval; \
    float sampleValue = 0.0; \
    for( float f = minFreq; f <= maxFreq; f += freqInterval, fPix += freqPixInterval ) \
    { \
        vec2 texCoord = vec2( fPix, texRow ); \
        vec4 texSample = texture2DRect( tex_id, texCoord ); \
        float freqIntensity = ( texSample.r + texSample.g + texSample.b ) / 3.0; \
        sampleValue += freqIntensity * sin( 2 * PI * f * time ); \
    } \
    gl_FragColor[0] = sampleValue * masterVolume; \
}";

void SoundGenerator::GenerateFramesForImage( drwDrawableTexture * image )
{
    // Update target texture size
    int textureHeight = m_nbFramesPerImage / 1000 + 1;
    if( !m_soundGenerationTarget )
    {
        m_soundGenerationTarget = new drwDrawableTexture;
        m_soundGenerationTarget->SetPixelTypeToGrey();
        m_soundGenerationTarget->SetComponentTypeToUnsignedShort();
    }
    m_soundGenerationTarget->Resize( 1000, textureHeight );
    m_soundGenerationTarget->DrawToTexture( true );

    // Update sound generation shader
    if( !m_soundGenerationShader )
    {
        m_soundGenerationShader = new drwGlslShader;
        m_soundGenerationShader->AddShaderMemSource( pixelShaderCode );
        m_soundGenerationShader->Init();
    }
    m_soundGenerationShader->UseProgram( true );
    m_soundGenerationShader->SetVariable( "tex_id", 0 );
    float samplePixInterval = ((float)image->GetHeight()) / m_nbFramesPerImage;
    m_soundGenerationShader->SetVariable( "samplePixInterval", samplePixInterval );
    float freqPixInterval = ((float)image->GetWidth() ) / m_nbFrequencies ;
    m_soundGenerationShader->SetVariable( "freqPixInterval", freqPixInterval );
    float freqInterval = ( m_maxFrequency - m_minFrequency ) / m_nbFrequencies;
    m_soundGenerationShader->SetVariable( "freqInterval", freqInterval );
    m_soundGenerationShader->SetVariable( "minFreq", float(m_minFrequency) );
    m_soundGenerationShader->SetVariable( "maxFreq", float(m_maxFrequency) );
    float sampleTimeInterval = 1.0 / m_nbFramesPerImage;
    m_soundGenerationShader->SetVariable( "sampleTimeInterval", sampleTimeInterval );
    m_soundGenerationShader->SetVariable( "startTime", float( 0.0 ) );
    m_soundGenerationShader->SetVariable( "masterVolume", float( 0.5 ) );

    // render sound in the GPU using shader
    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D( 0, 1000, 0, textureHeight );

    glBindTexture( GL_TEXTURE_RECTANGLE_ARB, image->GetTexId() );
    glBegin( GL_QUADS );
    {
        glTexCoord2i( 0, 0 );					glVertex2d( 0, 0 );
        glTexCoord2i( 1000, 0 );                glVertex2d( 1000, 0 );
        glTexCoord2i( 1000, textureHeight );	glVertex2d( 1000, textureHeight );
        glTexCoord2i( 0, textureHeight );		glVertex2d( 0, textureHeight );
    }
    glEnd();
    glBindTexture( GL_TEXTURE_RECTANGLE_ARB, 0 );

    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );

    m_soundGenerationTarget->DrawToTexture( false );

    // download soundtrack from GPU
    int soundBufferSize = textureHeight * 1000;
    if( !m_soundBuffer || soundBufferSize != m_soundBufferSize )
    {
        delete m_soundBuffer;
        m_soundBufferSize = soundBufferSize;
        m_soundBuffer = new SampleType[ m_soundBufferSize ];
    }
    m_soundGenerationTarget->Download( m_soundBuffer );
}

int SoundGenerator::SendFramesToSoundCard( AudioParams & params )
{
    // determine the number of frames to copy
    int nbFramesToCopy = params.nbFrames;
    int nbBlankFrames = 0;
    int nbFramesLeftInBuffer = m_soundBufferSize - m_nextBufferIndex;
    if( nbFramesToCopy < nbFramesLeftInBuffer )
    {
        nbFramesToCopy = nbFramesLeftInBuffer;
        nbBlankFrames = params.nbFrames - nbFramesToCopy;
    }

    // copy identical frames in each channel
    SampleType * outBuf = (SampleType*)params.outputBuffer;
    SampleType * inBuf = m_soundBuffer + m_nextBufferIndex;
    for( int i = 0; i < m_nbChannels; ++i )
    {
        memcpy( outBuf, inBuf, nbFramesToCopy * sizeof(SampleType) );
        outBuf += nbFramesToCopy;
        if( nbBlankFrames > 0 )
        {
            memset( outBuf, 0, nbBlankFrames * sizeof(SampleType) );
            outBuf += nbBlankFrames;
        }
    }
    m_nextBufferIndex += nbFramesToCopy;

    if( nbBlankFrames > 0 )
        return 1;
    return 0;
}
