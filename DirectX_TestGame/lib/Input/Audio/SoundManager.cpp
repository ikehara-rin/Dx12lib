#include "SoundManager.h"
#include <fstream>

SoundManager::SoundManager() {}

SoundManager::~SoundManager() {
    Finalize();
}

bool SoundManager::Initialize() {
    HRESULT hr = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
    if (FAILED(hr)) return false;

    hr = xAudio2->CreateMasteringVoice(&masterVoice);
    return SUCCEEDED(hr);
}

void SoundManager::Finalize() {
    if (bgmVoice) {
        bgmVoice->DestroyVoice();
        bgmVoice = nullptr;
    }

    if (masterVoice) {
        masterVoice->DestroyVoice();
        masterVoice = nullptr;
    }

    if (xAudio2) {
        xAudio2->Release();
        xAudio2 = nullptr;
    }
}

bool SoundManager::LoadSE(const std::string& name, const std::wstring& filePath) {
    SoundData data;
    if (!LoadWAVFile(filePath, data)) return false;
    seMap[name] = std::move(data);
    return true;
}

void SoundManager::PlaySE(const std::string& name) {
    auto it = seMap.find(name);
    if (it == seMap.end()) return;

    IXAudio2SourceVoice* voice = nullptr;
    xAudio2->CreateSourceVoice(&voice, &it->second.format);
    voice->SubmitSourceBuffer(&it->second.buffer);
    voice->Start();
}

bool SoundManager::LoadBGM(const std::wstring& filePath) {
    if (!LoadWAVFile(filePath, bgm)) return false;
    xAudio2->CreateSourceVoice(&bgmVoice, &bgm.format);
    return true;
}

void SoundManager::PlayBGM(bool loop) {
    if (!bgmVoice) return;

    XAUDIO2_BUFFER buffer = bgm.buffer;
    if (loop) {
        buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
    }

    bgmVoice->SubmitSourceBuffer(&buffer);
    bgmVoice->Start();
}

void SoundManager::StopBGM() {
    if (bgmVoice) {
        bgmVoice->Stop();
        bgmVoice->FlushSourceBuffers();
    }
}

bool SoundManager::LoadWAVFile(const std::wstring& filePath, SoundData& outData) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) return false;

    DWORD chunkId, chunkSize;
    file.read(reinterpret_cast<char*>(&chunkId), sizeof(DWORD)); // "RIFF"
    file.read(reinterpret_cast<char*>(&chunkSize), sizeof(DWORD));
    file.ignore(4); // "WAVE"

    // Read chunks
    while (true) {
        file.read(reinterpret_cast<char*>(&chunkId), sizeof(DWORD));
        file.read(reinterpret_cast<char*>(&chunkSize), sizeof(DWORD));
        if (chunkId == ' tmf') break;
        file.ignore(chunkSize);
    }

    file.read(reinterpret_cast<char*>(&outData.format), sizeof(WAVEFORMATEX));
    if (chunkSize > sizeof(WAVEFORMATEX))
        file.ignore(chunkSize - sizeof(WAVEFORMATEX));

    while (true) {
        file.read(reinterpret_cast<char*>(&chunkId), sizeof(DWORD));
        file.read(reinterpret_cast<char*>(&chunkSize), sizeof(DWORD));
        if (chunkId == 'atad') break;
        file.ignore(chunkSize);
    }

    outData.data.resize(chunkSize);
    file.read(reinterpret_cast<char*>(outData.data.data()), chunkSize);

    ZeroMemory(&outData.buffer, sizeof(XAUDIO2_BUFFER));
    outData.buffer.AudioBytes = static_cast<UINT32>(outData.data.size());
    outData.buffer.pAudioData = outData.data.data();
    outData.buffer.Flags = XAUDIO2_END_OF_STREAM;

    return true;
}
