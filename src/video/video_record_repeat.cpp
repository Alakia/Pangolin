/* This file is part of the Pangolin Project.
 * http://github.com/stevenlovegrove/Pangolin
 *
 * Copyright (c) 2013 Steven Lovegrove
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include <pangolin/video/video_record_repeat.h>
#include <pangolin/video/drivers/pango_video.h>
#include <pangolin/video/drivers/pango_video_output.h>

namespace pangolin
{

VideoRecordRepeat::VideoRecordRepeat(
    const std::string& input_uri,
    const std::string& log_filename,
    int buffer_size_bytes
    ) : video_src(0), video_file(0), video_recorder(0),
    filename(log_filename), buffer_size_bytes(buffer_size_bytes), frame_num(0)
{
    video_src = OpenVideo(input_uri);
}

VideoRecordRepeat::~VideoRecordRepeat()
{
    if(video_recorder)
        delete video_recorder;
    if( video_src )
        delete video_src;
    if( video_file )
        delete video_file;
}

const std::string& VideoRecordRepeat::LogFilename() const
{
    return filename;
}

void VideoRecordRepeat::Record()
{
    if( video_recorder ) {
        video_src->Stop();
        delete video_recorder;
        video_recorder = 0;
    }

    if(video_file) {
        delete video_file;
        video_file = 0;
    }

    video_recorder = new PangoVideoOutput(filename);
    video_recorder->AddStreams( video_src->Streams() );

    video_src->Start();
    frame_num = 0;
}

void VideoRecordRepeat::Play(bool realtime)
{
    if( video_file ) {
        delete video_file;
        video_file = 0;
    }

    video_src->Stop();

    if(video_recorder) {
        delete video_recorder;
        video_recorder = 0;
    }

    video_file = new PangoVideo(filename,realtime);
    frame_num = 0;
}

void VideoRecordRepeat::Source()
{
    if(video_file) {
        delete video_file;
        video_file = 0;
    }

    if(video_recorder) {
        delete video_recorder;
        video_recorder = 0;
    }

    video_src->Start();
    frame_num = 0;
}

size_t VideoRecordRepeat::SizeBytes() const
{
    if( !video_src ) throw VideoException("No video source open");
    return video_src->SizeBytes();
}

const std::vector<StreamInfo>& VideoRecordRepeat::Streams() const
{
    return video_src->Streams();
}

void VideoRecordRepeat::Start()
{
    // Semantics of this?
//    video_src->Start();
}

void VideoRecordRepeat::Stop()
{
    // Semantics of this?
    if(video_recorder) {
        delete video_recorder;
        video_recorder = 0;
    }
}

bool VideoRecordRepeat::GrabNext( unsigned char* image, bool wait )
{
    frame_num++;

    if( video_recorder != 0 ) {
        bool success = video_src->GrabNext(image, wait);
        if( success ) {
            video_recorder->WriteStreams(image);
        }
        return success;
    }else if( video_file != 0 ) {
        return video_file->GrabNext(image,wait);
    }else{
        return video_src->GrabNext(image,wait);
    }
}

bool VideoRecordRepeat::GrabNewest( unsigned char* image, bool wait )
{
    frame_num++;

    if( video_recorder != 0 )
    {
        bool success = video_src->GrabNewest(image,wait);
        if( success ) {
            video_recorder->WriteStreams(image);
        }
        return success;
    }else if( video_file != 0 ) {
        return video_file->GrabNewest(image,wait);
    }else{
        return video_src->GrabNewest(image,wait);
    }
}

int VideoRecordRepeat::FrameId()
{
    return frame_num;
}

bool VideoRecordRepeat::IsRecording() const
{
    return video_recorder != 0;
}

bool VideoRecordRepeat::IsPlaying() const
{
    return video_file != 0;
}



}

