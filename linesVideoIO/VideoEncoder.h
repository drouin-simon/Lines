#ifndef __VideoEncoder_h_
#define __VideoEncoder_h_

#include <string>

class VideoEncoderInternal;

class VideoEncoder
{

public:

    VideoEncoder();
    ~VideoEncoder();

    bool OpenVideo( const char * filename, int width, int height, int fps, int gob, int bitsPerSecond );
	void WriteDummyFrame();
	void WriteFrame(char * RGBFrame);
	void CloseVideo(void);
	
    int GetVideoWidth(void) { return m_videoWidth; }
    int GetVideoHeight(void) { return m_videoHeight; }

    std::string GetLastError() { return m_lastError; }
	
private:

    bool m_init;
    std::string m_filename;
    int m_videoWidth;
    int m_videoHeight;
    int m_fps;
    int m_bitPerSec;
    int m_gob;

	int m_sws_flags;
	int m_frame_count;

	void CloseCodec(void);
	void WriteFrame(void);
    bool IsInitialized() { return m_init; }

    VideoEncoderInternal * m_in;

    std::string m_lastError;
};

#endif
