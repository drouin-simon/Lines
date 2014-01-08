#ifndef __SoundGenerator_h_
#define __SoundGenerator_h_

class AudioParams;
class RtAudio;
class drwDrawableTexture;
class drwGlslShader;

class SoundGenerator
{

public:

    typedef float SampleType;

    SoundGenerator();
    ~SoundGenerator();

    void SetNumberOfImagesPerSecond( int nb );

    bool Play();
    void Stop();

    void GenerateFramesForImage( drwDrawableTexture * image );

    void SetMasterVolume( double v ) { m_masterVol = v; }
    double GetMasterVolume() { return m_masterVol; }

    int SendFramesToSoundCard( AudioParams & params );

protected:

    int m_deviceIndex;
    int m_nbChannels;
    int m_sampleFreq;

    int m_nbFramesPerImage;  // number of sound frames for each image
    double m_masterVol;
    double m_lastTime;

    double m_minFrequency;
    double m_maxFrequency;
    int m_nbFrequencies;

    drwDrawableTexture * m_soundGenerationTarget;
    drwGlslShader * m_soundGenerationShader;

    int m_nextBufferIndex;
    int m_soundBufferSize;
    SampleType * m_soundBuffer;

    RtAudio * m_audio;

};

#endif
