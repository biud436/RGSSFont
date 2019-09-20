# Features

- A Decryption of the encrypted game resources is illegal, so this tool does not rewrite script files and it will be going to change the default font and font size by injecting the 'RSFont.dll' file during the runtime. so the changed font is not permanent.
- The one core principle of changing the font is to override the class variables named 'Font.default_name' and 'Font.default_size' and some constants. but actually, some fonts may not be changed.
- This has been implemented a way to inject the DLL file directly into the target process(Game.exe), so the anti-virus program may diagnose it as the malware. But it is not malware.
- it is only tested in Windows 10.
- You could use any font that is in the Windows' Fonts folder excluding the System Font is already using.