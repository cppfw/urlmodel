#include <tst/set.hpp>
#include <tst/check.hpp>

#include <urlmodel/parser.hpp>

namespace{
const tst::set set("urlmodel__parser", [](tst::suite& suite){
    suite.add<std::pair<std::string, urlmodel::url>>(
        "samples",
        {
            {
                "http:",
                urlmodel::url{
                    .scheme = "http"
                }
            },
            {
                "http:/", // no authority, path is just '/'
                urlmodel::url{
                    .scheme = "http"
                }
            },
            {
                "http://", // empty autority
                urlmodel::url{
                    .scheme = "http"
                }
            },
            {
                "http://user:password@",
                urlmodel::url{
                    .scheme = "http",
                    .username = "user",
                    .password = "password"
                }
            },
            {
                "http://user:password@host.com",
                urlmodel::url{
                    .scheme = "http",
                    .username = "user",
                    .password = "password",
                    .host = "host.com"
                }
            },
            {
                "http://user:password@",
                urlmodel::url{
                    .scheme = "http",
                    .username = "user",
                    .password = "password"
                }
            },
            {
                "http://user@host.com",
                urlmodel::url{
                    .scheme = "http",
                    .username = "user",
                    .host = "host.com"
                }
            },
            {
                "http://host.com",
                urlmodel::url{
                    .scheme = "http",
                    .host = "host.com"
                }
            },
            {
                "http://user:password@host.com:8080",
                urlmodel::url{
                    .scheme = "http",
                    .username = "user",
                    .password = "password",
                    .host = "host.com",
                    .port = 8080
                }
            },
            {
                "http://user:password@:8080",
                urlmodel::url{
                    .scheme = "http",
                    .username = "user",
                    .password = "password",
                    .port = 8080
                }
            },
            {
                "http://:8080",
                urlmodel::url{
                    .scheme = "http",
                    .port = 8080
                }
            },
            {
                "http://host.com:8080",
                urlmodel::url{
                    .scheme = "http",
                    .host = "host.com",
                    .port = 8080
                }
            },
            {
                "http://user:password@host.com:8080/path/to/dir",
                urlmodel::url{
                    .scheme = "http",
                    .username = "user",
                    .password = "password",
                    .host = "host.com",
                    .port = 8080,
                    .path = {"path", "to", "dir"}
                }
            },
            {
                "http://user:password@host.com:8080/",
                urlmodel::url{
                    .scheme = "http",
                    .username = "user",
                    .password = "password",
                    .host = "host.com",
                    .port = 8080
                }
            },
            {
                "http://user:password@host.com:8080/path",
                urlmodel::url{
                    .scheme = "http",
                    .username = "user",
                    .password = "password",
                    .host = "host.com",
                    .port = 8080,
                    .path = {"path"}
                }
            },
            {
                "http://user:password@host.com:8080/path/to/dir?param1=value1",
                urlmodel::url{
                    .scheme = "http",
                    .username = "user",
                    .password = "password",
                    .host = "host.com",
                    .port = 8080,
                    .path = {"path", "to", "dir"},
                    .query = {
                        {"param1", "value1"}
                    },
                }
            },
            {
                "http://user:password@host.com:8080?param1=value1&param2=value2",
                urlmodel::url{
                    .scheme = "http",
                    .username = "user",
                    .password = "password",
                    .host = "host.com",
                    .port = 8080,
                    .query = {
                        {"param1", "value1"},
                        {"param2", "value2"}
                    }
                }
            },
            {
                "http://user:password@host.com:8080/?param1=value1&param2=value2",
                urlmodel::url{
                    .scheme = "http",
                    .username = "user",
                    .password = "password",
                    .host = "host.com",
                    .port = 8080,
                    .query = {
                        {"param1", "value1"},
                        {"param2", "value2"}
                    }
                }
            },
            {
                "http://user:password@host.com?param1=value1&param2=value2",
                urlmodel::url{
                    .scheme = "http",
                    .username = "user",
                    .password = "password",
                    .host = "host.com",
                    .query = {
                        {"param1", "value1"},
                        {"param2", "value2"}
                    }
                }
            },
            {
                "http://user:password@host.com:8080/path/to/dir?param1=value1&param2=value2#",
                urlmodel::url{
                    .scheme = "http",
                    .username = "user",
                    .password = "password",
                    .host = "host.com",
                    .port = 8080,
                    .path = {"path", "to", "dir"},
                    .query = {
                        {"param1", "value1"},
                        {"param2", "value2"}
                    }
                }
            },
            {
                "/",
                urlmodel::url{}
            },
            {
                "/path/to/dir",
                urlmodel::url{
                    .path = {"path", "to", "dir"},
                }
            },
            {
                "/path/to/dir?param1=value1",
                urlmodel::url{
                    .path = {"path", "to", "dir"},
                    .query = {
                        {"param1", "value1"}
                    }
                }
            },
            {
                "/path/to/dir?param1=value1&param2=value2",
                urlmodel::url{
                    .path = {"path", "to", "dir"},
                    .query = {
                        {"param1", "value1"},
                        {"param2", "value2"}
                    }
                }
            },
            {
                "/?param1=value1&param2=value2",
                urlmodel::url{
                    .query = {
                        {"param1", "value1"},
                        {"param2", "value2"}
                    }
                }
            },
            {
                "/#fragment",
                urlmodel::url{
                    .fragment = "fragment"
                }
            },
            {
                "/path/to/dir#fragment",
                urlmodel::url{
                    .path = {"path", "to", "dir"},
                    .fragment = "fragment"
                }
            },
            {
                "/path/to/dir?param1=value1&param2=value2#fragment",
                urlmodel::url{
                    .path = {"path", "to", "dir"},
                    .query = {
                        {"param1", "value1"},
                        {"param2", "value2"}
                    },
                    .fragment = "fragment"
                }
            },
            {
                "http://user:password@host.com:8080/path/to/dir?param1=value1&param2=value2#fragment",
                urlmodel::url{
                    .scheme = "http",
                    .username = "user",
                    .password = "password",
                    .host = "host.com",
                    .port = 8080,
                    .path = {"path", "to", "dir"},
                    .query = {
                        {"param1", "value1"},
                        {"param2", "value2"}
                    },
                    .fragment = "fragment"
                }
            }
        },
        [](const auto& p){
            urlmodel::parser parser;

            // TODO: feed by several batches

            auto span = utki::make_span(
                // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
                reinterpret_cast<const uint8_t*>(p.first.data()),
                p.first.size()
            );

            parser.feed(span);
            parser.end_of_data();

            tst::check(parser.is_end(), SL);

            tst::check(parser.url == p.second, SL)
                << "parsed = \n\t" << parser.url.to_string() << "\n"
                << "expected = \n\t" << p.second.to_string() << "\n";
        }
    );
});
}
