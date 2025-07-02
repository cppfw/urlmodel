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

#include "url.hpp"

#include <sstream>

using namespace urlmodel;

bool url::operator==(const urlmodel::url& url) const noexcept
{
	return this->scheme == url.scheme && this->username == url.username && this->password == url.password &&
		this->host == url.host && this->port == url.port && this->path == url.path && this->query == url.query &&
		this->fragment == url.fragment;
}

std::string url::to_string() const
{
	std::stringstream ss;

	if (!this->scheme.empty()) {
		ss << this->scheme << ':';
		if (!this->host.empty()) {
			ss << "//";
			if (!this->username.empty()) {
				ss << this->username;

				if (!this->password.empty()) {
					ss << ':' << this->password;
				}

				ss << '@';
			}

			ss << this->host;

			if (this->port != 0) {
				ss << ':' << this->port;
			}
		}
	} else {
		// start with path
		ss << '/';
	}

	if (!this->path.empty()) {
		for (const auto& p : this->path) {
			ss << '/' << p;
		}
	}

	if (!this->query.empty()) {
		bool is_first = true;
		for (const auto& q : this->query) {
			if (is_first) {
				is_first = false;
				ss << '?';
			} else {
				ss << '&';
			}

			ss << q.first << '=' << q.second;
		}
	}

	if (!this->fragment.empty()) {
		ss << '#' << this->fragment;
	}

	return ss.str();
}

bool path_less::operator()(utki::span<const std::string> a, utki::span<const std::string> b) const noexcept
{
	auto i = a.begin();
	auto j = b.begin();

	for (; i != a.end() && j != b.end(); ++i, ++j) {
		auto res = i->compare(*j);

		if (res < 0) {
			return true;
		} else if (res > 0) {
			return false;
		}
	}

	if (j != b.end()) {
		return true;
	}

	return false;
}
