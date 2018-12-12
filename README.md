# blogcxx

A static blog generator, written in ``C++17``. It is a fork of [blogcpp](https://bitbucket.org/tux_/blogcpp)

## First things first: How can I get it?

> git clone https://bitbucket.org/schorsch_76/blogcxx.

This is my public (but not main repository). It is the just used to publish released versions.

## Fork
As blogcpp is licenced under WTFPL2, i fork it and do what
the f*ck i want. As i rewrote so big chunks, i licence blogcxx
under [GPLv3](https://www.gnu.org/licenses/gpl-3.0.txt).
I think GPL has done much more good than harm
to the open source movement.

WTFPL 2 and GPL3 are compatible.
- [Wikipedia](https://en.wikipedia.org/wiki/WTFPL)
- [Fedora](https://fedoraproject.org/wiki/Licensing:Main?rd=Licensing)

## Why?

blogcpp was heavyly rewritten because there were internal source code
difficulties. ( There was a pattern used called [spagetti code](https://en.wikipedia.org/wiki/Spaghetti_code). )
Take a look at [blogcpp.cpp](https://bitbucket.org/tux_/blogcpp/src/89af0ad6f725f958b99cf1118252895cecde046c/src/blogcpp.cpp?at=default&fileviewer=file-view-default)
can you follow easily where the global varibales are locked? How is the programflow?

Aside from this, it didnt' handle it well when there was no site dir or it was
executed in an empty directory. It just segfaulted with no error message at all.
The setup of a new blogsite was painful if you dont know the internals.
The final reason to rewrite was that it just crashed with no help for the user.
It is a very bad first expierience for the user of a project if it just crashes
without printing anything.

As i tried to send my changes to blogcpp, the author didn't want them because
i use boost and added [cmark](https://github.com/commonmark/cmark) and/or
[cmark-gfm](https://github.com/github/cmark-gfm). Both are to heavy for the author of blogcpp.

## No, seriously, why?

- Just like other static site generators, ``blogcxx`` is virtually
 unbreakable by intruders as your server is not required to host
 anything but static HTML files - even JavaScript and CSS are,
 depending on your theme, entirely optional.
- Unlike other static site generators, ``blogcxx`` is written in modern
 C++, resulting in a fast build speed, reducing the need for
 workarounds like *incremental builds* or advanced caching mechanisms
 while compiling. Still, ``blogcxx`` tries to use as many CPU cores
 as possible to achieve the maximum speed on your machine.
- ``blogcxx`` aims to provide a decent feature set without requiring
 too much configuration; in fact, it would work right out of the box
 if your website was *example.com* (which is, admittedly, rather
 unlikely).

### What changes from a user perspective from blogcpp to blogcxx?

- usage should be much more convienent as it reports its error much better
- syntax and other stuff stays the same :-)
- more stable
- better markdown support with libcmark/libcmark-gfm

## Current version

The latest released version is version 1-alpha

---

### what i am not sure if it works
- highlight programming languages. The generated code looks correct.
- OEmbed. WITHOUT OEMBED set, the simple Media Embedder seems to work.
- Emoji Parser reimplemented but needs futher testing.
  Build regexes in Emojiparser just once and keep state in a
  static (once_flag protected) varaiable.

---

### What is not yet working
- metadata in the HTML (opengraphimg, feedmeta)
- comments (create an own template that gets loaded and parsed instead of
  poluting the source code with html. The job ob the template engine is
  separation of html and control.)
- OEmbed

### TODO list
- RSS (replace tinyxml2 with inja. we already have a templating engine)
- download the highlight.js into the static folder
- WITH_OEMBED: OEmbed (use boost::process to start the subprocess curl or wget.
  This remove the burden from us to compile curl with openssl on windows.
  There are prebuild binaries that get updated regulary. Take advantage of it.)

---

## Features

-  Markdown support: Write your articles and pages in Markdown, get HTML.
-  Theming support: You don't like the available themes? Well, nor do I.
   Go on and make a better one!
-  OEmbed support: Just post an OEmbed-enabled link (e.g. Twitter or
   YouTube) and ``blogcxx`` will embed the media for you.
-  RSS feeds for new articles, tags and categories.
-  Configurable article permalinks.
-  Sticky articles: Just put ``Sticky: On`` on top of your article file.
-  Emoji support: Write ``:-)`` and get a *real* smiling face instead.
-  Commenting support: Even a static blog needs a community. ;-)
-  OpenGraph support if you feel like sharing stuff on Facebook or
   whatever.
-  Syntax highlighting via [highlight.js](https://highlightjs.org/).
-  Article series: If you have articles which continue other articles,
   you can just add them to the same series and ``blogcxx`` will make
   them refer to each other.
-  blogcxx can now create a howto page if no blog exists yet.
   It also contails the default template for that issue. The internal tool
   bin2cxx creates cpp and h files that the cxx arrays get included in
   the executable.

## Features added compared to blogcpp

- switch the markdown parser at compiletime to libcmark/libcmark-gfm but the blogcpp
  markdown parser is still available.
  Markdown syntax used with [cmark](https://commonmark.org/help/)
  Markdown syntax used with [cmark-gfm](https://help.github.com/articles/basic-writing-and-formatting-syntax/)
- blogcxx can now run without an blog.cfg. It uses internal defaults
  (the one from the blog.cfg-example). Just use the blog.cfg file to
  override what you want to change.
  - The ``-help`` options displays also the file options
- handle gracefully UTF BOMS in read files
- use the configured locale everywhere.
- All messages that get printed go through boost::locale::translate().
  That means, i18n will be done in no time. All arguments are positional like %1%.
  different languages can rearrange the order of the parameters.
- Articles can be parsed with hardbreaks on to format code and so on
  Hardbreaks: On (default is off)
- a folder ``images`` gets copied into the outdir.
- colorize output for warn/error/fatal messages.
  - yellow = warning
  - red    = error
  - cyan   = fatal

## Features changed compared to blogcpp
- cxxopts replaced by boost::program_options (users dont care)
- icu replaced by boost::locale (but still used as a backend on Linux) (users dont care)

### other good things
- all compilation is now consistent on all platforms
- static executable size is now just 2-3 MB
- all path handling (including the url pathes) are handled with std::filesystem.
- option added to switch the API for variant(std::variant/boost::variant) and
  filesystem (std::filesystem/std::experimental::filesystem and boost::filesystem)
- option added to link all static
- option added to support testing on older compilers TEST_OLDER_COMPILERS
- Installation option for blogcxx added

## Features removed compared to blogcpp
- js plugins: I think they are unneeded and were used to work around the
  codequality of blogcpp. If you need them, i could create a pluginsystem.
  This pluginsystem would have a js plugin that loads your js plugins.

## Future plans
- remove inja/json to be able to compile with a c++11 compiler.
- remove tinyxml2 and use the templating engine

## Features that might get removed

- OEmbed

---

### Description of the internal changes

In blogcpp global variables were accessed (non const) by multiple threads.
No separation of collection and generation of html. The concurrency is now
like [Map and Reduce](https://en.wikipedia.org/wiki/MapReduce). This is the job of the

``template <typename KeyT, typename RetValT>``
``class AsyncWorker;``

The KeyT is used to identify the results. The RetValT can be void. No more
raw threads. use std::future() and std::async(launch::async,...)

- clangformat everything. Perfectly formatted code makes it easier for
  maintainance and future expansions.
- make a metadata structure that is after the FilesToMetadata phase const
- reduce #ifdef #else #endif over the whole codebase, but still keep the options
  available. This improves readability and maintainable a lot.
- update inja and use exists() and existsIn() in the templates [Inja 1.1.0-pre](https://github.com/pantor/inja/tree/e44c2372e1b70f79e83e8ecbb10d6f99f679a872)
- TemplateWrapper does now what it name implies. It totally encapulates
  json and the inja handling. It uses the so called PIMPL pattern that is also
  known as compiler firewall.
- Restructure the whole src folder. Split the tasks into manageable
  files that are grouped accordig to their function in the process.
- use boost::log and boost::format for logging.

No more

~~~~~
    #ifdef WITH_DEBUGLOG
        stringstream ss_debuglog;
        ss_debuglog << "Gathering ...";
        DebugLog::debuglog(ss_debuglog.str());
    #endif
~~~~~

Now it is typesafe and without sstream everywhere

~~~~~
    // these are variadic templates :-)
    LOG_DEBUG("Gathering files from %1%.", file.string());
~~~~~

---

### What stayed the same between blogcpp and blogcxx?
- Template design
- Markdown Parser (but exchangeable with libcmark)
- Emoji Parser (clear, but parse got reimplemented)
- Excerpt Extractor
- Fileparser (mostly)
- RSS Generator (mostly)
- SingleItem structure (mostly)
- Syntax in the pages/post files

---

## Used libraries

``blogcxx`` uses the following third-party libraries (in ``3rdparty/``):
- [inja](https://github.com/pantor/inja)
- [json](https://github.com/nlohmann/json)
- [TinyXML2](https://github.com/leethomason/tinyxml2)
- *optional* [cmark 0.28.3](https://github.com/commonmark/cmark)
- *optional* [cmark-gfm 0.28.3](https://github.com/github/cmark-gfm)

Not in 3rdparty directory because it is to big.
- [boost](https://www.boost.org) (ICU (as Boost::Locale Backend on Linux))

## Used tools

- cmake
- a ``c++17`` compiler. known to work
  * Visual Studio 2017 Community Edition
  * gcc 8.2
  * clang 6

---

# FAQ

## How can I use ``blogcxx``?

### The easier way:

Just start ``blogcxx`` in your blog directory. It will create a default blog with
an howto article. (basicly this Readme). It will ask you for every step. :-)

Use blog.cfg to override defaults.

- ``blogcxx --new-article`` or ``blogcxx --new-post`` to start a new article or
- ``blogcxx --new-page`` to start a new page. When you're done, run
- ``blogcxx`` and everything will be there soon.

### What is a post, what is a page?
A **post** is an **article** that you post on your blog. It can contain tags, categories
and be part of an series.

A **page** is a **information page**: It is added to the page navigation bar and it is always
there. It get used for download pages, impressum and so on.

### The nerd way:

Set up your __blog.cfg__ file first, follow the documentation in the
``blog.cfg-example`` file in this repository. Then start to add
Markdown-formatted posts (in __/posts__) and/or pages (in __/pages__)
formatted like this:

>    Author: Your name here.                         \
>    Date: 2016-01-01 12:00:00                       \
>    Title: This is the title.                       \
>    Tags: tag 1;tag 2;tag 3                         \
>    Categories: some;random;categories              \
>                                                    \
>    ``----------``                                  \
>                                                    \
>    (Everything that follows will be a part of the contents.)

When you're done, run ``blogcxx`` and everything will be there soon.

### Which meta data types are allowed?

You mean, except the Title/Tags/Date part? Well:

-  You can additionally set ``Markdown: Off`` to disable Markdown parsing
   for this specific post or page. Note that pages don't have categories,
   every category added to a page will be ignored.
-  If you want your newly created article (for logical reasons, only
   posts are supported) to be "sticked" to the top of your index page,
   just add ``Sticky: On`` into its header part.
-  Articles will always be sorted by their *creation date*; you can
   indicate the *latest change* (which will be displayed or not,
   depending on your theme) with ``Changed: yyyy-mm-dd hh:mm:ss``
   somewhere above or below the ``Date:`` field.
-  If you don't want to have emojis on this post or page without
   removing them everywhere, just add ``Emojis: Off``.
-  In case you want to have a specific OpenGraph image for this page or
   article, you can set ``OpenGraphImage: http://my/host/image.png`` or
   whatever you want it to show.
-  You can also disable comments per post (pages don't have comments):
   ``Comments: Off``.
-  If you use Content plug-ins but you want to disable them for this
   item exclusively, you can set ``Plugins: Off``.
-  If you want to add an article to a certain series, it is enough to
   set ``Series: Name Of Your Series`` here. Note that currently only
   one series is supported per article.
-  Pages are usually sorted alphabetically. If you don't want to do that,
   you can set the ``Position`` parameter with any number larger than 0.
   Note that the "start page" is not a real page, so the template
   decides where to put it.
-  Articles can be parsed with hardbreaks on to format code and so on
   Hardbreaks: On (default is off)

---

### OK, but how can I compile ``blogcxx`` first?

If you're on Windows, you can just [get a static binary](http://blogcxx.org/pages/download). There are also
[precompiled builds for Debian GNU/Linux](https://blog.mdosch.de/2017/08/26/blogc++-in-debian-benutzen/)
which have, sadly, not been updated in a while.
It's not hard to compile ``blogcxx`` yourself though:

Boost API is most suitable for 'older' compilers like gcc 6.3 and will build on almost every compiler.

``blogcxx`` has been proven to compile on:

  OS      | Compiler   | Status | filesystem | variant
 ---------|------------|--------|------------|---------
  Windows | MSVC 2017  | OK     | std        | std
  Windows | MSVC 2017  | OK     | std::exp   | std
  Windows | MSVC 2017  | OK     | boost      | std
  Windows | MSVC 2017  | OK     | std        | boost
  Windows | MSVC 2017  | OK     | std::exp   | boost
  Windows | MSVC 2017  | OK     | boost      | boost
  Windows | MSYS  8.2  |        | std        | std
  Windows | MSYS  8.2  |        | std::exp   | std
  Windows | MSYS  8.2  |        | boost      | std
  Windows | MSYS  8.2  |        | std        | boost
  Windows | MSYS  8.2  |        | std::exp   | boost
  Windows | MSYS  8.2  | OK     | boost      | boost
  Stretch | gcc 6.3    | FAIL   | std        | std
  Stretch | gcc 6.3    | FAIL   | std::exp   | std
  Stretch | gcc 6.3    | FAIL   | boost      | std
  Stretch | gcc 6.3    | FAIL   | std        | boost
  Stretch | gcc 6.3    | FAIL   | std::exp   | boost
  Stretch | gcc 6.3    | FAIL   | boost      | boost
  Buster  | gcc 8.2    | OK     | std        | std
  Buster  | gcc 8.2    | OK     | std::exp   | std
  Buster  | gcc 8.2    | OK     | boost      | std
  Buster  | gcc 8.2    | OK     | std        | boost
  Buster  | gcc 8.2    | OK     | std::exp   | boost
  Buster  | gcc 8.2    | OK     | boost      | boost
  FBSD 12 | clang 6    | OK     | std        | std
  FBSD 12 | clang 6    |        | std::exp   | std
  FBSD 12 | clang 6    |        | boost      | std
  FBSD 12 | clang 6    |        | std        | boost
  FBSD 12 | clang 6    |        | std::exp   | boost
  FBSD 12 | clang 6    |        | boost      | boost

Building on Stretch fails because of inja/json. :-(

---

###  Build on Windows:
Requirements:
* Visual Studio 2017
* cmake 3.13
* Boost 1.68.0

Manual compilation instructions:
- Get boost from https://www.boost.org/ and unzip it to your build directory.
- Start a Commandline prompt from
  ``Visualstudio VS2017 x64 Native Tools-Commandprompt``
- cd into .../boost-1.68.0
- bootstrap.bat
- ``b2 -j8 --build-type=complete address-model=64 --stagedir=stage64 stage``
  Depending on your machine it takes about 15 min.

Start cmake gui:
- setup source directory to .../blogcxx-src and build directory to
  .../blogcxx-src/build
- configure: choose Visualstudio 2017 x64
- Add an Entry: BOOST_ROOT (path) pointing to .../boost-1.68.0
- Add an Entry: BOOST_LIBRARYDIR (path) pointing to .../boost-1.68.0/stage64
- Press generate.
- you can choose the options and generate again  (options see below)
  Here i recommend STATIC_LINKING=ON (just on windows)
- press "Open project"
- choose the build type. Probally Release
- build

---

### Build on Linux/FreeBSD:
Requirements:
* clang6 or gcc 8.2
* cmake 3.9
* Boost 1.62.0

#### Build boost 1.68.0 on Linux

Install cmake and gcc/clang from your package manager.

Build boost:

- cd ..../boost/1.68.0
- ./bootstrap.sh
- time ./b2 -j8 --build-type=complete --layout=tagged  --stagedir=stage64 address-model=64 stage

This took on my machine (Amd 8350) about 8 min
3179,97s user 212,00s system 730% cpu 7:44,02 total

build blogcxx

- cd .../blogcxx-src
- mkdir build
- cd build
- export CC=/usr/bin/clang-6.0
- export CXX=/usr/bin/clang++-6.0
- cmake 	-DBOOST_ROOT=${HOME}/Dokumente/boost/1.68.0 \
	-DBOOST_INCLUDEDIR=${HOME}/Dokumente/boost/1.68.0 \
	-DBOOST_LIBRARYDIR=${HOME}/Dokumente/boost/1.68.0/stage64/lib \
	-DBoost_NO_SYSTEM_PATHS=ON \
	-DCMAKE_BUILD_TYPE=Debug \
	-DWITH_DEBUG_LOG=ON \
	..
- ccmake .. (now choose your options)
- time make -j8 clean all

It took again on my machine (Amd 8350) about 1 min
make -j8 clean all  278,61s user 7,67s system 520% cpu 55,023 total

---

### Optional preprocessor definitions while compiling:

#### MARKDOWN_API
* ``blogcxx`` is the traditional and default MarkdownParser.
* ``libcmark`` is the reference implementation.
* ``libcmark-gfm`` is the implementation used at github with table support

#### BOOST_LOG_API
Boost Log Backend API (just on windows)
* ``BOOST_WINAPI_VERSION_WINXP``
* ``BOOST_WINAPI_VERSION_WIN7``

#### FILESYSTEM_API
  * ``std::filesystem``
  * ``std::experimental::filesystem``
  * ``boost::filesystem``

#### VARIANT_API
  * std::variant
  * boost::variant

#### STATIC_LINKING
Create a static binary.

#### WITH_OEMBED
``blogcxx`` supports the OEmbed standard. This will in some cases affect
the page generation speed. (If not used,``blogcxx`` supports simple
replacements for YouTube and Vimeo only.)

#### TEST_OLDER_COMPILERS
This is just an develop option. Please let it off. If on, it disables
all ``c++17`` support. That also means, no output >:-) . This changes
*soon* as i plan to replace json/inja with boost::spirit. The template
is a [CFG](https://en.wikipedia.org/wiki/Context-free_grammar) grammar.


## Which directories need to be there?

Here's a site's minimal directory structure:

- ./templates/TPLNAME/
- ./INDIR/posts/
- ./INDIR/pages/
- ./OUTDIR/

This directory structure can be created by blogcxx for you :-)

Of course, the capital letters mean that the values are indeed
variables. By default, **TPLNAME** is **default**, **INDIR** is **site**
and **OUTDIR** is **outdir**. Please use the configuration file when
building your site in order to change these values.

## How does syntax highlighting work?

Four spaces at the beginning of a line mark the beginning of a code
block. ``blogcxx`` tries to determine the language of the code
automatically. If you want to set the language explicitly, you can use
the [Pelican](http://getpelican.com) syntax for code blocks; example:

>
     ::::python
     print("This is Python code.")

## Which parameters are supported?

You can call ``blogcxx`` with the following parameters:

-  ``-h`` / ``--help``
	* Prints this list of parameters. It contains the description of all parameters which
	  are part of the ``blog.cfg-example``
-  ``-v`` / ``--version``
	* Prints the version number of ``blogcxx``.
-  ``--verbosity``
	* Generates your site, but set the amount of info you want to see.
		Quiet = 5 / Verbose = 0
-  ``--new-article``
	* Generates a new article stub and opens it in your default editor.
-  ``--new-page``
	* Generates a new page stub and opens it in your default editor.
-  ``--config [filename]``
	* Uses a different configuration file than blog.cfg. The file must exist.

## Which emojis are supported?

Given that you have *actually enabled* Emoji support in your
``blog.cfg`` file and not disabled it for the page or article you want
to process, the following smiley codes will automatically be replaced:


 Code      | Emoji
 ----------|-----------
 ``:-)``    | ☺
 ``;-)``    | 😉
 ``:-D``    | 😀
 ``:-(``    | ☹
 ``:'(``    | 😭
 ``:-|``    | 😐
 ``>:)``    | 😈
 ``>:-)``   | 😈
 ``>:(``    | 😠
 ``>:-(``   | 😠
 ``:-*``    | 😘
 ``:-O``    | 😮
 ``:-o``    | 😮
 ``:-S``    | 😕
 ``:-s``    | 😕
 ``:-#``    | 😶
 ``0:-)``   | 😇
 ``:o)``    | 🤡
 ``<_<``    | 😒
 ``^^``     | 😊
 ``^_^``    | 😊
 ``<3``     | ❤
 ``m(``     | 🤦


## Which comment systems does ``blogcxx`` support?

While ``blogcxx`` does not have its own commenting system, you can
easily integrate existing external commenting systems via your
``blog.cfg`` (please refer to the ``blog.cfg-example`` file in this
repository). Currently supported are:

-  [Disqus](http://www.disqus.com) (``comments = disqus``, you need
   to set your ``commentdata`` to your Disqus shortname)
-  [isso](http://posativ.org/isso) (``comments = isso``, you need
   to set your ``commentdata`` to your *isso* folder)
-  [Hashover](https://github.com/jacobwb/hashover-next)
   (``comments = hashover``, you need to set your ``commentdata`` to
   your *Hashover* folder) -- currently, version 2.0 only
-  Google Plus comments (``comments = google``, no additional settings
   required)
-  [Discourse](http://discourse.org) (``comments = discourse``, you
   need to set your ``commentdata`` to your *Discourse* domain)
-  [Commento](https://gitlab.com/commento/commento-ce) (``comments = commento``,
   you need to set your ``commentdata`` to your *Commento* server URI)


## Can I use raw HTML in my Markdown-enabled article or page?

### blogcpp markdown parser
Yes, you can! Everything between ``<rawhtml>`` and ``</rawhtml>`` will
be ignored by blogcpp Markdown parser.

### cmark/cmark-gfm markdown parser
[Inline HTML with cmark-gfm](https://daringfireball.net/projects/markdown/syntax#html)

In short, just add the raw html to the Markdown and it will be embedded. It
must just be correct. Opening and closing divs and so on.


## Which files does a template need to work?

``blogcxx`` needs **index.txt**, **post.txt**, **page.txt** and
**archives.txt** in order to be able to process your site correctly. All
other template files are optional. CSS and image files can be put into a
subfolder named **static**, ``blogcxx`` will automatically copy this
folder to your output directory then.

Starting with version 1, ``blogcxx``'s templates are *mostly* compatible
with the [Jinja2](http://jinja.pocoo.org/) syntax, so porting your
existing Python themes should be rather easy.
