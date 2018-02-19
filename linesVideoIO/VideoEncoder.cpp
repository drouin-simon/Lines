#include "VideoEncoder.h"

extern "C" {
    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>
    #include <math.h>
    #include <libavutil/opt.h>
    #include <libavutil/mathematics.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
    #include <libswresample/swresample.h>
    #include <libavutil/imgutils.h>
    #include <libavcodec/avcodec.h>
}

class VideoEncoderInternal
{
public:
    AVFrame * m_frame;
    AVCodecContext * m_c;
    AVStream * m_video_st;
    AVOutputFormat * m_fmt;
    AVFormatContext * m_oc;
    AVCodec * m_video_codec;
    AVPicture m_src_picture;
    AVPicture m_dst_picture;
};


VideoEncoder::VideoEncoder()
{
    m_init = false;
    m_videoWidth = 640;
    m_videoHeight = 480;
    m_fps = 30;
    m_bitPerSec = 10000;
    m_gob = 10;
    m_sws_flags = SWS_BICUBIC;
    m_frame_count = 0;
    m_in = new VideoEncoderInternal;
}

VideoEncoder::~VideoEncoder()
{
    delete m_in;
}

void VideoEncoder::WriteFrame(void)
{
    // Inits
	int ret;
    static struct SwsContext * sws_ctx;
    AVCodecContext * c = m_in->m_video_st->codec;

	//If we haven't already done so init the context of the frame conversion from RGB to YUV
    if (!sws_ctx)
    {
        sws_ctx = sws_getContext( c->width, c->height, AV_PIX_FMT_RGB24,
									c->width, c->height, AV_PIX_FMT_YUV420P,
                                    m_sws_flags, NULL, NULL, NULL );
        if (!sws_ctx)
        {
            m_lastError = "Couldn't initialize frame rgb to yuv convertion.";
			return;
		}
	}

    // Convert RGB frame (m_src_picture) to and YUV frame (m_dst_picture)
	sws_scale(sws_ctx,
                m_in->m_src_picture.data, m_in->m_src_picture.linesize,
                0, c->height, m_in->m_dst_picture.data, m_in->m_dst_picture.linesize);
	
	
    // Some inits for encoding the frame
	AVPacket pkt = { 0 };
	int got_packet;
	av_init_packet(&pkt);

    // Encode the frame
    ret = avcodec_encode_video2( c, &pkt, m_in->m_frame, &got_packet );
    if( ret < 0 )
    {
		return;
	}
	
    // If size of encoded frame is zero, it means the image was buffered.
    if( !ret && got_packet && pkt.size )
    {
        pkt.stream_index = m_in->m_video_st->index;
		
		//Write the compressed frame to the media file.
        ret = av_interleaved_write_frame( m_in->m_oc, &pkt );

		//if non-zero then it means that there was something wrong writing the frame to
		//the file
        if (ret != 0)
        {
			return;
		}
	} 
    else
    {
		ret = 0;
	}
	
	//Increment Frame counter
	m_frame_count++;
    m_in->m_frame->pts += av_rescale_q( 1, m_in->m_video_st->codec->time_base, m_in->m_video_st->time_base );
}

