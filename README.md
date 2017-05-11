# BoostLogUsage
Output log information with boost library

# Boost Installation

[Boost Install](https://www.zybuluo.com/breakerthb/note/576587)

----

boost log 使用

# 1. 最平常的写日志

```cpp
#include <boost/log/trivial.hpp>

namespace logging = boost::log;

int main()
{
    BOOST_LOG_TRIVIAL(trace) << "A trace severity message";
    BOOST_LOG_TRIVIAL(debug) << "A debug severity message";
    BOOST_LOG_TRIVIAL(info) << "An informational severity message";
    BOOST_LOG_TRIVIAL(warning) << "A warning severity message";
    BOOST_LOG_TRIVIAL(error) << "An error severity message";
    BOOST_LOG_TRIVIAL(fatal) << "A fatal severity message";

    return 0;
}
```

信息会打印到控制台，类似std::cout，但优势如下：

- 不光有日志信息，输出还包含时间戳，当前线程标识，安全等级
- 不同线程同时写日志是安全的
- 可以应用过滤

编译：

    $ g++ -g -std=c++11 -Wall -DBOOST_ALL_DYN_LINK -DBOOST_LOG_DYN_LINK -o test.o -c test.cpp
    $ g++ -g -std=c++11 test.o -o test  -rdynamic -lpthread -lboost_log_setup  -lboost_log -lboost_filesystem  -lboost_system -lboost_thread

执行结果：

![](https://raw.githubusercontent.com/breakerthb/CloudIDE_WB/master/PicBed/2017/1494244488.png)

添加过滤日志：

# 2. 添加过滤日志

```cpp
#include <iostream>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>

using namespace std;
namespace logging = boost::log;

void InitFilter()
{
    logging::core::get()->set_filter
    (
        logging::trivial::severity >= logging::trivial::info
    );
}

void UpdateFilter() {
  logging::core::get()->set_filter
  (
      logging::trivial::severity >= logging::trivial::debug
  );
}

int main()
{
    InitFilter();

    BOOST_LOG_TRIVIAL(trace) << "A trace severity message";
    BOOST_LOG_TRIVIAL(debug) << "A debug severity message";
    BOOST_LOG_TRIVIAL(info) << "An informational severity message";
    BOOST_LOG_TRIVIAL(warning) << "A warning severity message";
    BOOST_LOG_TRIVIAL(error) << "An error severity message";
    BOOST_LOG_TRIVIAL(fatal) << "A fatal severity message";

    UpdateFilter();

    cout << "---- ----" << endl;

    BOOST_LOG_TRIVIAL(trace) << "A trace severity message";
    BOOST_LOG_TRIVIAL(debug) << "A debug severity message";
    BOOST_LOG_TRIVIAL(info) << "An informational severity message";
    BOOST_LOG_TRIVIAL(warning) << "A warning severity message";
    BOOST_LOG_TRIVIAL(error) << "An error severity message";
    BOOST_LOG_TRIVIAL(fatal) << "A fatal severity message";

    return 0;
}
```

编译：

    $ g++ -g -std=c++11 -Wall -DBOOST_ALL_DYN_LINK -DBOOST_LOG_DYN_LINK -o test.o -c test.cpp
    $ g++ -g -std=c++11 test.o -o test  -rdynamic -lpthread -lboost_log_setup  -lboost_log -lboost_filesystem  -lboost_system -lboost_thread

执行结果：

![](https://raw.githubusercontent.com/breakerthb/CloudIDE_WB/master/PicBed/2017/1494246009.png)

# 3. Setting up sinks

sink由两个类组成：

- frontend

负责所有sink的各种常见任务，如线程同步模型，过滤，针对text-based sink的格式化；

- backend

实现特定sink的所有事情，如写到一个文件。

synchronous_sink：支持多线程同时写日志，必要时会自动阻塞。
text_ostream_backend类可以将格式化的日志项写到STL兼容的stream。例：

```cpp
#include <boost/core/null_deleter.hpp>

// We have to provide an empty deleter to avoid destroying the global stream object
boost::shared_ptr< std::ostream > stream(&std::clog, boost::null_deleter());
sink->locked_backend()->add_stream(stream);
```

text_ostream_backend支持添加多个流。对于这个sink来说这种方式可以复制输出到控制台和文件，而这个过程中对于每个日志项过滤/格式化和其他类库只有一次开销。
locked_backend成员函数调用sink的backend。它被用来得到一个对backend的线程安全的调用，所有sink fronteds可以提供改成员函数。这个函数返回一个智能指针，并且只要它存在，backend就是被锁的。只有一个例外，unlocked_sink frontend，它不是同步的，而且只是简单返回一个未锁的指向backend的指针。

# 4.Creating loggers and writing logs

## 4.1 创建logger对象

src::logger lg;
类库提供了两个版本的logger：线程安全/非线程安全。

- 非线程安全的logger当使用不同logger实例写日志时是安全的。也就也意味着每个线程都要建立一个logger实例。
- 线程安全的logger可以多个线程同时写，但是这个过程会包含锁，降低效率。线程安全的logger类型都带有_mt后缀。

## 4.2 全局logger对象

BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(my_logger, src::logger_mt)
src::logger_mt& lg = my_logger::get(); //get本身是线程安全的，不用添加同步

## 4.3 写日志

	logging::record rec = lg.open_record();
	if (rec)
	{
		logging::record_ostream strm(rec);
		strm << "Hello, World!";
		strm.flush();
		lg.push_record(boost::move(rec));
	}

或者使用宏定义

 	BOOST_LOG(lg) << "Hello, World!";

全部例子

```cpp
/*
*          Copyright Andrey Semashev 2007 - 2015.
* Distributed under the Boost Software License, Version 1.0.
*    (See accompanying file LICENSE_1_0.txt or copy at
*          http://www.boost.org/LICENSE_1_0.txt)
*/
#include <boost/move/utility.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace keywords = boost::log::keywords;

BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(my_logger, src::logger_mt)

void logging_function1()
{
    src::logger lg;

//[ example_tutorial_logging_manual_logging
    logging::record rec = lg.open_record();
    if (rec)
    {
        logging::record_ostream strm(rec);
        strm << "Hello, World!";
        strm.flush();
        lg.push_record(boost::move(rec));
    }
//]
}

void logging_function2()
{
    src::logger_mt& lg = my_logger::get();
    BOOST_LOG(lg) << "Greetings from the global logger!";
}

int main(int, char*[])
{
    logging::add_file_log("sample.log");
    logging::add_common_attributes();

    logging_function1();
    logging_function2();

    return 0;
}
```

编译链接：

    g++ -g -std=c++11 -Wall -DBOOST_ALL_DYN_LINK -DBOOST_LOG_DYN_LINK -o ./main.o -c ./main.cc
    g++ -g -std=c++11 ./main.o -o main -rdynamic -lpthread -lboost_log_setup -lboost_log -lboost_filesystem -lboost_system -lboost_thread 

 
# 5. Adding more information to log: Attributes
 
    logging::add_common_attributes();

添加LineID,TimeStamp,ProcessID,ThreadID

其相当于如下代码：

```cpp
void add_common_attributes()
{
	boost::shared_ptr< logging::core > core = logging::core::get();
	core->add_global_attribute("LineID", attrs::counter< unsigned int >(1));
	core->add_global_attribute("TimeStamp", attrs::local_clock());

	// other attributes skipped for brevity
}
```

可以为参数定义占位符

```cpp
BOOST_LOG_ATTRIBUTE_KEYWORD(line_id, "LineID", unsigned int)
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", severity_level)
BOOST_LOG_ATTRIBUTE_KEYWORD(tag_attr, "Tag", std::string)
BOOST_LOG_ATTRIBUTE_KEYWORD(scope, "Scope", attrs::named_scope::value_type)
BOOST_LOG_ATTRIBUTE_KEYWORD(timeline, "Timeline", attrs::timer::value_type)
```

# 6. Log record formatting

## 6.1 Lambda-style formatters 

### 示例1：

```cpp
void init()
{
	logging::add_file_log
	(
	    keywords::file_name = "sample_%N.log",
	    // This makes the sink to write log records that look like this:
	    // 1:  A normal severity message
	    // 2:  An error severity message
	    keywords::format =
	    (
	        expr::stream
	            << expr::attr< unsigned int >("LineID")
	            << ": <" << logging::trivial::severity
	            << "> " << expr::smessage
	    )
	);
}
```

### 示例2

```cpp
void init()
{
	typedef sinks::synchronous_sink< sinks::text_ostream_backend > text_sink;
	boost::shared_ptr< text_sink > sink = boost::make_shared< text_sink >();

	sink->locked_backend()->add_stream(
	    boost::make_shared< std::ofstream >("sample.log"));

	sink->set_formatter
	(
	    expr::stream
	           // line id will be written in hex, 8-digits, zero-filled
	        << std::hex << std::setw(8) << std::setfill('0') << expr::attr< unsigned int >("LineID")
	        << ": <" << logging::trivial::severity
	        << "> " << expr::smessage
	);

	logging::core::get()->add_sink(sink);
}
```

## 6.2 Boost.Format-style formatters

### 示例1：

```cpp
void init()
{
    typedef sinks::synchronous_sink< sinks::text_ostream_backend > text_sink;
    boost::shared_ptr< text_sink > sink = boost::make_shared< text_sink >();

    sink->locked_backend()->add_stream(
        boost::make_shared< std::ofstream >("sample.log"));

    // This makes the sink to write log records that look like this:
    // 1:  A normal severity message
    // 2:  An error severity message
    sink->set_formatter
    (
        expr::format("%1%: <%2%> %3%")
            % expr::attr< unsigned int >("LineID")
            % logging::trivial::severity
            % expr::smessage
    );

    logging::core::get()->add_sink(sink);
}
```

# 7. Filtering revisited

示例：

```cpp
BOOST_LOG_ATTRIBUTE_KEYWORD(line_id, "LineID", unsigned int)
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", severity_level)
BOOST_LOG_ATTRIBUTE_KEYWORD(tag_attr, "Tag", std::string)

void init()
{
    // Setup the common formatter for all sinks
    logging::formatter fmt = expr::stream
        << std::setw(6) << std::setfill('0') << line_id << std::setfill(' ')
        << ": <" << severity << ">\t"
        << expr::if_(expr::has_attr(tag_attr))
            [
                expr::stream << "[" << tag_attr << "] "
            ]
        << expr::smessage;

    // Initialize sinks
    typedef sinks::synchronous_sink< sinks::text_ostream_backend > text_sink;
    boost::shared_ptr< text_sink > sink = boost::make_shared< text_sink >();

    sink->locked_backend()->add_stream(
        boost::make_shared< std::ofstream >("full.log"));

    sink->set_formatter(fmt);

    logging::core::get()->add_sink(sink);

    sink = boost::make_shared< text_sink >();

    sink->locked_backend()->add_stream(
        boost::make_shared< std::ofstream >("important.log"));

    sink->set_formatter(fmt);

    sink->set_filter(severity >= warning || (expr::has_attr(tag_attr) && tag_attr == "IMPORTANT_MESSAGE"));

    logging::core::get()->add_sink(sink);

    // Add attributes
    logging::add_common_attributes();
}
void logging_function()
{
    src::severity_logger< severity_level > slg;

    BOOST_LOG_SEV(slg, normal) << "A regular message";
    BOOST_LOG_SEV(slg, warning) << "Something bad is going on but I can handle it";
    BOOST_LOG_SEV(slg, critical) << "Everything crumbles, shoot me now!";

    {
        BOOST_LOG_SCOPED_THREAD_TAG("Tag", "IMPORTANT_MESSAGE");
        BOOST_LOG_SEV(slg, normal) << "An important message";
    }
}

int main(int, char*[])
{
    init();

    logging_function();

    return 0;
}
```

示例中初始化了两个sink，两个设置了相同的记录格式。第二个设置了过滤，要求级别大于warning或者包含tag参数，且参数为IMPORTANT_MESSAGE。在调用logger对象写日志的时候可以用BOOST_LOG_SCOPED_THREAD_TAG("Tag", "IMPORTANT_MESSAGE");设置tag。

# Demo

Ref : <https://github.com/breakerthb/BoostLogUsage/blob/master/BoostLogTest>
