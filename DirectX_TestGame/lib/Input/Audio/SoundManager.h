#pragma once
#include <xaudio2.h>
#include <unordered_map>
#include <string>
#include <vector>

class SoundManager {
public:
    SoundManager();
    ~SoundManager();

    bool Initialize();
    void Finalize();

    bool LoadSE(const std::string& name, const std::wstring& filePath);
    void PlaySE(const std::string& name);

    bool LoadBGM(const std::wstring& filePath);
    void PlayBGM(bool loop = true);
    void StopBGM();

private:
    struct SoundData {
        XAUDIO2_BUFFER buffer;
        WAVEFORMATEX format;
        std::vector<BYTE> data;
    };

    IXAudio2* xAudio2 = nullptr;
    IXAudio2MasteringVoice* masterVoice = nullptr;
    IXAudio2SourceVoice* bgmVoice = nullptr;
    SoundData bgm;

    std::unordered_map<std::string, SoundData> seMap;

    bool LoadWAVFile(const std::wstring& filePath, SoundData& outData);
};
