#if 0
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

DWORD xinputState;    
// i = controller index
for (DWORD i=0; i < XUSER_MAX_COUNT; i++ )
{

    XINPUT_STATE state;
    if(XInputGetStateFunction( i, &state ) == ERROR_SUCCESS)
    {
        XINPUT_GAMEPAD *gamepad = &state.Gamepad;
        bool up = (gamepad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
        bool down = (gamepad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
        bool left = (gamepad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
        bool right = (gamepad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
        bool start = (gamepad->wButtons & XINPUT_GAMEPAD_START);
        bool back = (gamepad->wButtons & XINPUT_GAMEPAD_BACK);
        bool leftShoulder = (gamepad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
        bool rightShoulder = (gamepad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
        bool AButton = (gamepad->wButtons & XINPUT_GAMEPAD_A);
        bool BButton = (gamepad->wButtons & XINPUT_GAMEPAD_B);
        bool XButton = (gamepad->wButtons & XINPUT_GAMEPAD_X);
        bool YButton = (gamepad->wButtons & XINPUT_GAMEPAD_Y);

        i16 stickX = gamepad->sThumbLX;
        i16 stickY = gamepad->sThumbLY;

        offset.x += stickX / 4096 / 2;
        offset.y -= stickY / 4096 / 2;

        // SoundOutput.toneHz = 512 + (int)(256.0f*((float)stickY / 30000.0f));
        // SoundOutput.wavePeriod = SoundOutput.samplesPerSecond/SoundOutput.toneHz;
    }
    else
    {
        OutputDebugStringA("controller not connected\n");
    }
}


internal void 
Win32LoadXInput(void)    
{
    HMODULE XInputLibrary = LoadLibraryA("xinput1_4.dll");

    if(!XInputLibrary)
    {
        XInputLibrary = LoadLibraryA("xinput9_1_0.dll");
        OutputDebugStringA("xinput1_4.dll Failed");
    }

    if(!XInputLibrary)
    {
        XInputLibrary = LoadLibraryA("xinput1_3.dll");
        OutputDebugStringA("xinput9_1_0.dll Failed");
    }

    if(XInputLibrary)
    {
        XInputGetStateFunction = (x_input_get_state *)GetProcAddress(XInputLibrary, "XInputGetState");
        if(!XInputGetStateFunction) {XInputGetStateFunction = XInputGetStateStub;}

        XInputSetStateFunction = (x_input_set_state *)GetProcAddress(XInputLibrary, "XInputSetState");
        if(!XInputSetStateFunction) {XInputSetStateFunction = XInputSetStateStub;}

        // TODO:
    }
    else
    {
        // TODO: 
    }
}