#pragma once

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

	void* GetVideoRam() {return m_pVideoRamPtr;}
	void  SetVideoRam(void* ptr) { m_pVideoRamPtr = ptr; }

private:
	VideoRam();
	static VideoRam* m_pVideoRam;

	void* m_pVideoRamPtr;
	int m_width;
	int m_height;
	int m_bpp;
};