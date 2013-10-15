Name
====

ngx-http-status-code-counter - Nginx module to keep a count of sent HTTP status codes

This is a fairly simple module for the nginx webserver that simply keeps a count of HTTP status codes. I made it to feed data to the included munin plugin so I can keep track of changes in http status code distributions.

Most of the code is based on the http-stub-status module that comes with the nginx distribution.

This is my first Nginx module and it is still very early. Please feel free to play around with it and/or submit patches!

I've included a sample munin plugin that uses ruby and the excellent munin_plugin gem.

Example Config
==============

    server {
        location /status-codes {
            show_status_code_count on;
        }
    }
    
Sample Output (in JSON)
=============
    {"200":196333,"204":25244311,"302":614,"404":3,"499":396943}

Installation
============

1. Download this module or clone it.
1. Get the nginx source code from [wiki.nginx.org](http://wiki.nginx.net/). I've only used this module so far with 0.8.50

        $ wget 'http://sysoev.ru/nginx/nginx-0.8.50.tar.gz'
        $ tar -xzvf nginx-0.8.50.tar.gz
        $ cd nginx-0.8.50/
        
        # Here we assume you would install you nginx under /opt/nginx/.
        $ ./configure --prefix=/opt/nginx \
            --add-module=/path/to/ngx_http_status_code_counter
        
        $ make
        $ make install

Compatibility
=============

This has only been tried on 0.8.x, not sure if it works with 0.7.x

*   0.8.x (last tested version is 0.8.50)

Todo
====

* Figure out how to write some tests for this. I don't even
* Figure out how to store the count array better. Dynamic arrays in pure C are painful :-(

Author
=======

* Kennon Ballou <kennon at angryturnip dot com>

Copyright & License
===================

    This module is licenced under the BSD license.

    Copyright (C) 2010, Kennon Ballou.

    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

        * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

        * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

        * Neither the name of the Taobao Inc. nor the names of its
        contributors may be used to endorse or promote products derived from
        this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
    A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
    TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
    PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
    NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
