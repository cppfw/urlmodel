/*
MIT License

Copyright (c) 2023 Ivan Gagis <igagis@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/* ================ LICENSE END ================ */

#pragma once

#include <map>
#include <string>
#include <vector>

#include <utki/span.hpp>

namespace urlmodel {

struct path_less {
	// allow comparing different types (heterogeneous comparison),
	// for automatic conversion of arguments
	// from std::vector<std::string> to utki::span<std::string>
	using is_transparent = int;

	bool operator()(utki::span<const std::string> a, utki::span<const std::string> b) const noexcept;
};

class url
{
public:
	std::string scheme;

	std::string username;
	std::string password;

	std::string host;
	uint16_t port = 0;

	std::vector<std::string> path;
	std::map<std::string, std::string> query;
	std::string fragment;

	bool operator==(const urlmodel::url& url) const noexcept;

	std::string to_string() const;
};

} // namespace urlmodel
