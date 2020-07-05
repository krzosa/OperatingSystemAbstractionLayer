#include <dsound.h>
typedef HRESULT WINAPI direct_sound_create(LPGUID lpGuid, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter);

typedef struct win32_audio_data
{
    LPDIRECTSOUNDBUFFER audioBuffer;
    // NOTE: keeps track of where are we in the buffer in terms of writing
    u32 runningSampleIndex;
    // NOTE: this value determines the tone of the sound, we use it to calculate the period of the wave
    //       period of the wave is equal to how far apart are the peaks of the wave
    // it describes how many full peaks we should hit in a second to get a pitch
    i32 cyclesPerSecondOrHz;
    // NOTE: how many samples to output in a second, that means that there are about 800 samples to output
    //       per frame at 60 fps becuase 60 fps is 16ms per frame
    i32 samplesPerSecond;
    // NOTE: audio channels determine how many output speakers we want to output to, we want stereo sound
    //      aka we want to output into 2 speakers
    i32 numberOfChannels;
    // NOTE: how big is one samples in terms of bytes. One sample should be 16 bits but we have 2 channels
    //      so there are 2 channel samples per sample in format || 16b left channel | 16b right channel || ...
    i32 bytesPerSample;
    // NOTE: how big in bytes is the buffer. So we need to know how big is one sample in bytes.
    //       and we multiply how many samples we want by the one sample size which is 16 bit
    //       in this case but there are 2 channels per sample so 32 bit in the end
    i32 bufferSize;
    // NOTE: wave period is how many samples it takes to go from one peak of the wave to another
    i32 wavePeriod;
    i16 toneVolume;
    bool32 soundIsPlaying;
} win32_audio_data;

internal LPDIRECTSOUNDBUFFER
Win32AudioInitialize(HWND window, i32 samplesPerSecond, i32 bufferSize)
{
    LPDIRECTSOUNDBUFFER secondaryBuffer = 0;
    HMODULE DSoundLibrary = LoadLibraryA("dsound.dll");
    if (DSoundLibrary)
    {
        direct_sound_create *directSoundCreateObject =
            (direct_sound_create *)GetProcAddress(DSoundLibrary, "DirectSoundCreate");

        assert(directSoundCreateObject);

        // NOTE: first buffer is for setting cooperative level and general settings
        //       we write to the second buffer only
        LPDIRECTSOUND directSound;
        if (directSoundCreateObject && SUCCEEDED(directSoundCreateObject(0, &directSound, 0)))
        {
            WAVEFORMATEX WaveFormat = {0};
            {
                WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
                WaveFormat.nChannels = 2;
                WaveFormat.nSamplesPerSec = samplesPerSecond;
                WaveFormat.wBitsPerSample = 16;
                WaveFormat.nBlockAlign = (WaveFormat.nChannels * WaveFormat.wBitsPerSample) / 8;
                WaveFormat.nAvgBytesPerSec = WaveFormat.nSamplesPerSec * WaveFormat.nBlockAlign;
                WaveFormat.cbSize = 0;
            }
            if (SUCCEEDED(directSound->lpVtbl->SetCooperativeLevel(
                    directSound, window, DSSCL_PRIORITY)))
            {
                DSBUFFERDESC bufferDescription = {0};
                {
                    bufferDescription.dwSize = sizeof(bufferDescription);
                    bufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;
                }

                LPDIRECTSOUNDBUFFER primaryBuffer;
                if (SUCCEEDED(directSound->lpVtbl->CreateSoundBuffer(
                        directSound, &bufferDescription, &primaryBuffer, 0)))
                {
                    HRESULT Error = primaryBuffer->lpVtbl->SetFormat(primaryBuffer, &WaveFormat);
                    if (SUCCEEDED(Error))
                    {
                        LogSuccess("Primary buffer format was set");
                    }
                    else
                    {
                        LogError("failed to set format for primary buffer");
                    }
                }
                else
                {
                    LogError("failed to create sound buffer");
                }
            }
            else
            {
                LogError("failed to set cooperative level");
            }
            DSBUFFERDESC bufferDescriptor = {0};
            {
                bufferDescriptor.dwSize = sizeof(bufferDescriptor);
                bufferDescriptor.dwFlags = 0;
                bufferDescriptor.dwBufferBytes = bufferSize;
                bufferDescriptor.lpwfxFormat = &WaveFormat;
            }
            HRESULT Error = directSound->lpVtbl->CreateSoundBuffer(
                directSound, &bufferDescriptor, &secondaryBuffer, 0
            );
            if (SUCCEEDED(Error))
            {
                LogSuccess("Secondary buffer created");
            }
        }
        else
        {
            LogError("Direct sound failed to create sound object");
        }
    }
    else
    {
        LogError("Direct sound failed to load dll");
    }

    return secondaryBuffer;
}

internal void 
Win32FillAudioBuffer(win32_audio_data *audioData, DWORD byteToLock, DWORD numberOfBytesToLock)
{
    VOID *region1 = 0;
    VOID *region2 = 0;
    DWORD region1Size = 0;
    DWORD region2Size = 0;
    LPDIRECTSOUNDBUFFER audioBuffer = audioData->audioBuffer;

    if(SUCCEEDED(audioBuffer->lpVtbl->Lock(audioBuffer, 
                                            byteToLock, numberOfBytesToLock, 
                                            &region1, &region1Size, 
                                            &region2, &region2Size, 0)))
    {
        i16 *sample = (i16 *)region1;
        i32 region1SampleCount = region1Size / audioData->bytesPerSample;

        for(i32 i = 0; i != region1SampleCount; i++)
        {
            f32 t = 2 * MATH_PI * (f32)audioData->runningSampleIndex / (f32)audioData->wavePeriod;
            f32 sinValue = sinf(t);
            i16 sampleValue = (i16)(sinValue * 6000);
            *sample++ = sampleValue;
            *sample++ = sampleValue;
            audioData->runningSampleIndex++;
        }

        sample = (i16 *)region2;
        i32 region2SampleCount = region2Size / audioData->bytesPerSample;

        for(i32 i = 0; i != region2SampleCount; i++)
        {
            f32 t = 2 * MATH_PI * (f32)audioData->runningSampleIndex / (f32)audioData->wavePeriod;
            f32 sinValue = sinf(t);
            i16 sampleValue = (i16)(sinValue * 6000);
            *sample++ = sampleValue;    
            *sample++ = sampleValue;
            audioData->runningSampleIndex++;
        }

        audioBuffer->lpVtbl->Unlock(audioBuffer, 
                                    region1, region1Size, 
                                    region2, region2Size);
    } 
}