#pragma once

class HostSpeech
{
	CString str_speech_language, m_dir;
	int m_volume = 0;

public:
	HostSpeech();
	virtual ~HostSpeech();

	void test();
	void play(CString filePath);
	void stop(); //note: need the same thread. Or, use play a mute sound to stop.
	void set_language(int lang=0); 	//0:EN, 1:CH 2:ESP 255:user's define
	void set_volume(int vol=2);		//0:small, 1:medium, 2:large 
	void set_dir(CString dir) { m_dir = dir; }
};