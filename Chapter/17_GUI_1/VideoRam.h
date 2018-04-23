#pragma once

typedef struct tagVideoRamInfo
{
	void* _pVideoRamPtr;
	int _width;
	int _height;
	int _bpp;

	tagVideoRamInfo()
	{
		_pVideoRamPtr = nullptr;
		_width = 0;
		_height = 0;
		_bpp = 0;
	}

}VideoRamInfo;

class VideoRam
{
public:	
	virtual ~VideoRam();

	static VideoRam* GetInstance()
	{
		if (m_pVideoRam == nullptr)
			m_pVideoRam = new VideoRam();

		return m_pVideoRam;
	}

	VideoRamInfo& GetVideoRamInfo() {return m_videoRamInfo;}
	void  SetVideoRamInfo(VideoRamInfo& info) { m_videoRamInfo = info; }

private:
	VideoRam();
	static VideoRam* m_pVideoRam;

	VideoRamInfo m_videoRamInfo;
};