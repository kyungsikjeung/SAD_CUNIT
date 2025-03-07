## Unit test RUN
 F5(Launch)



## launch.json ex
파일에서 preLaunchTask 필드를 build-and-post-build 작업으로 설정하여, 디버깅 세션이 시작되기 전에 build 작업과 post-build 작업이 순차적으로 실행

```code
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug C",
            "type": "cppdbg",
            "request": "launch",
            "console": "integratedTerminal",
            "program": "${fileDirname}\\${fileBasenameNoExtension}.exe",
            "args": ["arg1", "arg2", "arg3", "333"],
            "stopAtEntry": false,
            "cwd": "${fileDirname}",
            "environment": [],
            "externalConsole": true,
            "MIMode": "gdb",
            "miDebuggerPath": "C:\\MinGW\\bin\\gdb.exe",
            "preLaunchTask": "build-and-post-build",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ],
            "internalConsoleOptions": "neverOpen",
            "logging": {
                "moduleLoad": false,
                "engineLogging": false,
                "trace": false,
                "traceResponse": false
            }
        }
    ]
}
```


# Task Json ex
``` code 
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "build",
            "type": "shell",
            "command": "gcc",
            "args": [
                "-g",
                "${file}",
                "-o",
                "${fileDirname}\\${fileBasenameNoExtension}.exe"
            ],
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "problemMatcher": ["$gcc"]
        },
        {
            "label": "post-build",
            "type": "shell",
            "command": "echo",
            "args": [
                "빌드가 완료되었습니다."
            ],
            "problemMatcher": []
        },
        {
            "label": "build-and-post-build",
            "dependsOn": ["build", "post-build"],
            "problemMatcher": []
        }
    ]
}

```