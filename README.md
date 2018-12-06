# blogcpp

A static blog generator, written in ``C++17``.

## First things first: How can I get it?

1. You can use [SourceTree](http://sourcetreeapp.com/), just add this
   repository's URL, clone it and you're done.
2. You can use [SmartHg](http://www.syntevo.com/smartgit/), just add
   this repository's URL, clone it and you're done.
3. You can use [TortoiseHg](http://tortoisehg.bitbucket.org/), just
   add this repository's URL, clone it and you're done.
4. You can use the command-line Mercurial client:
   hg clone https://bitbucket.org/tux_/blogcpp.

## Why?

Because there are not enough [static site generators](http://www.staticsitegenerators.net) yet.

## No, seriously, why?

-  Just like other static site generators, ``blogcpp`` is virtually
   unbreakable by intruders as your server is not required to host
   anything but static HTML files - even JavaScript and CSS are,
   depending on your theme, entirely optional.
-  Unlike other static site generators, ``blogcpp`` is written in modern
   C++, resulting in a fast build speed, reducing the need for
   workarounds like *incremental builds* or advanced caching mechanisms
   while compiling. Still, ``blogcpp`` tries to use as many CPU cores
   as possible to achieve the maximum speed on your machine.
-  ``blogcpp`` aims to provide a decent feature set without requiring
   too much configuration; in fact, it would work right out of the box
   if your website was *example.com* (which is, admittedly, rather
   unlikely).

## Current version

The latest released version is version 10-alpha

## Features

-  Markdown support: Write your articles and pages in Markdown, get HTML.
-  Theming support: You don't like the available themes? Well, nor do I.
   Go on and make a better one!
-  OEmbed support: Just post an OEmbed-enabled link (e.g. Twitter or
   YouTube) and ``blogcpp`` will embed the media for you.
-  RSS feeds for new articles, tags and categories.
-  Configurable article permalinks.
-  Sticky articles: Just put ``Sticky: On`` on top of your article file.
-  Emoji support: Write ``:-)`` and get a *real* smiling face instead.
-  Commenting support: Even a static blog needs a community. ;-)
-  OpenGraph support if you feel like sharing stuff on Facebook or
   whatever.
-  Syntax highlighting via [highlight.js](https://highlightjs.org/).
-  Plug-in support: If anything's missing, just write a script.
-  Article series: If you have articles which continue other articles,
   you can just add them to the same series and ``blogcpp`` will make
   them refer to each other.
-  blog cpp can now create a howto page if no blog exists yet.
   It also contails the default template for that issue. The internal tool
   bin2cxx creates cpp and h files that the cxx arrays get included in
   the executable.
-  blogcpp can now run without an blog.cfg. It uses internal defaults
   (the one from the blog.cfg-example). Just use the blog.cfg file to
   override what you want to change
-  handle gracefully UTF BOMS in read files
-  At compilation time you can choose to use the libcmark Markdownparser.
   For compatibility you can also use the traditional blogcpp Markdownparser.
-  much more is planned - see `the
   [roadmap](https://blogcpp.org/pages/roadmap) for a clue.

---

## Major rewrite from blogcpp 9 -> 10

### Rewrite of blogcpp
blogcpp was heavyly rewritten because there were internal source code
difficulties. ( There was a pattern used called spagetti code. ) It didnt'
handle it well when there was no site dir or it was executed in an empty
directory. It just segfaulted with no error message at all. The setup of a new
blogsite was painful if you dont know the internals.
The final reason to rewrite was that it just crashed with no help for the user.
It is a very bad first expierience for the user of a project if it just crashes
without printing anything.

---

### What was changed? Why was this neccesary?
- **rewrite blogcpp.cpp**
> It was total messy. Global variables that were accessed (non const) by
multiple threads. No separation of collection and generation of html. The
concurrency is now like [Map and Reduce](https://en.wikipedia.org/wiki/MapReduce).
This is the job of the

    ::::cpp
    template <typename KeyT, typename RetValT>
    class AsyncWorker;

> The KeyT is used to identify the results. The RetValT can be void. No more
raw threads. use std::future() and std::async(launch::async,...)

- clangformat everything. Perfectly formatted code makes it easier for
  maintainance and future expansions.
- make a metadata structure that is after the FilesToMetadata phase const
- All messages that get printed go through boost::locale::translate().
  That means, i18n will be done in no time. All arguments are positional like %1%.
- cxxopts replaced by boost::program_options
- icu replaced by boost::locale (but still used as a backend on UNIX)
- reduce #ifdef #else #endif over the whole codebase, but still keep the options
  available.
- update inja and use exists() and existsIn() in the templates [Inja 1.1.0-pre](https://github.com/pantor/inja/tree/e44c2372e1b70f79e83e8ecbb10d6f99f679a872)
- TemplateWrapper does now what it name implies. It totally encapulates
  json and the inja handling. It uses the so called PIMPL pattern that is also
  known as compiler firewall.
- Restructure the whole src folder. Split the tasks into manageable
  files that are grouped accordig to their function in the process.
- use boost::log and boost::format for logging.

> No more

    ::::cpp
    #ifdef WITH_DEBUGLOG
        stringstream ss_debuglog;
        ss_debuglog << "Gathering ...";
        DebugLog::debuglog(ss_debuglog.str());
    #endif

> Now it is typesafe and without sstream everywhere

    ::::cpp
	LOG_DEBUG("Gathering files from %1%.", file.string()); // these are variadic templates :)

---

### What changes from a user perspective?
- usage should be much more convienent as it reports its error much better
- syntax and other stuff stays the same :-)
- more stable

---

### What stayed the same?
- Template design
- Markdown Parser (but exchangeable with libcmark)
- Emoji Parser
- Fileparser
- RSS Generator (mostly)
- SingleItem structure (mostly)
- Syntax in the pages/post files

---

### what i am not sure if it works
- highlight programming languages. The generated code looks correct.
- OEmbed. WITHOUT OEMBED set, the simple Media Embedder seems to work.
- RSS: Are the xml names right?

---

### what is not yet working
- metadata in the HTML (opengraphimg, feedmeta)
- js plugins (headscripts)
- comments (create an own template that gets loaded and parsed instead of
  poluting the source code with html. The job ob the template engine is
  separation of html and control.)
- pagination (but this was not working on master either)

### TODO list
- RSS (replace tinyxml2 with inja. we already have a templating engine)
- download the highlight.js into the static folder
- WITH_OEMBED: OEmbed (use boost::process to start the subprocess curl or wget.
  This remove the burden from us to compile curl with openssl on windows.
  There are prebuild binaries that get updated regulary. Take advantage of it.)

### good things
- all compilation is now consistent on all platforms
- static executable size is now just 2-3 MB
- all path  handling (including the url pathes) are handled with std::filesystem.
- option added to switch the API for variant(std::variant/boost::variant) and
  filesystem (std::filesystem/std::experimental::filesystem and boost::filesystem)
- option added to link all static
- option added to support testing on older compilers TEST_OLDER_COMPILERS
- Installation option for logcpp added

## Used libraries

``blogcpp`` uses the following third-party libraries (in ``3rdparty/``):

-  [inja](https://github.com/pantor/inja)
-  *optional* [cmark 0.28.3](https://github.com/commonmark/cmark)
-  [json](https://github.com/nlohmann/json)
-  [TinyXML2](https://github.com/leethomason/tinyxml2)
-  [dukglue](https://github.com/Aloshi/dukglue) with [duktape](http://duktape.org/)
-  ICU (as Boost::Locale Backend on UNIX)

Not in 3rdparty because it is to big. Grab it from https://www.boost.org
-  [boost](https://www.boost.org)

## Used tools

- cmake
- a ``c++17`` compiler. known to work
  * Visual Studio 2017 Community Edition
  * gcc 8.2
  * clang 6

---

# FAQ

## How can I use ``blogcpp``?

### The easier way:

Just start ``blogcpp`` in your blog directory. It will create a default blog with
an howto article. (basicly this Readme). It will ask you for every step. :-)

Use blog.cfg to override defaults.

- ``blogcpp --new-article`` to start a new article or
- ``blogcpp --new-page`` to start a new page. When you're done, run
- ``blogcpp`` and everything will be there soon.

### The nerd way:

Set up your __blog.cfg__ file first, follow the documentation in the
``blog.cfg-example`` file in this repository. Then start to add
Markdown-formatted posts (in __/posts__) and/or pages (in __/pages__)
formatted like this:

>
    Author: Your name here.
    Date: 2016-01-01 12:00:00
    Title: This is the title.
    Tags: tag 1;tag 2;tag 3
    Categories: some;random;categories

    ----------

    (Everything that follows will be a part of the contents.)

When you're done, run ``blogcpp`` and everything will be there soon.

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

---

### OK, but how can I compile ``blogcpp`` first?

If you're on Windows, you can just [get a static binary](http://blogcpp.org/pages/download). There are also
[precompiled builds for Debian GNU/Linux](https://blog.mdosch.de/2017/08/26/blogc++-in-debian-benutzen/)
which have, sadly, not been updated in a while.
It's not hard to compile ``blogcpp`` yourself though:

``blogcpp`` has been proven to compile on:
- Windows 10 with Visual Studio 2017.
- Windows 10 with Clang 6.0.1.
- Debian GNU/Linux Buster with **g++ 8.2**.
- FreeBSD 12 with clang

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
- setup source directory to .../blogcpp-src and build directory to
  .../blogcpp-src/build
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
* Boost 1.68.0
  Building with earlier versions of boost done have the function:

>
  template<typename charT = char>
      BOOST_PROGRAM_OPTIONS_DECL basic_parsed_options< charT >
      parse_config_file(const char *, const options_description &,
                        bool = false);

I tried to use the ifstream version but the parsing of our config options
failed. :-(

#### Build boost 1.68.0 on Linux

Install cmake and gcc/clang from your package manager.

Build boost:
>
cd ..../boost/1.68.0
./bootstrap.sh
time ./b2 -j8 --build-type=complete --layout=tagged  --stagedir=stage64 address-model=64 stage

This took on my machine (Amd 8350) about 8 min
3179,97s user 212,00s system 730% cpu 7:44,02 total

build blogcpp
>
cd .../blogcpp-src
mkdir build
cd build
export CC=/usr/bin/clang-6.0
export CXX=/usr/bin/clang++-6.0
cmake 	-DBOOST_ROOT=${HOME}/Dokumente/boost/1.68.0 \
	-DBOOST_INCLUDEDIR=${HOME}/Dokumente/boost/1.68.0 \
	-DBOOST_LIBRARYDIR=${HOME}/Dokumente/boost/1.68.0/stage64/lib \
	-DBoost_NO_SYSTEM_PATHS=ON \
	-DCMAKE_BUILD_TYPE=Debug \
	-DWITH_DEBUG_LOG=ON \
	..
ccmake .. (now choose your options)
time time make -j8 clean all

It took again on my machine (Amd 8350) about 1 min
make -j8 clean all  278,61s user 7,67s system 520% cpu 55,023 total

---

### Optional preprocessor definitions while compiling:

#### MARKDOWN_API
* ``blogcpp`` is the traditional and default MarkdownParser.
* ``libcmark`` is the reference implementation.

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

#### WITH_DEBUGLOG
Prints verbose debug output to ``debug.txt``.

#### WITH_PLUGINS
Actually supports loading and evaluating plug-ins.

#### WITH_OEMBED
``blogcpp`` supports the OEmbed standard. This will in some cases affect
the page generation speed. (If not used,``blogcpp`` supports simple
replacements for YouTube and Vimeo only.)

#### TEST_OLDER_COMPILERS
This is just an develop option. Please let it off. If on, it disables
all ``c++17`` support. That also means, no output >:-)


### Boost API is most suitable for 'older' compilers like gcc 6.3 and will build on almost every compiler.


## Which directories need to be there?

Here's a site's minimal directory structure:
>
    ./blogcpp
    ./templates/TPLNAME/
    ./INDIR/posts/
    ./INDIR/pages/
    ./OUTDIR/

This directory structure can be created by blogcpp for you :-)

Of course, the capital letters mean that the values are indeed
variables. By default, **TPLNAME** is **default**, **INDIR** is **site**
and **OUTDIR** is **outdir**. Please use the configuration file when
building your site in order to change these values.

## The name is stupid!

Well, I am a developer, not an economist. I do software, not shiny
product names. However, ``blogcpp`` is path-agnostic (as long as it
finds its usual path structure). You think ``blogcpp`` is a stupid name?
``mv blogcpp klaus`` and blog with ``klaus``!

Also, please consider to ask a real question next time.

## How does syntax highlighting work?

Four spaces at the beginning of a line mark the beginning of a code
block. ``blogcpp`` tries to determine the language of the code
automatically. If you want to set the language explicitly, you can use
the [Pelican](http://getpelican.com) syntax for code blocks; example:

>
     ::::python
     print("This is Python code.")

## Which parameters are supported?

You can call ``blogcpp`` with the following parameters:

-  ``-h`` / ``--help``
	* Prints this list of parameters. It contains the description of all parameters which
	  are part of the ``blog.cfg-example``
-  ``-v`` / ``--version``
	* Prints the version number of ``blogcpp``.
-  ``--verbosity``
	* Generates your site, but set the amount of info you want to see.
		Quiet = 5 / Verbose = 0
-  ``--new-article``
	* Generates a new article stub and opens it in your default editor.
-  ``--new-page``
	* Generates a new page stub and opens it in your default editor.
-  ``--config [filename]``
	* Uses a different configuration file than blog.cfg. The file must exist.

If compiled with ``WITH_PLUGINS``. three more parameters are supported:

-  ``--list-plugins``
	* Shows a list of all available plug-ins with their enabled/disabled status.
-  ``--enable-plugin [name]``
	* Enables the given plug-in (if found).
-  ``--disable-plugin [name]``
	* Disables the given plug-in (if found).

## Which emojis are supported?

Given that you have *actually enabled* Emoji support in your
``blog.cfg`` file and not disabled it for the page or article you want
to process, the following smiley codes will automatically be replaced:

+------------+---------+
| Code       | Emoji   |
+============+=========+
| ``:-)``    | ☺       |
+------------+---------+
| ``;-)``    | 😉      |
+------------+---------+
| ``:-D``    | 😀      |
+------------+---------+
| ``:-(``    | ☹       |
+------------+---------+
| ``:'(``    | 😭      |
+------------+---------+
| ``:-|``    | 😐      |
+------------+---------+
| ``>:)``    | 😈      |
+------------+---------+
| ``>:-)``   | 😈      |
+------------+---------+
| ``>:(``    | 😠      |
+------------+---------+
| ``>:-(``   | 😠      |
+------------+---------+
| ``:-*``    | 😘      |
+------------+---------+
| ``:-O``    | 😮      |
+------------+---------+
| ``:-o``    | 😮      |
+------------+---------+
| ``:-S``    | 😕      |
+------------+---------+
| ``:-s``    | 😕      |
+------------+---------+
| ``:-#``    | 😶      |
+------------+---------+
| ``0:-)``   | 😇      |
+------------+---------+
| ``:o)``    | 🤡      |
+------------+---------+
| ``<_<``    | 😒      |
+------------+---------+
| ``^^``     | 😊      |
+------------+---------+
| ``^_^``    | 😊      |
+------------+---------+
| ``<3``     | ❤       |
+------------+---------+
| ``m(``     | 🤦      |
+------------+---------+

## Which comment systems does ``blogcpp`` support?

While ``blogcpp`` does not have its own commenting system, you can
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


## How can I write a ``blogcpp`` plug-in? Is it hard?

Good news: ``blogcpp`` *plug-ins* are actually JavaScript scripts,
meaning that it is rather easy to write one. *Everyone* can write
JavaScript today, right?

The plug-in mechanisms are still in an early phase, they will probably
be more extended in later releases. As of now, ``blogcpp`` only supports
three kinds of plug-ins:

-  **Content plug-ins:** Will be searched under ``<plugindir>/contents``.
   ``blogcpp`` will draw your contents (articles *and* pages) through all
   available scripts and spit out the results. Note that this happens
   during the initial reading stadium, so your scripts will see the raw
   Markdown text. The ``process()`` method is expected to take and return
   a string value.
-  **Header plug-ins:** Will be searched under ``<plugindir>/header``.
   The ``process()`` method is expected to return a string value which
   will be attached to the ``{{ headscripts }}`` template variable (if
   applicable).
-  **Footer plug-ins**: Similar to header plug-ins, but they should be
   in ``<plugindir>/footer`` and fill the ``{{ footscripts }}`` variable.

The standard naming scheme is *something.plugin.js*, ``blogcpp`` will
gladly ignore any files which do *not* have a name like this. You are also
able to have a multi-part plug-in which affects multiple parts of the
blog: Plug-ins with an identical file name are considered to belong
together, ``blogcpp`` will handle them as one big plug-in which affects
various parts of the software, e.g. ``<plugindir>/contents/moo.plugin.js``
for moo'ing in the contents part, ``<plugindir>/header/moo.plugin.js``
for moo'ing in the site header.

In order for this to work, you'll need to have ``blogcpp`` compiled with
``WITH_PLUGINS`` and the configuration variable ``plugins`` set to ``on``.
If you did so, ``blogcpp`` will read all files in the particular folder
under your plug-in directory. If they are - more or less - valid JavaScript
(ECMAScript 5.1 is mostly supported), ``blogcpp`` will try to find the
``process()`` method in them, fill it with the appropriate text and replace
it by the results of the ``process()`` method before continuing as usual.

Plug-ins can also access certain parts of ``blogcpp`` itself from the
``BlogEngine`` object. By the time of writing, those are the following one:

-  ``getVersion()``: Returns the current version of ``blogcpp`` as a number.

Please refer to the ``hello-world.plugin.js`` example plug-in in this
repository for more or less information.


## Can I use raw HTML in my Markdown-enabled article or page?

Yes, you can! Everything between ``<rawhtml>`` and ``</rawhtml>`` will
be ignored by ``blogcpp``'s Markdown parser.


## Which files does a template need to work?

``blogcpp`` needs **index.txt**, **post.txt**, **page.txt** and
**archives.txt** in order to be able to process your site correctly. All
other template files are optional. CSS and image files can be put into a
subfolder named **static**, ``blogcpp`` will automatically copy this
folder to your output directory then.

Starting with version 9, ``blogcpp``'s templates are *mostly* compatible
with the [Jinja2](http://jinja.pocoo.org/) syntax, so porting your
existing Python themes should be rather easy.