bool VideoEncoder::OpenVideo( const char * filename, int width, int height, int fps, int gob, int bitsPerSecond )
{
    m_filename = filename;
    m_videoWidth = width;
    m_videoHeight = height;
    m_fps = fps;
    m_gob = gob;  // nb partial frames per keyframe
    m_bitPerSec = bitsPerSecond;
    m_sws_flags = SWS_BICUBIC;
    m_frame_count = 0;

    //---------------------
    //The basic flow for opening a video is:
    //---------------------
    //Register VideoEncoder -> Allocate Media Context -> Allocate Stream Format Context ->
    //Allocate Codec Context -> Allocate Frames -> Open Media File -> Start Writing Frames!!

    // You must call these subroutines otherwise you get errors!!
    avcodec_register_all();
    av_register_all();

    avformat_alloc_output_context2( &(m_in->m_oc), NULL, "mp4", m_filename.c_str() );
    if( !m_in->m_oc )
    {
        m_lastError = "Couldn't allocate output context";
        return false;
    }

    // Get the format determined by the container
    m_in->m_fmt = m_in->m_oc->oformat;

    // Add the audio and video streams using the default format codecs
    // and initialize the codecs.
    m_in->m_video_st = NULL;

    //Setup the codecs
    m_in->m_fmt->video_codec = AV_CODEC_ID_H264;
    m_in->m_fmt->audio_codec = AV_CODEC_ID_NONE;

    //-------------------------------------------------------------------
    // Add an output stream.
    //-------------------------------------------------------------------

    //find the encoder
    m_in->m_video_codec = avcodec_find_encoder( m_in->m_fmt->video_codec );
    if( !(m_in->m_video_codec) )
    {
        m_lastError = "Couldn't find an encoder corresponding to the requested video codec.";
        return false;
    }

    //Create new video stream
    AVStream * st = avformat_new_stream( m_in->m_oc, m_in->m_video_codec );
    if( !st )
    {
        m_lastError = "Couldn't open a video stream on the chosen video encoder.";
        return false;
    }
    st->id = m_in->m_oc->nb_streams - 1;

    // Set codec context
    m_in->m_c = st->codec;

    // Setup fundumental video stream parameters
    m_in->m_c->codec_id = m_in->m_fmt->video_codec;
    m_in->m_c->bit_rate = m_bitPerSec;			//Bits Per Second
    m_in->m_c->width    = m_videoWidth;			//Note Resolution must be a multiple of 2!!
    m_in->m_c->height   = m_videoHeight;		//Note Resolution must be a multiple of 2!!
    m_in->m_c->time_base.den = m_fps;		//Frames per second
    m_in->m_c->time_base.num = 1;
    m_in->m_c->gop_size      = m_gob;		// Intra frames per x P frames
    m_in->m_c->pix_fmt       = AV_PIX_FMT_YUV420P;//Do not change this, H264 needs YUV format not RGB

    // Some formats want stream headers to be separate.
    if( m_in->m_oc->oformat->flags & AVFMT_GLOBALHEADER )
        m_in->m_c->flags |= CODEC_FLAG_GLOBAL_HEADER;

    // Set our video stream pointer
    m_in->m_video_st = st;

    //-------------------------------------------------------------------
    // Now that all the parameters are set, we can open the audio and
    // video codecs and allocate the necessary encode buffers.
    //-------------------------------------------------------------------

    //Allocated Codec Context
    AVCodecContext * c = m_in->m_video_st->codec;

    //Open the codec
    int ret = avcodec_open2( c, m_in->m_video_codec, NULL );
    if( ret < 0 )
    {
        m_lastError = "Couldn't open requested codec.";
        return false;
    }

    // allocate and init a re-usable frame
    //m_in->m_frame = avcodec_alloc_frame();
    if( !m_in->m_frame )
    {
        m_lastError = "Couldn't allocate a video frame.";
        return false;
    }

    // Allocate the YUV encoded raw picture.
    ret = avpicture_alloc( &(m_in->m_dst_picture), c->pix_fmt, c->width, c->height );
    if( ret < 0 )
    {
        m_lastError = "Coulnd't allocate encoded raw YUV picture.";
        return false;
    }

    // Allocate RGB frame that we can pass to the YUV frame
    ret = avpicture_alloc( &(m_in->m_src_picture), AV_PIX_FMT_RGB24, c->width, c->height );
    if( ret < 0 )
    {
        m_lastError = "Coulnd't allocate encoded RGB frame.";
        return false;
    }

    //Copy data and linesize picture pointers to frame
    *((AVPicture *)m_in->m_frame) = m_in->m_dst_picture;

    // Tell VideoEncoder that we are going to write encoded frames to a file
    av_dump_format( m_in->m_oc, 0, filename, 1 );

    //open the output file, if needed
    if( !(m_in->m_fmt->flags & AVFMT_NOFILE) )
    {
        ret = avio_open( &m_in->m_oc->pb, filename, AVIO_FLAG_WRITE );
        if( ret < 0 )
        {
            m_lastError = "Couldn't open output file for writing.";
            return false;
        }
    }

    // Write the stream header, if any.
    ret = avformat_write_header( m_in->m_oc, NULL);
    if( ret < 0 )
    {
        m_lastError = "Couldn't write output file header.";
        return false;
    }

    // Set frame count to zero
    if( m_in->m_frame )
        m_in->m_frame->pts = 0;
}

//-----------------------------
// Close Video Codec
//-----------------------------
void VideoEncoder::CloseCodec(void)
{
    if( !IsInitialized() )
        return;

    // Write trailing bits
    av_write_trailer(m_in->m_oc);

	//Close Video codec
    avcodec_close(m_in->m_video_st->codec);

	//Free our frames
    av_free(m_in->m_src_picture.data[0]);
    av_free(m_in->m_dst_picture.data[0]);
    av_free(m_in->m_frame);

	//A bit more cleaning
    if (!(m_in->m_fmt->flags & AVFMT_NOFILE))
        avio_close(m_in->m_oc->pb);

    avformat_free_context(m_in->m_oc);

    m_init = false;
}

//-----------------------------
// Close Video Codec
//-----------------------------
void VideoEncoder::CloseVideo(void)
{
	CloseCodec();	
}

//-----------------------------
// Write a Randomly Generated RGB frame
//-----------------------------
void VideoEncoder::WriteDummyFrame()
{
    for (int y=0;y<m_in->m_c->height;y++)
    {
        for (int x=0;x<m_in->m_c->width;x++)
        {
            m_in->m_src_picture.data[0][y * m_in->m_src_picture.linesize[0] + x*3+0] = char(rand()*255);
            m_in->m_src_picture.data[0][y * m_in->m_src_picture.linesize[0] + x*3+1] = char(rand()*255);
            m_in->m_src_picture.data[0][y * m_in->m_src_picture.linesize[0] + x*3+2] = char(rand()*255);
		}
	}
	
	WriteFrame();
}

//-----------------------------
// Write an RGB frame supplied by the user
//-----------------------------
void VideoEncoder::WriteFrame( char * RGBFrame )
{
	//Data should be in the format RGBRGBRGB...etc and should be Width*Height*3 long
	
	//Step through height of frame
    for (int y=0;y<m_in->m_c->height;y++) {  //Height Loop

		//Step through width of frame
        for (int x=0;x<m_in->m_c->width;x++) { //Width Loop

            //Save RGB frame to VideoEncoder's source frame
            m_in->m_src_picture.data[0][y * m_in->m_src_picture.linesize[0] + x*3+0] = RGBFrame[(y*m_in->m_c->width+x)*3+0];  //Red Channel
            m_in->m_src_picture.data[0][y * m_in->m_src_picture.linesize[0] + x*3+1] = RGBFrame[(y*m_in->m_c->width+x)*3+1];  //Green Channel
            m_in->m_src_picture.data[0][y * m_in->m_src_picture.linesize[0] + x*3+2] = RGBFrame[(y*m_in->m_c->width+x)*3+2];  //Blue Channel
		}
	}

    //Send frame off to VideoEncoder for encoding
	WriteFrame();
}
