#if 0
struct win32_sound_output
{
    i32 samplesPerSecond;
    i32 toneHz;
    i16 toneVolume;
    i32 runningSampleIndex;
    i32 wavePeriod;
    i32 bytesPerSample;
    i32 secondaryBufferSize;
    f32 tSine;
    i32 latencySampleCount;
};

// TODO: test going up to direct sound 8
internal void Win32InitDSound(HWND Window, int32_t samplesPerSecond, int32_t bufferSize)
{
    HMODULE DSoundLibrary = LoadLibraryA("dsound.dll");
    if(DSoundLibrary)
    {
        direct_sound_create* directSoundCreateObject = 
            (direct_sound_create*)GetProcAddress(DSoundLibrary, "DirectSoundCreate");

        /* NOTE(Kkrz): first buffer is for setting cooperative level and general settings
                        we write to the second buffer only */
        LPDIRECTSOUND DirectSound;
        if(directSoundCreateObject && SUCCEEDED(directSoundCreateObject(0, &DirectSound, 0)))
        {
            WAVEFORMATEX WaveFormat = {};
            {
                WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
                WaveFormat.nChannels = 2;
                WaveFormat.nSamplesPerSec = samplesPerSecond;
                WaveFormat.wBitsPerSample = 16;
                WaveFormat.nBlockAlign = (WaveFormat.nChannels*WaveFormat.wBitsPerSample) / 8;
                WaveFormat.nAvgBytesPerSec = WaveFormat.nSamplesPerSec*WaveFormat.nBlockAlign;
                WaveFormat.cbSize = 0;
            }
            if(SUCCEEDED(DirectSound->SetCooperativeLevel(Window, DSSCL_PRIORITY)))
            {
                DSBUFFERDESC bufferDescription = {};
                {
                    bufferDescription.dwSize = sizeof(bufferDescription);
                    bufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;
                }

                LPDIRECTSOUNDBUFFER primaryBuffer;
                if(SUCCEEDED(DirectSound->CreateSoundBuffer(&bufferDescription, &primaryBuffer, 0)))
                {
                    HRESULT Error = primaryBuffer->SetFormat(&WaveFormat);
                    if(SUCCEEDED(Error))
                    {
                        OutputDebugStringA("Primary buffer format was set.\n");
                    }
                    else
                    {
                        // TODO: failed to set format for primary buffer
                    }
                }
                else
                {
                    // TODO: failed to create sound buffer 
                }
            }
            else
            {
                // TODO: failed to set cooperative level
            }
            DSBUFFERDESC BufferDescription = {};
            {
                BufferDescription.dwSize = sizeof(BufferDescription);
                BufferDescription.dwFlags = 0;
                BufferDescription.dwBufferBytes = bufferSize;
                BufferDescription.lpwfxFormat = &WaveFormat;
            }
            HRESULT Error = DirectSound->CreateSoundBuffer(&BufferDescription, &global_variableSecondaryBuffer, 0);
            if(SUCCEEDED(Error))
            {
                OutputDebugStringA("Secondary buffer created successfully.\n");
            }
        }
        else
        {
            // TODO: failed to create sound object
        }
    }
    else
    {
        // TODO: failed to load dll
    }
}

internal void
Win32FillSoundBuffer(win32_sound_output *SoundOutput, DWORD ByteToLock, DWORD BytesToWrite)
{
    VOID *Region1;
    DWORD Region1Size;
    VOID *Region2;
    DWORD Region2Size;
    if(SUCCEEDED(GlobalSecondaryBuffer->Lock(ByteToLock, BytesToWrite,
                                             &Region1, &Region1Size,
                                             &Region2, &Region2Size, 0)))
    {
        DWORD Region1SampleCount = Region1Size/SoundOutput->bytesPerSample;
        i16 *SampleOut = (i16 *)Region1;
        for(DWORD SampleIndex = 0; SampleIndex < Region1SampleCount; ++SampleIndex)
        {
            float SineValue = sinf(SoundOutput->tSine);
            i16 SampleValue = (i16)(SineValue * SoundOutput->toneVolume);
            *SampleOut++ = SampleValue;
            *SampleOut++ = SampleValue;

            SoundOutput->tSine += 2.0f*Pi32*1.0f/(float)SoundOutput->wavePeriod;
            ++SoundOutput->runningSampleIndex;
        }

        DWORD Region2SampleCount = Region2Size/SoundOutput->bytesPerSample;
        SampleOut = (i16 *)Region2;
        for(DWORD SampleIndex = 0; SampleIndex < Region2SampleCount; ++SampleIndex)
        {
            float SineValue = sinf(SoundOutput->tSine);
            i16 SampleValue = (i16)(SineValue * SoundOutput->toneVolume);
            *SampleOut++ = SampleValue;
            *SampleOut++ = SampleValue;

            SoundOutput->tSine += 2.0f*Pi32*1.0f/(float)SoundOutput->wavePeriod;
            ++SoundOutput->runningSampleIndex;
        }

        GlobalSecondaryBuffer->Unlock(Region1, Region1Size, Region2, Region2Size);
    }
}

win32_sound_output SoundOutput = {};
{
    SoundOutput.samplesPerSecond = 48000;
    SoundOutput.toneHz = 256;
    SoundOutput.toneVolume = 1500;
    SoundOutput.wavePeriod = SoundOutput.samplesPerSecond/SoundOutput.toneHz;
    SoundOutput.bytesPerSample = sizeof(i16)*2;
    SoundOutput.secondaryBufferSize = SoundOutput.samplesPerSecond*SoundOutput.bytesPerSample;
    SoundOutput.latencySampleCount = SoundOutput.samplesPerSecond / 15;
}

Win32InitDSound(Window, SoundOutput.samplesPerSecond, SoundOutput.secondaryBufferSize);
Win32FillSoundBuffer(&SoundOutput, 0, SoundOutput.latencySampleCount*SoundOutput.bytesPerSample);
GlobalSecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);

DWORD PlayCursor;
DWORD WriteCursor;
if(SUCCEEDED(GlobalSecondaryBuffer->GetCurrentPosition(&PlayCursor, &WriteCursor)))
{
    DWORD ByteToLock = ((SoundOutput.runningSampleIndex*SoundOutput.bytesPerSample) %
                        SoundOutput.secondaryBufferSize);

    DWORD TargetCursor = ((PlayCursor + 
        (SoundOutput.latencySampleCount*SoundOutput.bytesPerSample)) %
            SoundOutput.secondaryBufferSize);

    DWORD BytesToWrite;

    if(ByteToLock > TargetCursor)
    {
        BytesToWrite = (SoundOutput.secondaryBufferSize - ByteToLock);
        BytesToWrite += TargetCursor;
    }
    else
    {
        BytesToWrite = TargetCursor - ByteToLock;
    }

    Win32FillSoundBuffer(&SoundOutput, ByteToLock, BytesToWrite);
}



#endif