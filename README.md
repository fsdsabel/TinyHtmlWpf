# TinyHtml.Wpf
## Introduction
TinyHtml.Wpf is a WPF HTML rendering library based on [litehtml](https://github.com/litehtml/litehtml).
It provides the rendering backend for WPF. Contrary to similar libraries it uses native WPF drawing to 
render texts, images and other stuff. So it is fully capable of doing high dpi and blends seemlessly with
other content.

Originally it was developed by me to replace HTMLayout that we used before. However text rendering
was always blocky and didn't blend well with other WPF content. We use this extensively for dynamic formatted text that 
is authored as HTML.

This library does not provide scripting capabilities or very advanced HTML. It proved to be quite
capable nonetheless.

A [Nuget package](https://www.nuget.org/packages/TinyHtml.Wpf) can be found on nuget.org. It supports
x86 and x64 (or AnyCPU) applications.

## Requirements
- .NET 4.6
- A WPF application
- Visual Studio 2017 for building

## Supported features
- WPF Text rendering
- HTML Tables
- HTML Lists
- Images
- css
- and much more...

## Building
1. Get sources from https://github.com/fsdsabel/TinyHtmlWpf:
<br/><pre><code>git clone --recursive https://github.com/fsdsabel/TinyHtmlWpf.git</code></pre>
2. Open the Solution TinyWpfHtml.sln in Visual Studio 2017
3. Compile configuration Release - x86
4. Compile configuration Release - x64
5. Binaries should be in **Bin** directory and **HtmlTestApp** should run

## Usage
Basically there is only one class you need. It is `WpfHtmlControlBase` which is subclassed from a standard
WPF `Control`. 

Before you can use the control, you must give it a master style sheet. This style is used for all
the default HTML elements like paragraphs or bold elements. You can either use the master.css contained in
the Nuget package or modify it to your liking. For example you could do something like this:

```csharp
public class WpfHtmlControl : WpfHtmlControlBase
{
    static WpfHtmlControl()
    {
        using (var s = typeof (WpfHtmlControl).Assembly.GetManifestResourceStream(typeof (WpfHtmlControl), "master.css"))
        {
            SetMasterStylesheet(new StreamReader(s).ReadToEnd());
        }
    }
}
```

You would then use `WpfHtmlControl` in your application. The most basic usage would be:

```xml
<local:WpfHtmlControl Background="Blue" Foreground="Red" FontSize="16" 
                      Html="&lt;b&gt;Hello world!&lt;/b&gt;"></local:WpfHtmlControl>
```

It is possible to bind to the `Html` property to update the displayed HTML dynamically.

If you want to display images, you have to override `OnLoadResource`.

## License
Copyright (c) 2017, Daniel Sabel  
All rights reserved.

Redistribution and use in source and binary forms, with or without  
modification, are permitted provided that the following conditions are met:  
- Redistributions of source code must retain the above copyright  
      notice, this list of conditions and the following disclaimer.  
- Redistributions in binary form must reproduce the above copyright  
      notice, this list of conditions and the following disclaimer in the  
      documentation and/or other materials provided with the distribution.  
- Neither the name of the <organization> nor the  
      names of its contributors may be used to endorse or promote products  
      derived from this software without specific prior written permission.  

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND  
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED  
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE  
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY  
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES  
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;  
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND  
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT  
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS  
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

**This library uses litehtml:**

Copyright (c) 2013, Yuri Kobets (tordex)  
All rights reserved.
