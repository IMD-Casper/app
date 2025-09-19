#include "stdafx.h"
#include "imd_framework.h"
#include "HostSpeech.h"
#include <mmsystem.h>

HostSpeech::HostSpeech()
{
	set_language();
	set_volume();
}

HostSpeech::~HostSpeech()
{

}

void HostSpeech::play(CString mp3_filename)
{
	if (m_volume == 0)
		return;

	CString command;
	CString filePath;
	filePath.Format(L"%s/speech/%s/%s", m_dir, str_speech_language, mp3_filename);
	//dbg("%s\n", (CT2A)filePath);
	

	stop();

	command.Format(_T("open \"%s\" type mpegvideo alias mp3"), filePath);
	mciSendString(command, nullptr, 0, nullptr);
	mciSendString(_T("play mp3"), nullptr, 0, nullptr);
}

void HostSpeech::stop()
{
	if (m_volume == 0)
		return;

	mciSendString(_T("stop mp3"), nullptr, 0, nullptr);
	mciSendString(_T("close mp3"), nullptr, 0, nullptr);
}

void HostSpeech::set_volume(int vol)
{
	// 確保音量在 0 到 1000 的範圍內
	switch (vol) {
	case 0:
		m_volume = 250;
		break;
	case 1:
		m_volume = 600;
		break;
	default:
	case 2:
		m_volume = 1000;
		break;
	case 3:
		m_volume = 0;
		break;
	}

	CString command;
	command.Format(_T("setaudio mp3 volume to %d"), m_volume);

	mciSendString(command, nullptr, 0, nullptr);
}

void HostSpeech::test()
{
	//set_volume(2);
	play(L"00001.mp3");
	play(L"00002.mp3");
	play(L"00000.mp3");
}

void HostSpeech::set_language(int speech_language)
{
	switch (speech_language) {
	case 0:
		str_speech_language = L"EN";
		break;
	case 1:
		str_speech_language = L"CH";
		break;
	case 2:
		str_speech_language = L"ESP";
		break;
	default:
		str_speech_language = L"USER";
		break;
	}

}