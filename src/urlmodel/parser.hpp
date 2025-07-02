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

#include <utki/span.hpp>

#include "url.hpp"

namespace urlmodel {

// TODO: why lint complains?
// "error: an exception may be thrown in function 'parser'"
// NOLINTNEXTLINE(bugprone-exception-escape)
class parser
{
	enum class state {
		scheme,
		authority_prefix,
		authority,
		path,
		query_name,
		query_value,
		fragment,
		end
	};

	state cur_state = state::scheme;

	std::vector<uint8_t> buf;

	utki::span<const uint8_t> parse_scheme(utki::span<const uint8_t> data);
	utki::span<const uint8_t> parse_authority_prefix(utki::span<const uint8_t> data);
	utki::span<const uint8_t> parse_authority(utki::span<const uint8_t> data);
	utki::span<const uint8_t> parse_path(utki::span<const uint8_t> data);
	utki::span<const uint8_t> parse_query_name(utki::span<const uint8_t> data);
	utki::span<const uint8_t> parse_query_value(utki::span<const uint8_t> data);
	utki::span<const uint8_t> parse_fragment(utki::span<const uint8_t> data);

	void handle_end_of_authority();
	void handle_end_of_path_segment();
	void handle_end_of_query_value();

	// for storing query name until query value is parsed
	std::string parsed_query_name;

public:
	urlmodel::url url;

	/**
   * @brief Feed data portion to parse.
   * @param data - portion of data to parse.
   * @return span remained after parsing. It can be non-empty in case
   *     URI end has been encountered in the middle of the fed data.
   * @throw std::invalid_argument in case of malformed URL.
   */
	utki::span<const uint8_t> feed(utki::span<const uint8_t> data);

	/**
   * @brief End of input data reached.
   * This function tells parser that end of input data has been reached. The
   * parser state will be set to 'end'. This function doesn't have to be called
   * in case URL is terminated by a whitespace, in this case parser state will
   * be set to 'end' when this whitespace is reached.
   */
	void end_of_data();

	/**
   * @brief Check if end of URL is reached.
   * @return true if end of URL is reached.
   * @return false otherwise.
   */
	bool is_end() const noexcept
	{
		return this->cur_state == state::end;
	}
};

} // namespace urlmodel
